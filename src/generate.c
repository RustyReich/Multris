#include "MGF.h"

// Function for finding and creating the centerBlock for a piece
void createCenterBlock(piece* piece)
{

	piece->centerBlock = SDL_calloc(1, sizeof(block));
	piece->centerBlock->X = piece->minX + SDL_floor((float)piece->width / (float)2);
	piece->centerBlock->Y = piece->minY + SDL_floor((float)piece->height / (float)2);

}

//Function for copying all blocks from one piece to another
	//Make sure piece2 has allocated enough space to store all blocks
void copyBlocks(piece* piece1, piece* piece2)
{
	
	//Copy over all blocks
	for (unsigned short i = 0; i < piece1->numOfBlocks; i++)
	{

		piece2->blocks[i].X = piece1->blocks[i].X;
		piece2->blocks[i].Y = piece1->blocks[i].Y;

	}

	// Copy over the coordinates for the centerBlock
	piece2->centerBlock->X = piece1->centerBlock->X;
	piece2->centerBlock->Y = piece1->centerBlock->Y;
	
}

//Copy a piece
void copyPiece(piece* piece1, piece* piece2)
{

	//Copy all piece attributes
	piece2->color = piece1->color;
	piece2->height = piece1->height;
	piece2->width = piece1->width;
	piece2->minX = piece1->minX;
	piece2->minY = piece1->minY;
	piece2->numOfBlocks = piece1->numOfBlocks;

	//Copy the blocks from the piece
	copyBlocks(piece1, piece2);

}

//Clear memory allocated by a Piece
void delPiece(piece** Piece)
{

	// Delete the centerBlock
	SDL_free((*Piece)->centerBlock);
	(*Piece)->centerBlock = NULL;

	//Delete blocks
	SDL_free((*Piece)->blocks);
	(*Piece)->blocks = NULL;

	//Delete Piece itself
	SDL_free(*Piece);
	*Piece = NULL;

}

//Check if a block exists in a specified direction from an existing block in a piece
bool isOccupied(piece* Piece, unsigned short blockID, unsigned short direction)
{

	if (direction == UP)
	{

		for (unsigned short i = 0; i < Piece->numOfBlocks; i++)
		{

			if (Piece->blocks[i].X == Piece->blocks[blockID].X &&
				Piece->blocks[i].Y == Piece->blocks[blockID].Y - 1)
				return true;

		}

	}
	else if (direction == RIGHT)
	{

		for (unsigned short i = 0; i < Piece->numOfBlocks; i++)
		{

			if (Piece->blocks[i].X == Piece->blocks[blockID].X + 1 &&
				Piece->blocks[i].Y == Piece->blocks[blockID].Y)
				return true;

		}

	}
	else if (direction == DOWN)
	{

		for (unsigned short i = 0; i < Piece->numOfBlocks; i++)
		{

			if (Piece->blocks[i].X == Piece->blocks[blockID].X &&
				Piece->blocks[i].Y == Piece->blocks[blockID].Y + 1)
				return true;

		}

	}
	else if (direction == LEFT)
	{

		for (unsigned short i = 0; i < Piece->numOfBlocks; i++)
		{

			if (Piece->blocks[i].X == Piece->blocks[blockID].X - 1 &&
				Piece->blocks[i].Y == Piece->blocks[blockID].Y)
				return true;

		}

	}

	return false;

}

//Check if a specified block in a piece is surrounded by other blocks in the piece
bool isSurrounded(piece* Piece, unsigned short blockID)
{

	bool occupied[4];

	//We can basically just check if each direction from the block is occupied
	occupied[UP] = isOccupied(Piece, blockID, UP);
	occupied[RIGHT] = isOccupied(Piece, blockID, RIGHT);
	occupied[DOWN] = isOccupied(Piece, blockID, DOWN);
	occupied[LEFT] = isOccupied(Piece, blockID, LEFT);

	if (occupied[UP] == true && occupied[RIGHT] == true && occupied[DOWN] == true && occupied[LEFT] == true)
		return true;
	else 
		return false;

}

//Get the width, in blocks, of a piece
unsigned short calcWidth(piece* Piece)
{

	signed short minX = 0;
	signed short maxX = 0;

	//Go through all blocks to find the min and max X values
	for (unsigned short i = 0; i < Piece->numOfBlocks; i++)
	{

		if (Piece->blocks[i].X < minX)
			minX = Piece->blocks[i].X;
		else if (Piece->blocks[i].X > maxX)
			maxX = Piece->blocks[i].X;

	}

	Piece->minX = minX;

	return ((maxX - minX) + 1);

}

