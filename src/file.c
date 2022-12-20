#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "HEADERS/MGF.h"

#define COMP_START	32
#define COMP_END	126

//draw.c
unsigned short getIntLength(int num);

//controls.c
bool invalidKey(int key);

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

void createWindowFile()
{

	FILE* windowFile = NULL;

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

	//Create window file and fill it with default data
	windowFile = fopen("SAVES/window.cfg", "w");
	if (windowFile != NULL)
	{

		fprintf(windowFile, "WIDTH=%d\n", globalInstance->DM.w / 2);
		fprintf(windowFile, "HEIGHT=%d\n", globalInstance->DM.h / 2);
		fprintf(windowFile, "X=%d\n", globalInstance->DM.w / 4);
		fprintf(windowFile, "Y=%d", globalInstance->DM.h / 4);

		fclose(windowFile);

	}

}

void createControls()
{

	FILE* controlsFile = NULL;

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

	//Create controls file and fill it with default data
	controlsFile = fopen("SAVES/controls.cfg", "w");
	if (controlsFile != NULL)
	{

		fprintf(controlsFile, "LEFT=80\n");
		fprintf(controlsFile, "RIGHT=79\n");
		fprintf(controlsFile, "SOFT DROP=81\n");
		fprintf(controlsFile, "HARD DROP=82\n");
		fprintf(controlsFile, "HOLD=44\n");
		fprintf(controlsFile, "ROTATE CCW=29\n");
		fprintf(controlsFile, "ROTATE CW=27\n");
		fprintf(controlsFile, "MIRROR=6\n");
		fprintf(controlsFile, "SELECT=40\n");
		fprintf(controlsFile, "EXIT=41\n");
		fprintf(controlsFile, "DOWN=81\n");
		fprintf(controlsFile, "UP=82\n");

		fclose(controlsFile);

	}

}

//Return the name, the part before the =, of something saved in a file at the specified line in the file
	//Returned value should be freed to avoid memory leaks
	//Returns NULL if the specified line is larger than the number of lines in the file
char* getNameAtLine(const char* file_path, int line)
{

	if (line > getLineCount((char*)file_path))
		return NULL;

	FILE* file = NULL;
	char currentLine[256];

	char* returnString = NULL;
	
	file = fopen(file_path, "r");
	if (file != NULL)
	{

		int i = 0;
		while (fgets(currentLine, sizeof(currentLine), file) != NULL)
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

		fclose(file);

	}

	return returnString;

}

//Return the value for the specified name in the specified file
	//Returns INT_MAX on error
