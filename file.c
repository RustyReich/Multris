#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "technicalDefinitions.h"

bool fileExists(char* fileName)
{

	struct stat buffer;
	return (stat(fileName, &buffer) == 0);

}

void createOptions()
{

	FILE* optionsFile = NULL;
	errno_t err;

	//Create options file and fill it with default data
	err = fopen_s(&optionsFile, "options.cfg", "a");
	if (optionsFile != NULL)
	{

		fprintf(optionsFile, "0");

		fclose(optionsFile);

	}

}

void saveOption(unsigned short line, unsigned short value)
{

	FILE* optionsFile = NULL;
	errno_t err;

	//Open file for writing
	err = fopen_s(&optionsFile, "options.cfg", "w");
	if (optionsFile != NULL)
	{

		fprintf(optionsFile, "%d", value);

		fclose(optionsFile);

	}

}

unsigned short getOption(unsigned short line)
{

	FILE* optionsFile = NULL;
	errno_t err;

	//Open file for reading
	err = fopen_s(&optionsFile, "options.cfg", "r");
	if (optionsFile != NULL)
	{

		//Conver the char into an integer value
		unsigned short returnValue = getc(optionsFile) - '0';

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
	errno_t err;

	if (!fileExists("top.md"))
		return 0;
	else
		err = fopen_s(&topFile, "top.md", "r");

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
	errno_t err;

	//Create file if it does not exist, open it if it does
	if (!fileExists("top.md"))
		err = fopen_s(&topFile, "top.md", "a");
	else
		err = fopen_s(&topFile, "top.md", "w");

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
			fprintf(topFile, string);

		free(string);
		fclose(topFile);

	}

}