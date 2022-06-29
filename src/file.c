#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <math.h>

#include "HEADERS/MGF.h"

#define COMP_START	32
#define COMP_END	126

//draw.c
unsigned short getIntLength(int num);

unsigned short getLineCount(char* fileName)
{

	FILE* file = NULL;

	char currentLine[256];
	unsigned short lineCount = 0;

	file = fopen(fileName, "r");
	if (file != NULL)
	{

		while (fgets(currentLine, sizeof(currentLine), file) != NULL)
			lineCount++;

		fclose(file);

		return lineCount;

	}
	else
		return 0;

}

bool fileExists(char* fileName)
{

	struct stat buffer;
	return (stat(fileName, &buffer) == 0);

}

void createOptions()
{

	FILE* optionsFile = NULL;

	//Create SAVES folder if it does not exist
	struct stat st = {0};
	if (stat("SAVES", &st) == -1)
	{

		#ifdef __unix__
			mkdir("SAVES", 0700);
		#endif
		#ifdef _WIN32
			mkdir("SAVES");
		#endif

	}

	//Create options file and fill it with default data
	optionsFile = fopen("SAVES/options.cfg", "w");
	if (optionsFile != NULL)
	{

		fprintf(optionsFile, "FULLSCREEN=0\n");
		fprintf(optionsFile, "VOLUME=10\n");
		fprintf(optionsFile, "LIMIT FPS=1");

		fclose(optionsFile);

	}

}

//Return the name of the option at the specified line in the options file
	//Returned value should be freed to avoid memory leaks
	//Returns NULL if the specified line is larger than the number of lines in the file
char* getOptionName(int line)
{

	if (line > getLineCount("SAVES/options.cfg"))
		return NULL;

	FILE* optionsFile = NULL;
	char currentLine[256];

	char* returnString = NULL;
	
	optionsFile = fopen("SAVES/options.cfg", "r");
	if (optionsFile != NULL)
	{

		int i = 0;
		while (fgets(currentLine, sizeof(currentLine), optionsFile) != NULL)
		{

			if (i == line)
			{

				//Get length of name
				int len = 0;
				while (currentLine[len] != '=')
					len++;

				//Copy name into returnString
				returnString = calloc(len + 1, sizeof(char));
				returnString[len] = '\0';
				for (unsigned short i = 0; i < len; i++)
					returnString[i] = currentLine[i];

				break;

			}
			else
				i++;

		}

		fclose(optionsFile);

	}

	return returnString;

}

//Return the value for the options specifed by the given strings
	// Returns -1 if option is not found
int getOptionValue(const char* str)
{

	FILE* optionsFile = NULL;

	char currentLine[256];

	char* returnValue = NULL;

	//Open file for reading
	optionsFile = fopen("SAVES/options.cfg", "r");
	if (optionsFile != NULL)
	{

		while (fgets(currentLine, sizeof(currentLine), optionsFile) != NULL)
		{

			//Check if the currentLine contains the option we're looking for
			bool matches = true;
			for (unsigned short i = 0; i < SDL_strlen(str); i++)
			{

				if (currentLine[i] != str[i])
				{

					matches = false;
					break;

				}
				
			}

			if (matches == true)
			{

				int i = SDL_strlen(str);

				//Skip all spaces and equal signs after the option name
				while (currentLine[i] == ' ' || currentLine[i] == '=')
					i++;

				//Find the start_index and length of the value string
					//Count until a character that is not a number and not a negative sign
					//is reached
				int value_start_index = i;
				while (isdigit(currentLine[i]) || currentLine[i] == '-')
					i++;
				int value_length = i - value_start_index;

				//Create string to store value in
				char* value = calloc((value_length + 1), sizeof(char));
				value[value_length] = '\0';

				//Copy value from currentLine into value string
				for (int j = value_start_index; j < i; j++)
					value[j - value_start_index] = currentLine[j];

				returnValue = value;

			}

		}

		fclose(optionsFile);

	}

	if (returnValue == NULL)
		return -1;
	else
	{

		//Copy returnValue over into a non-dynamically allocated string
		char non_dynamic[strlen(returnValue) + 1];
		strcpy(non_dynamic, returnValue);

		//That way we can free the memory taken up by runtime_string and avoid
		//memory leaks
		free(returnValue);

		//Return runtime_string as an integer
		return SDL_atoi(non_dynamic);

	}

}

//Return true if there are any irregularities with the options file
	//This means if any options have irregular values
	//It also checks if there are the right amount of lines in the file
