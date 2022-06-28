#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <math.h>

#include "HEADERS/MGF.h"

#define COMP_START	32
#define COMP_END	126

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
		fprintf(optionsFile, "VOLUME=100\n");
		fprintf(optionsFile, "LIMIT FPS=1");

		fclose(optionsFile);

	}

}

void saveOption(unsigned short line, unsigned short value)
{
	
	FILE* optionsFile = NULL;

	char currentLine[2];
	unsigned short count = 0;

	unsigned short lineCount = getLineCount("SAVES/options.cfg");

	char* fileLines = NULL;
	if (fileLines == NULL)
		fileLines = malloc(lineCount * sizeof(*fileLines));
	
	//Save all lines of file 
	optionsFile = fopen("SAVES/options.cfg", "r");
	if (optionsFile != NULL && fileLines != NULL)
	{

		while (fgets(currentLine, sizeof(currentLine), optionsFile) != NULL)
		{
		
			//Dont count any newlines
			if (currentLine[0] != 10)
			{
				
				//Edit the line that we are saving
				if (count == line)
					*(fileLines + count) = '0' + value;
				else
					*(fileLines + count) = currentLine[0];
				
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
			fprintf(optionsFile, "%c\n", *(fileLines + i));

		fclose(optionsFile);

	}

	free(fileLines);
	fileLines = NULL;
	
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

				//Skip all non-alphanumeric characters after the option name
				while (!isalpha(currentLine[i]) && !isdigit(currentLine[i]))
					i++;

				//Find the start_index and length of the value string
					//Count until a non-numeric character is reached
				int value_start_index = i;
				while (isdigit(currentLine[i]))
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
		return SDL_atoi(returnValue);

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