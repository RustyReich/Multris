#include "MGF.h"

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

// Function for creating the "name.cfg" file
void createNameFile()
{

	createFile("SAVES/name.cfg");

	// Open nameFile to verify it was created. Then close it.
	FILE* nameFile = fopen("SAVES/name.cfg", "w");
	if (nameFile != NULL)
		fclose(nameFile);

}

// Function for creating the "hostPort.cfg" file
void createHostPortFile()
{

	createFile("SAVES/hostPort.cfg");

	// Open hostPort file to verify it was created. Then close it.
	FILE* hostPortFile = fopen("SAVES/hostPort.cfg", "w");
	if (hostPortFile != NULL)
		fclose(hostPortFile);

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
		fprintf(optionsFile, "MUSIC=35\n");
		fprintf(optionsFile, "LIMIT FPS=1\n");
		fprintf(optionsFile, "SHOW FPS=1\n");
		fprintf(optionsFile, "CENTER DOT=1\n");
		fprintf(optionsFile, "GRAPHICS=1\n");
		fprintf(optionsFile, "DEBUG=0");

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

		// Get display bounds of the primary display
        SDL_Rect mainDisplayBounds;
        SDL_GetDisplayBounds(0, &mainDisplayBounds);

		// Create window dimensions and position in the center of the primary display
		fprintf(windowFile, "WIDTH=%d\n", mainDisplayBounds.w / 2);
		fprintf(windowFile, "HEIGHT=%d\n", mainDisplayBounds.h / 2);
		fprintf(windowFile, "X=%d\n", mainDisplayBounds.x + mainDisplayBounds.w / 4);
		fprintf(windowFile, "Y=%d", mainDisplayBounds.y + mainDisplayBounds.h / 4);

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

	// Return error INT_MAX value if the name cannot be found in the file
	if (returnValue == NULL)
		return INT_MAX;

	// Return error INT_MAX value if the length of the returnValue is zero
	if (SDL_strlen(returnValue) == 0)
		return INT_MAX;

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

// Return true if there are irregularities with the name file
	// This means the name is either too long, or there are non-alphanumeric characters in the name
	// or the file has more than one line
bool brokenNameFile()
{

	if (getLineCount("SAVES/name.cfg") != 1)
		return true;

	char currentLine[256];

	FILE* file = NULL;
	file = fopen("SAVES/name.cfg", "r");
	if (file != NULL)
	{

		if (fgets(currentLine, sizeof(currentLine), file) != NULL)
		{

			if (SDL_strlen(currentLine) > MAX_NAME_LENGTH)
			{

				fclose(file);
				return true;

			}

			int index = 0;
			while(currentLine[index] != '\0' && currentLine[index] != '\n')
			{

				if (!SDL_isdigit(currentLine[index]) && !SDL_isalpha(currentLine[index]))
				{

					fclose(file);
					return true;

				}

				index++;

			}

		}

		fclose(file);
		return false;
	
	}

	return true;

}

// Return true if there are irregularities with the hostPort file
	// This means the hostPort is either too long, or there are non-numeric characters in the port or the file has more than one line
bool brokenHostPortFile()
{

	if (getLineCount("SAVES/hostPort.cfg") != 1)
		return true;

	char currentLine[256];

	FILE* file = NULL;
	file = fopen("SAVES/hostPort.cfg", "r");
	if (file != NULL)
	{

		if (fgets(currentLine, sizeof(currentLine), file) != NULL)
		{

			if (SDL_strlen(currentLine) > 5)
			{

				fclose(file);
				return true;

			}

			int index = 0;
			while(currentLine[index] != '\0' && currentLine[index] != '\n')
			{

				if (!SDL_isdigit(currentLine[index]))
				{

					fclose(file);
					return true;

				}

				index++;

			}

		}

		fclose(file);
		return false;
	
	}

	return true;

}

// Function for saving the name to the name.cfg file
void saveName(char* name)
{

	// Open the file and write the name to it
	FILE* file = NULL;
	file = fopen("SAVES/name.cfg", "w");
	if (file != NULL)
	{

		fprintf(file, "%s", name);
		fclose(file);

	}

}

// Function for saving the hostPort to the hostPort.cfg file
void saveHostPort(char* portString)
{

	// Open the file and write the name to it
	FILE* file = NULL;
	file = fopen("SAVES/hostPort.cfg", "w");
	if (file != NULL)
	{

		fprintf(file, "%s", portString);
		fclose(file);

	}

}

// Function for loading the name from the name.cfg file
	// This function returns a dynamically allocated string which must be freed
char* loadName()
{

	char currentLine[256];
	char* name = NULL;

	FILE* file = NULL;
	file = fopen("SAVES/name.cfg", "r");
	if (file != NULL)
	{

		if (fgets(currentLine, sizeof(currentLine), file) != NULL)
		{

			name = SDL_calloc(SDL_strlen(currentLine) + 1, sizeof(char));
			SDL_strlcpy(name, currentLine, SDL_strlen(currentLine) + 1);

		}

		fclose(file);

	}

	return name;

}

// Function for loading the hostPort from the hostPort.cfg file
	// This function returns a dynamically allocated string which must be freed
char* loadHostPort()
{

	char currentLine[256];
	char* hostPort = NULL;

	FILE* file = NULL;
	file = fopen("SAVES/hostPort.cfg", "r");
	if (file != NULL)
	{

		if (fgets(currentLine, sizeof(currentLine), file) != NULL)
		{

			hostPort = SDL_calloc(SDL_strlen(currentLine) + 1, sizeof(char));
			SDL_strlcpy(hostPort, currentLine, SDL_strlen(currentLine) + 1);

		}

		fclose(file);

	}

	return hostPort;

}

//Return true if there are any irregularities with the options file
	//This means if any options have irregular values
	//It also checks if there are the right amount of lines in the file
bool brokenOptions()
{

	if (getLineCount("SAVES/options.cfg") != NUM_OF_OPTIONS)
		return true;

	int fullscreen_value = getFileValue("SAVES/options.cfg", "FULLSCREEN");
	int volume_value = getFileValue("SAVES/options.cfg", "VOLUME");
	int music_value = getFileValue("SAVES/options.cfg", "MUSIC");
	int fps_value = getFileValue("SAVES/options.cfg", "LIMIT FPS");
	int show_fps_value = getFileValue("SAVES/options.cfg", "SHOW FPS");
	int center_dot_value = getFileValue("SAVES/options.cfg", "CENTER DOT");
	int graphics_value = getFileValue("SAVES/options.cfg", "GRAPHICS");
	int debug_value = getFileValue("SAVES/options.cfg", "DEBUG");

	if (fullscreen_value != 0 && fullscreen_value != 1)
		return true;
	if (volume_value < 0 || volume_value > 100)
		return true;
	if (music_value < 0 || music_value > 100)
		return true;
	if (fps_value != 0 && fps_value != 1)
		return true;
	if (show_fps_value != 0 && show_fps_value != 1)
		return true;
	if (center_dot_value != 0 && center_dot_value != 1)
		return true;
	if (graphics_value != 0 && graphics_value != 1)
		return true;
	if (debug_value != 0 && debug_value != 1)
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

	// Check if window dimensions and position are unreasonably outside of the possible display bounds
	if (getFileValue("SAVES/window.cfg", "WIDTH") <= 0 || 
		getFileValue("SAVES/window.cfg", "WIDTH") > globalInstance->maxDisplayX)
		return true;
	if (getFileValue("SAVES/window.cfg", "X") < -1 * globalInstance->maxDisplayX || 
		getFileValue("SAVES/window.cfg", "X") > globalInstance->maxDisplayX)
		return true;
	if (getFileValue("SAVES/window.cfg", "HEIGHT") <= 0 ||
		getFileValue("SAVES/window.cfg", "HEIGHT") > globalInstance->maxDisplayY)
		return true;
	if (getFileValue("SAVES/window.cfg", "Y") <= -1 * globalInstance->maxDisplayY ||
		getFileValue("SAVES/window.cfg", "Y") > globalInstance->maxDisplayY)
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
	{

		// If value of top is error INT_MAX value, save score as 0 and return 0
		saveTop(0, size, inCustomMode);
		return 0;

	}
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