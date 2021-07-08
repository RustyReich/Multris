#include <stdlib.h>
#include <stdio.h>

#include "Structures.h"
#include "technicalDefinitions.h"

//Decompile a sprite string into an array of rectangles
rectangle* decompSprite(char* spriteData, size_t* numOfRectangles)
{

	//Each rectangle stores four bytes of data. So by dividing the length of the sprite by 4, we get the number
	//of rectangles in the sprite
	rectangle* rectangles = malloc((_mbstrlen(spriteData) / 4) * sizeof(*rectangles));
	
	//To decompile the data, all we need to do is take the integer-equivalent of each character in the string,
	//subtract the COMP_START constant from it, and store in the corresponding dataentry in the current rectangle
	if (rectangles != NULL)
	{
		
		for (unsigned short i = 0; i < _mbstrlen(spriteData) - 2; i++)
		{
			
			if (i % 4 == 0 || i == 0)
				rectangles[i / 4].X = (int)(spriteData[i]) - COMP_START;
			else if (i % 4 == 1 || i == 1)
				rectangles[i / 4].Y = (int)(spriteData[i]) - COMP_START;
			else if (i % 4 == 2 || i == 2)
				rectangles[i / 4].Width = (int)(spriteData[i]) - COMP_START;
			else if (i % 4 == 3 || i == 3)
				rectangles[i / 4].Height = (int)(spriteData[i]) - COMP_START;
			
		}
		
	}
	
	*numOfRectangles = _mbstrlen(spriteData) / 4;
	return rectangles;

}

sprite* loadSprites(size_t* numOfSprites)
{

	sprite* Sprites = NULL;
	sprite* temp = NULL;

	char line[255];
	*numOfSprites = 0;

	FILE* file = NULL;
	errno_t err = fopen_s(&file, "12.mse1", "r");
	if (!err && file != NULL)
	{
		
		//Count the number of lines in the sprite file to get the number of sprites being loaded
		while (fgets(line, sizeof(line), file))
			*numOfSprites = *numOfSprites + 1;
		
		//Allocate space for all the sprites
		Sprites = malloc(*numOfSprites * sizeof(*Sprites));
		
		size_t numOfRect;
		rectangle* rectangles = NULL;
		
		rewind(file);
		
		//Read file line by line, decompressing each line
		while (fgets(line, sizeof(line), file))
		{
			
			rectangles = decompSprite(line, &numOfRect);
			
			//Store the sprite into the SpritesData array
			Sprites[(int)(line[_mbstrlen(line) - 2]) - COMP_START].rectangles = rectangles;
			Sprites[(int)(line[_mbstrlen(line) - 2]) - COMP_START].numOfRect = (unsigned short)numOfRect;
			
		}
		
		fclose(file);
		
	}

	return Sprites;

}