int getFileValue(const char* file_path, const char* name)
{

	FILE* file = NULL;

	char currentLine[256];

	char* returnValue = NULL;

	//Open file for reading
	file = fopen(file_path, "r");
	if (file != NULL)
	{

		while (fgets(currentLine, sizeof(currentLine), file) != NULL)
		{

			//Check if the currentLine contains the name we're looking for
			bool matches = true;
			for (unsigned short i = 0; i < SDL_strlen(name); i++)
			{

				if (currentLine[i] != name[i])
				{

					matches = false;
					break;

				}
				
			}

			if (matches == true)
			{

				int i = SDL_strlen(name);

				//Skip all spaces and equal signs after the name
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

		fclose(file);

	}

	if (returnValue != NULL)
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
	else	
		return INT_MAX;

}

//Return true if there are any irregularities with the options file
	//This means if any options have irregular values
	//It also checks if there are the right amount of lines in the file
bool brokenOptions()
{

	if (getLineCount("SAVES/options.cfg") != 3)
		return true;

	int fullscreen_value = getFileValue("SAVES/options.cfg", "FULLSCREEN");
	int volume_value = getFileValue("SAVES/options.cfg", "VOLUME");
	int fps_value = getFileValue("SAVES/options.cfg", "LIMIT FPS");

	if (fullscreen_value != 0 && fullscreen_value != 1)
		return true;
	if (volume_value < 0 || volume_value > 100)
		return true;
	if (fps_value != 0 && fps_value != 1)
		return true;

	return false;

}

//Check if there is the wrong number of lines in the controls file or if any of the controls are set to
//invalid keys
bool brokenControls()
{

	if (getLineCount("SAVES/controls.cfg") != NUM_OF_CONTROLS)
		return true;

	if (invalidKey(getFileValue("SAVES/controls.cfg", "LEFT")))
		return true;
	if (invalidKey(getFileValue("SAVES/controls.cfg", "RIGHT")))
		return true;
	if (invalidKey(getFileValue("SAVES/controls.cfg", "SOFT DROP")))
		return true;
	if (invalidKey(getFileValue("SAVES/controls.cfg", "HARD DROP")))
		return true;
	if (invalidKey(getFileValue("SAVES/controls.cfg", "HOLD")))
		return true;
	if (invalidKey(getFileValue("SAVES/controls.cfg", "ROTATE CCW")))
		return true;
	if (invalidKey(getFileValue("SAVES/controls.cfg", "ROTATE CW")))
		return true;
	if (invalidKey(getFileValue("SAVES/controls.cfg", "MIRROR")))
		return true;
	if (invalidKey(getFileValue("SAVES/controls.cfg", "SELECT")))
		return true;
	if (invalidKey(getFileValue("SAVES/controls.cfg", "EXIT")))
		return true;
	if (invalidKey(getFileValue("SAVES/controls.cfg", "DOWN")))
		return true;
	if (invalidKey(getFileValue("SAVES/controls.cfg", "UP")))
		return true;

	return false;

}

//Check if there is an invalid size or position saved in the window file
bool brokenWindowFile()
{

	if (getLineCount("SAVES/window.cfg") != 4)
		return true;

	if (getFileValue("SAVES/window.cfg", "WIDTH") <= 0 || 
		getFileValue("SAVES/window.cfg", "WIDTH") > globalInstance->DM.w)
		return true;
	if (getFileValue("SAVES/window.cfg", "X") < -1 * globalInstance->DM.w || 
		getFileValue("SAVES/window.cfg", "X") > globalInstance->DM.w)
		return true;
	if (getFileValue("SAVES/window.cfg", "HEIGHT") <= 0 ||
		getFileValue("SAVES/window.cfg", "HEIGHT") > globalInstance->DM.h)
		return true;
	if (getFileValue("SAVES/window.cfg", "Y") <= -1 * globalInstance->DM.h ||
		getFileValue("SAVES/window.cfg", "Y") > globalInstance->DM.h)
		return true;

	return false;

}

//Function for a value to a name in a file
void saveToFile(const char* file_path, const char* str, int value)
{
	
	FILE* file = NULL;

	char currentLine[256];
	unsigned short count = 0;

	unsigned short lineCount = getLineCount((char*)file_path);

	char** fileLines = calloc(lineCount, sizeof(char*));
	
	file = fopen(file_path, "r");
	if (file != NULL && fileLines != NULL)
	{

		while (fgets(currentLine, sizeof(currentLine), file) != NULL)
		{
		
			//Dont count any newlines
			if (currentLine[0] != '\n')
			{

				//Get the name at the current line
				char* currName = getNameAtLine(file_path, count);

				if (strcmp(currName, str) == 0)	//If it is the name we're editing
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
				else	//If its not the name we're looking for, just store it as-is in
				{		//in fileLines

					//Remove newline at end of currentLine
					if (currentLine[SDL_strlen(currentLine)] == '\n')
						currentLine[SDL_strlen(currentLine)] = '\0';
					else if (currentLine[SDL_strlen(currentLine) - 1] == '\n')
						currentLine[SDL_strlen(currentLine) - 1] = '\0';

					//Copy currentLine into fileLines array
					fileLines[count] = calloc(SDL_strlen(currentLine) + 1, sizeof(char));
					strcpy(fileLines[count], currentLine);

				}

				free(currName);
				
				count++;

			}
			
		}

		fclose(file);

	}
	
	//Rewrite all the lines back to the file
	file = fopen(file_path, "w");
	if (file != NULL && fileLines != NULL)
	{
		
		for (unsigned short i = 0; i < lineCount; i++)
			fprintf(file, "%s\n", fileLines[i]);

		fclose(file);

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

void saveWindowSettings()
{

	SDL_GetWindowSize(globalInstance->window, &globalInstance->minimizedWindow_W, 
						&globalInstance->minimizedWindow_H);
	SDL_GetWindowPosition(globalInstance->window, &globalInstance->minimizedWindow_X, 
							&globalInstance->minimizedWindow_Y);

	saveToFile("SAVES/window.cfg", "WIDTH", globalInstance->minimizedWindow_W);
	saveToFile("SAVES/window.cfg", "HEIGHT", globalInstance->minimizedWindow_H);
	saveToFile("SAVES/window.cfg", "X", globalInstance->minimizedWindow_X);
	saveToFile("SAVES/window.cfg", "Y", globalInstance->minimizedWindow_Y);

}