//Get the height, in blocks, of a piece
unsigned short calcHeight(piece* Piece)
{

	signed short minY = 0;
	signed short maxY = 0;

	//Go through all blocks to find the min and max Y values
	for (unsigned short i = 0; i < Piece->numOfBlocks; i++)
	{

		if (Piece->blocks[i].Y < minY)
			minY = Piece->blocks[i].Y;
		else if (Piece->blocks[i].Y > maxY)
			maxY = Piece->blocks[i].Y;

	}

	Piece->minY = minY;

	return ((maxY - minY) + 1);

}

//Generate a random piece of the given size
piece* generateGamePiece(unsigned short size)
{

	piece* Piece = NULL;

	Piece = SDL_malloc(sizeof(*Piece));
	
	if (Piece != NULL)
	{

		Piece->numOfBlocks = 0;
		
		//Pick random color for piece
		Piece->color = (rand() % (RED - YELLOW + 1)) + YELLOW;

		//Allocate memory for all blocks
		if (size > 0)
			Piece->blocks = SDL_malloc(size * sizeof(*Piece->blocks));
		
		if (Piece->blocks != NULL)
		{

			//Firstly, start off by placing a single block
			Piece->blocks[0].X = 0;
			Piece->blocks[0].Y = 0;
			Piece->numOfBlocks = Piece->numOfBlocks + 1;
			
			//There is a 1 in 7 chance of the game piece spawning as just a straight line
				//It is this way for game balancing reasons
			if (rand() % 7 == 3)
			{

				//When the game piece does spawn as a straight line, there is a 50/50 shot of whether it will
				//spawn horizontally or vertically
				if (rand() % 2 == 1)
				{

					//Go through each block that needs to be generated, simply creating each one directly to the
					//left of the last one
						//This is for the horizontal staight-line piece
					for (unsigned short i = 1; i < size; i++)
					{

						Piece->blocks[i].X = Piece->blocks[i - 1].X - 1;
						Piece->blocks[i].Y = Piece->blocks[i - 1].Y;
						Piece->numOfBlocks = Piece->numOfBlocks + 1;

					}

				}
				else
				{

					//Now go through each block that needs to be generated, and create each one immediately below
					//the previous one
						//This is for the vertical straight-line piece
					for (unsigned short i = 1; i < size; i++)
					{

						Piece->blocks[i].X = Piece->blocks[i - 1].X;
						Piece->blocks[i].Y = Piece->blocks[i - 1].Y + 1;
						Piece->numOfBlocks = Piece->numOfBlocks + 1;

					}

				}

			}	
			else	//This is the section that actually procedurally builds a random game piece 6 out of 7 times
			{

				//Go through each block that needs to be generated
				for (unsigned short i = 1; i < size; i++)
				{

					unsigned short randID;
					unsigned short randDirection;

					//Here, we randomly chose a block that we want our next block to build off of, making sure to
					//only pick a block that isnt already surrounded by blocks
					do
						randID = rand() % i;			//Randomly choose a block to build off of
					while (isSurrounded(Piece, randID));
					
					//Here, we randomly choose a direction from our chosen block to build off from, making sure to
					//choose a direction that isnt already currently occupied
					do
						randDirection = rand() % 4;
					while (isOccupied(Piece, randID, randDirection));

					//Set the coordinates of our next block
					if (randDirection == UP)
					{

						Piece->blocks[i].X = Piece->blocks[randID].X;
						Piece->blocks[i].Y = Piece->blocks[randID].Y - 1;
						Piece->numOfBlocks = Piece->numOfBlocks + 1;

					}
					else if (randDirection == RIGHT)
					{

						Piece->blocks[i].X = Piece->blocks[randID].X + 1;
						Piece->blocks[i].Y = Piece->blocks[randID].Y;
						Piece->numOfBlocks = Piece->numOfBlocks + 1;

					}
					else if (randDirection == DOWN)
					{

						Piece->blocks[i].X = Piece->blocks[randID].X;
						Piece->blocks[i].Y = Piece->blocks[randID].Y + 1;
						Piece->numOfBlocks = Piece->numOfBlocks + 1;

					}
					else if (randDirection == LEFT)
					{

						Piece->blocks[i].X = Piece->blocks[randID].X - 1;
						Piece->blocks[i].Y = Piece->blocks[randID].Y;
						Piece->numOfBlocks = Piece->numOfBlocks + 1;

					}

				}

			}

			//Calculate the width and height of the piece
			Piece->width = calcWidth(Piece);
			Piece->height = calcHeight(Piece);

			// Create the centerBlock
			createCenterBlock(Piece);

		}
		
	}

	return Piece;

}