bool brokenOptions()
{

	if (getLineCount("SAVES/options.cfg") != 3)
		return true;

	int fullscreen_value = getOptionValue("FULLSCREEN");
	int volume_value = getOptionValue("VOLUME");
	int fps_value = getOptionValue("LIMIT FPS");

	if (fullscreen_value != 0 && fullscreen_value != 1)
		return true;
	if (volume_value < 0 || volume_value > 100)
		return true;
	if (fps_value != 0 && fps_value != 1)
		return true;

	return false;

}

//Function for saving the value specified by value to the option specified by str
void saveOption(const char* str, int value)
{
	
	FILE* optionsFile = NULL;

	char currentLine[256];
	unsigned short count = 0;

	unsigned short lineCount = getLineCount("SAVES/options.cfg");

	char** fileLines = calloc(lineCount, sizeof(char*));
	
	optionsFile = fopen("SAVES/options.cfg", "r");
	if (optionsFile != NULL && fileLines != NULL)
	{

		while (fgets(currentLine, sizeof(currentLine), optionsFile) != NULL)
		{
		
			//Dont count any newlines
			if (currentLine[0] != '\n')
			{

				//Get the name of the option at the current line
				char* currName = getOptionName(count);

				if (strcmp(currName, str) == 0)	//If it is the option we're editing
				{

					//Allocate memory for rewriting the line
					int len = SDL_strlen(currName) + SDL_strlen("=") + getIntLength(value);
					fileLines[count] = calloc(len + 1, sizeof(char));

					//Convert value to string
					char valueAsString[getIntLength(value)];
					SDL_itoa(value, valueAsString, 10);

					//Combine all aspects into a single string
					strcpy(fileLines[count], currName);
					strcat(fileLines[count], "=");
					strcat(fileLines[count], valueAsString);

				}
				else	//If its not the option we're looking for, just store it as-is in
				{		//in fileLines

					//Remove newline at end of currentLine
					if (currentLine[SDL_strlen(currentLine)] == '\n')
						currentLine[SDL_strlen(currentLine)] = '\0';
					else if (currentLine[SDL_strlen(currentLine) - 1] == '\n')
						currentLine[SDL_strlen(currentLine) - 1] = '\0';

					//Copy currentLine intuo fileLines array
					fileLines[count] = calloc(SDL_strlen(currentLine) + 1, sizeof(char));
					strcpy(fileLines[count], currentLine);

				}

				free(currName);
				
				count++;

			}
			
		}

		fclose(optionsFile);

	}
	
	//Rewrite all the lines back to the file
	optionsFile = fopen("SAVES/options.cfg", "w");
	if (optionsFile != NULL && fileLines != NULL)
	{
		
		for (unsigned short i = 0; i < lineCount; i++)
			fprintf(optionsFile, "%s\n", fileLines[i]);

		fclose(optionsFile);

	}

	//Free memory taken up by fileLines
	for (unsigned short i = 0; i < lineCount; i++)
		free(fileLines[i]);
	free(fileLines);
	fileLines = NULL;
	
}

unsigned int loadTop()
{

	unsigned int score = 0;
	int c;

	FILE* topFile = NULL;

	if (!fileExists("SAVES/top.md"))
		return 0;
	else
		topFile = fopen("SAVES/top.md", "r");

	if (topFile != NULL)
	{

		//Decrypt the score
		while ((c = getc(topFile)) != EOF)	
			score = (score * 10) + (c - COMP_START);

		fclose(topFile);
		return score;

	}
	else
		return 0;

}

void saveTop(unsigned int score)
{

	FILE* topFile = NULL;

	//Create file if it does not exist, open it if it does
	topFile = fopen("SAVES/top.md", "w");

	if (topFile != NULL)
	{

		unsigned short length = (unsigned short)(floor(log10(abs(score))) + 1);

		//Convert integer to a string
		char* string = malloc((length + 1) * sizeof(*string));
		if (string != NULL)
		{

			//Store each digit from num into string in reverse order
			unsigned short count = 0;
			while (score)
			{

				string[length - count - 1] = '0' + (score % 10);

				count = count + 1;
				score /= 10;

			}

			//End the string with a end-character
			string[length] = '\0';

			//Encrypt the score
			for (unsigned short i = 0; i < length; i++)
				*(string + i) = ((*(string + i) - '0') % (COMP_END - COMP_START) + COMP_START);

		}
		
		//Print score to topFile
		if (string != 0)
			for (unsigned short i = 0; i < length; i++)
				fprintf(topFile, "%c", *(string + i));

		free(string);
		fclose(topFile);

	}

}