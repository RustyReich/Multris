#include <sys/stat.h>
#include <limits.h>
#include <stdio.h>

//Fix for cross-compiling mkdir() for windows on linux
#ifdef _WIN32
#include <windows.h>
#define mkdir(dir, mode) mkdir(dir)
#endif


#include "HEADERS/MGF.h"

//draw.c
unsigned short getIntLength(int num);

//controls.c
bool invalidKey(int key);

//Get the number of lines in a file
unsigned short getLineCount(char* fileName)
{

	FILE* file = NULL;

	char currentLine[256];
	unsigned short lineCount = 0;

	file = fopen(fileName, "r");
	if (file != NULL)
	{

		//Increase count for every line in the final
		while (fgets(currentLine, sizeof(currentLine), file) != NULL)
			lineCount++;

		//Close file stream
		fclose(file);

		return lineCount;

	}
	else
		return 0;

}

//Return whether a file exist with the specified name
bool fileExists(char* fileName)
{

	struct stat buffer;
	return (stat(fileName, &buffer) == 0);

}

//Function for creating a new file at the given path
	//This will erase the current file if it already exists.
void createFile(char* file_path)
{

	FILE *file;

	//Get length of file name
	int file_name_length = 0;
	for (int i = SDL_strlen(file_path) - 1; i >= 0; i--)
		if (file_path[i] != '/')
			file_name_length++;
		else
			break;

	//Get name of file as string as well as directory path
	char* file_name = SDL_calloc(file_name_length + 1, sizeof(char));
	char* file_dir = SDL_calloc(SDL_strlen(file_path) - file_name_length + 1, sizeof(char));
	SDL_strlcpy(file_name, (file_path + (SDL_strlen(file_path) - file_name_length)), file_name_length + 1);
	SDL_strlcpy(file_dir, file_path, SDL_strlen(file_path) - file_name_length);

	//Create all directories and intermediate directories needed in order to create the file
	char tmp[256];
    char *p = NULL;
    size_t len;
    snprintf(tmp, sizeof(tmp), "%s", file_dir);
    len = strlen(tmp);
    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;
    for (p = tmp + 1; *p; p++) {

        if (*p == '/') {

            *p = 0;
            mkdir(tmp, S_IRWXU);
            *p = '/';

        }

	}
    mkdir(tmp, S_IRWXU);

	//Finally, create the actual file
	file = fopen(file_path, "w");
	fclose(file);

	SDL_free(file_name);
	SDL_free(file_dir);

}

//Function for creating the "progress.md" file
void createProgressFile()
{

	createFile("SAVES/progress.md");

	//Open progressFile and fill it with default data
	FILE* progressFile = fopen("SAVES/progress.md", "w");
	if (progressFile != NULL)
	{

		fprintf(progressFile, "progress=1");
		fclose(progressFile);

	}

}

void createCustomTopFile()
{

	createFile("SAVES/custom_top.md");

	// Open customTopFile and fill it with default data
	FILE* customTopFile = fopen("SAVES/custom_top.md", "w");
	if (customTopFile != NULL) 
	{

		for (unsigned short i = 0; i <= MAX_CUSTOM_SIZE; i++)
			if (i != MAX_CUSTOM_SIZE)
				fprintf(customTopFile, "%d=0\n", i);
			else
				fprintf(customTopFile, "%d=0", i);

		fclose(customTopFile);

	}

}

//Function for creating the "top.md" file
void createTopFile()
{

	createFile("SAVES/top.md");

	//Open topFile and fill it with default data
	FILE* topFile = fopen("SAVES/top.md", "w");
	if (topFile != NULL) 
	{

		for (unsigned short i = 0; i <= MAX_PIECE_SIZE; i++)
			if (i != MAX_PIECE_SIZE)
				fprintf(topFile, "%d=0\n", i);
			else
				fprintf(topFile, "%d=0", i);

		fclose(topFile);

	}

}

//Function for creating the "options.cfg" file
void createOptions()
{

	createFile("SAVES/options.cfg");

	//Open optionsFile and fill it with default data
	FILE* optionsFile = fopen("SAVES/options.cfg", "w");
	if (optionsFile != NULL)
	{

		fprintf(optionsFile, "FULLSCREEN=0\n");
		fprintf(optionsFile, "VOLUME=10\n");
		fprintf(optionsFile, "LIMIT FPS=1");

		fclose(optionsFile);

	}

}

//Function for creating the "window.cfg" file
void createWindowFile()
{

	createFile("SAVES/window.cfg");

	//Open windowFile and fill it with default data
	FILE* windowFile = fopen("SAVES/window.cfg", "w");
	if (windowFile != NULL)
	{

		fprintf(windowFile, "WIDTH=%d\n", globalInstance->DM.w / 2);
		fprintf(windowFile, "HEIGHT=%d\n", globalInstance->DM.h / 2);
		fprintf(windowFile, "X=%d\n", globalInstance->DM.w / 4);
		fprintf(windowFile, "Y=%d", globalInstance->DM.h / 4);

		fclose(windowFile);

	}

}