// Function for converting a piece into a string of data to be sent to a server.
	// This function dynamically allocates memory for the return value. Ensure to free it with SDL_free().
	// Information is encoded in the format "COLOR|NUM_OF_BLOCKS|X|Y|X|Y|...|X|Y|"
char* convertPieceToString(piece* Piece)
{

	char* string = SDL_calloc(2, sizeof(char));
	string[0] = '0' + (char)Piece->color;
	string[1] = '\0';

	char* numOfBlocksString = SDL_calloc(getIntLength(Piece->numOfBlocks) + 1, sizeof(char));
	SDL_itoa(Piece->numOfBlocks, numOfBlocksString, 10);
	numOfBlocksString[getIntLength(Piece->numOfBlocks)] = '\0';

	int len = SDL_strlen(string) + SDL_strlen("|") + SDL_strlen(numOfBlocksString) + 1;
	string = SDL_realloc(string, len * sizeof(char));
	SDL_strlcat(string, "|", len);
	SDL_strlcat(string, numOfBlocksString, len);

	SDL_free(numOfBlocksString);

	for (unsigned short i = 0; i < Piece->numOfBlocks; i++)
	{

		int x = Piece->blocks[i].X;
		int y = Piece->blocks[i].Y;

		char* xString = SDL_calloc(getIntLength(x) + 1, sizeof(char));
		SDL_itoa(x, xString, 10);
		xString[getIntLength(x)] = '\0';

		char* yString = SDL_calloc(getIntLength(y) + 1, sizeof(char));
		SDL_itoa(y, yString, 10);
		yString[getIntLength(y)] = '\0';

		len = SDL_strlen(string) + SDL_strlen("|") + SDL_strlen(xString) + 1;
		string = SDL_realloc(string, len * sizeof(char));
		SDL_strlcat(string, "|", len);
		SDL_strlcat(string, xString, len);

		len = SDL_strlen(string) + SDL_strlen("|") + SDL_strlen(yString) + 1;
		string = SDL_realloc(string, len * sizeof(char));
		SDL_strlcat(string, "|", len);
		SDL_strlcat(string, yString, len);

		SDL_free(xString);
		SDL_free(yString);

	}

	len = SDL_strlen(string) + SDL_strlen("|") + 1;
	string = SDL_realloc(string, len * sizeof(char));
	SDL_strlcat(string, "|", len);

	return string;

}

// Function for creating a piece structure from a string of data received from the server
	// Information is encoded in the format "COLOR|NUM_OF_BLOCKS|X|Y|X|Y|...|X|Y|"
piece* createPieceFromString(char* string)
{

	// Allocate memory for the piece
	piece* Piece = SDL_calloc(1, sizeof(piece));

	char* currentStringValue = NULL;

	int valueIndex = 0;
	int stringIndex = 0;
	int blockIndex = 0;

	// Go through each character in the string.
	while (string[stringIndex] !='\0')
	{

		// If the character is not a delimiter, then we are currently reading a value
		if (string[stringIndex] != '|')
		{

			// Build a string for the current value we are reading
			if (currentStringValue == NULL)
			{

				currentStringValue = SDL_calloc(2, sizeof(char));
				currentStringValue[0] = string[stringIndex];
				currentStringValue[1] = '\0';

			}
			else
			{

				int newLen = SDL_strlen(currentStringValue) + 1 + 1;
				currentStringValue = SDL_realloc(currentStringValue, newLen * sizeof(char));
				SDL_strlcat(currentStringValue, &(string[stringIndex]), newLen);

			}

		}
		else	// Once we hit a delimiter, we know we are done reading the current value
		{

			// So apply it to the currect attribute for the piece depending on the current valueIndex
			if (valueIndex == 0)
				Piece->color = SDL_atoi(currentStringValue);
			else if (valueIndex == 1)
			{

				Piece->numOfBlocks = SDL_atoi(currentStringValue);
				Piece->blocks = SDL_calloc(Piece->numOfBlocks, sizeof(block));

			}
			else if (valueIndex % 2 == 0)
				Piece->blocks[blockIndex].X = SDL_atoi(currentStringValue);
			else if (valueIndex % 2 == 1)
			{

				Piece->blocks[blockIndex].Y = SDL_atoi(currentStringValue);
				blockIndex++;

			}

			valueIndex++;

			// Free the currentStringValue so that we can start reading the next value
			SDL_free(currentStringValue);
			currentStringValue = NULL;

		}

		stringIndex++;

	}

	// Ensure we free the memory for the currentStringValue
	if (currentStringValue != NULL)
	{

		SDL_free(currentStringValue);
		currentStringValue = NULL;

	}

	Piece->width = calcWidth(Piece);
	Piece->height = calcHeight(Piece);

	createCenterBlock(Piece);

	return Piece;	

}