#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <math.h>

#include "HEADERS/technicalDefinitions.h"

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

		fprintf(optionsFile, "1\n1\n0\n9\n");

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

unsigned short getOption(unsigned short line)
{

	FILE* optionsFile = NULL;

	char currentLine[2];
	unsigned short count = 0;

	unsigned short returnValue = 0;

	//Open file for reading
	optionsFile = fopen("SAVES/options.cfg", "r");
	if (optionsFile != NULL)
	{

		while (fgets(currentLine, sizeof(currentLine), optionsFile) != NULL)
		{

			//Dont count any newlines
			if (currentLine[0] != 10)
			{

				//Return the value at the requested line
				if (count == line)
					returnValue = currentLine[0] - '0';

				count++;
				
			}

		}

		fclose(optionsFile);

		return returnValue;

	}
	else
		return 0;

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