//Function for creating the "controls.cfg"
void createControls()
{

	createFile("SAVES/controls.cfg");

	//Open controlsFile and fill it with default data
	FILE* controlsFile = fopen("SAVES/controls.cfg", "w");
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

	//Return NULL if the line number is larger than the number of lines in the file
	if (line > getLineCount((char*)file_path))
		return NULL;

	FILE* file = NULL;
	char currentLine[256];

	char* returnString = NULL;
	
	//Open file
	file = fopen(file_path, "r");
	if (file != NULL)
	{

		//Iterate through lines until we get to the specified line
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
				returnString = SDL_calloc(len + 1, sizeof(char));
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
			for (unsigned short i = 0; i < SDL_strlen(currentLine); i++)
			{

				// If we find a '=', that means we are no longer in the "name" part of the line
				if (currentLine[i] == '=')
					break;

				if (currentLine[i] != name[i])
				{

					matches = false;
					break;

				}
				
			}

			//Once we find the name we're looking for
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
				char* value = SDL_calloc((value_length + 1), sizeof(char));
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

		int returnInt = SDL_atoi(returnValue);
		
		SDL_free(returnValue);

		return returnInt;

	}
	else	
		return INT_MAX;

}

//Return true if there are irregularities with the progress file
	//This means that the players progress is saved as being less than 1 or greater than 8
bool brokenProgress()
{

	if (getLineCount("SAVES/progress.md") != 1)
		return true;

	int progress_value = getFileValue("SAVES/progress.md", "progress");

	if (progress_value < 1 || progress_value > 8)
		return true;

	return false;

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

//Function for saving an integer value to a name in a file
void saveToFile(const char* file_path, const char* str, int value)
{
	
	FILE* file = NULL;

	char currentLine[256];
	unsigned short count = 0;

	unsigned short lineCount = getLineCount((char*)file_path);

	char** fileLines = SDL_calloc(lineCount, sizeof(char*));
	
	//Read all lines from file and store in fileLines array
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
					fileLines[count] = SDL_calloc(len + 1, sizeof(char));

					//Convert value to string
					char* valueAsString = SDL_calloc(getIntLength(value), sizeof(char));
					SDL_itoa(value, valueAsString, 10);

					//Combine all aspects into a single string
					strcpy(fileLines[count], currName);
					strcat(fileLines[count], "=");
					strcat(fileLines[count], valueAsString);

					SDL_free(valueAsString);

				}
				else	//If its not the name we're looking for, just store it as-is in
				{		//in fileLines

					//Remove newline at end of currentLine
					if (currentLine[SDL_strlen(currentLine)] == '\n')
						currentLine[SDL_strlen(currentLine)] = '\0';
					else if (currentLine[SDL_strlen(currentLine) - 1] == '\n')
						currentLine[SDL_strlen(currentLine) - 1] = '\0';

					//Copy currentLine into fileLines array
					fileLines[count] = SDL_calloc(SDL_strlen(currentLine) + 1, sizeof(char));
					strcpy(fileLines[count], currentLine);

				}

				SDL_free(currName);
				
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
		SDL_free(fileLines[i]);
	SDL_free(fileLines);
	fileLines = NULL;
	
}

//Load the top score of the specified size
	//size = 0 is for Multris mode
unsigned int loadTop(unsigned short size, bool inCustomMode)
{

	// Create the top file if it doesn't exist
	if (inCustomMode == false)
	{

		if (!fileExists("SAVES/top.md"))
			createTopFile();

	}
	else
	{

		if (!fileExists("SAVES/custom_top.md"))
			createCustomTopFile();

	}

	char* sizeAsString = SDL_calloc(getIntLength(size), sizeof(char));
	SDL_itoa(size, sizeAsString, 10);

	// Different save files depending on if playing in CUSTOM_MODE or not
	int top = 0;
	if (inCustomMode == false)
		top = getFileValue("SAVES/top.md", sizeAsString);
	else
		top = getFileValue("SAVES/custom_top.md", sizeAsString);

	SDL_free(sizeAsString);

	if (top == INT_MAX)
		return 0;
	else
		return top;

}

//Load progress
unsigned int loadProgress()
{

	int prog = getFileValue("SAVES/progress.md", "progress");

	if (prog < 1 || prog > MAX_PIECE_SIZE)
		return 1;
	else
		return prog;

}

//Save top score for specified size
void saveTop(unsigned int score, unsigned short size, bool inCustomMode)
{

	// Different save files depending on if playing in CUSTOM_MODE or not
	if (inCustomMode == false)
	{

		if (!fileExists("SAVES/top.md"))
			createTopFile();

	}
	else
	{

		if (!fileExists("SAVES/custom_top.md"))
			createCustomTopFile();

	}

	char* sizeAsString = SDL_calloc(getIntLength(size), sizeof(char));
	SDL_itoa(size, sizeAsString, 10);

	if (inCustomMode == false)
		saveToFile("SAVES/top.md", sizeAsString, score);
	else
		saveToFile("SAVES/custom_top.md", sizeAsString, score);

	SDL_free(sizeAsString);

}

//Save progress
void saveProgress()
{

	if (!fileExists("SAVES/progress.md"))
		createProgressFile();

	saveToFile("SAVES/progress.md", "progress", PROGRESS);

}

//Save window size and position
void saveWindowSettings()
{

	gameInstance* inst = globalInstance;

	SDL_GetWindowSize(inst->window, &inst->minimizedWindow_W, &inst->minimizedWindow_H);
	SDL_GetWindowPosition(inst->window, &inst->minimizedWindow_X, &inst->minimizedWindow_Y);

	saveToFile("SAVES/window.cfg", "WIDTH", globalInstance->minimizedWindow_W);
	saveToFile("SAVES/window.cfg", "HEIGHT", globalInstance->minimizedWindow_H);
	saveToFile("SAVES/window.cfg", "X", globalInstance->minimizedWindow_X);
	saveToFile("SAVES/window.cfg", "Y", globalInstance->minimizedWindow_Y);

}