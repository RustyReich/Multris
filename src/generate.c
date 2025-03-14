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

	//Initialize minX and maX to the X value of our first block
	signed short minX = Piece->blocks[0].X;
	signed short maxX = Piece->blocks[0].X;

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

	//Initialize minY and maxY to the Y value of our first block
	signed short minY = Piece->blocks[0].Y;
	signed short maxY = Piece->blocks[0].Y;

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

//Function for making a piece more random. This should be called on a pseudo-random piece created by generateGamePiece().
piece* makePieceMoreRandom(piece* Piece)
{

	//Only perform on pieces that are more than 2 block because there is only one unique polyomino for pieces of size 1 or 2.
	if (Piece->numOfBlocks > 2)
	{

		//Define mixing time (the number of times we run through the algorithm)
			//We're choosing to do it twice as many times as the number of blocks in our piece
		int mixingTime = 2 * Piece->numOfBlocks;
		int currentIteration = 0;

		//Go through the algorithm a mixingTime number of times
		while (currentIteration < mixingTime)
		{

			//Pick a random block in the piece
			int currentBlockIndex = MGF_rand() % Piece->numOfBlocks;

			//Get all of the blocks that neighbor this block
			int* occupiedNeighborBlocks = NULL;
			int numOccupiedNeighborBlocks = 0;
			for (unsigned short direction = 0; direction < 4; direction++)
			{

				int x = Piece->blocks[currentBlockIndex].X;
				int y = Piece->blocks[currentBlockIndex].Y;

				if (direction == 0) // Above
					y = y - 1;
				else if (direction == 1) // Right
					x = x + 1;
				else if (direction == 2) // Below
					y = y + 1;
				else if (direction == 3) // Left
					x = x - 1; 

				for (unsigned short i = 0; i < Piece->numOfBlocks; i++)
				{

					if (x == Piece->blocks[i].X && y == Piece->blocks[i].Y)
					{

						if (occupiedNeighborBlocks == NULL)
							occupiedNeighborBlocks = SDL_calloc(1, sizeof(int));
						else
							occupiedNeighborBlocks = SDL_realloc(occupiedNeighborBlocks, (numOccupiedNeighborBlocks + 1) * sizeof(int));
						occupiedNeighborBlocks[numOccupiedNeighborBlocks] = i;
						numOccupiedNeighborBlocks = numOccupiedNeighborBlocks + 1;

					}

				}

			}

			//Get coords of unoocupied neighbor cells of all blocks other than our currentBlock
			int* unoccupiedX = SDL_calloc(1, sizeof(int));
			int* unoccupiedY = SDL_calloc(1, sizeof(int));
			int numUnoccupiedNeighbors = 0;
			
			//Go through every block
			for (unsigned short otherBlockIndex = 0; otherBlockIndex < Piece->numOfBlocks; otherBlockIndex++)
			{

				// For every block other than our current block
				if (otherBlockIndex != currentBlockIndex)
				{

					// Check all directions to see if it's neighboring cell is occupied
					for (unsigned short direction = 0; direction < 4; direction++)
					{

						int x = Piece->blocks[otherBlockIndex].X;
						int y = Piece->blocks[otherBlockIndex].Y;
						bool occupied = false;
						bool alreadyInList = false;

						if (direction == 0) // Above
							y = y - 1;
						else if (direction == 1) // Right
							x = x + 1;
						else if (direction == 2) // Below
							y = y + 1;
						else if (direction == 3) // Left
							x = x - 1; 


						//Check if this coord is already in our list so that we don't have duplicates in our list
						if (numUnoccupiedNeighbors > 0)
						{

							for (unsigned short indexInList = 0; indexInList < numUnoccupiedNeighbors; indexInList++)
							{

								if (x == unoccupiedX[indexInList] && y == unoccupiedY[indexInList])
								{

									alreadyInList = true;
									break;

								}

							}

						}

						//If this coord is not already in the list
						if (alreadyInList == false)
						{

							//Check all other blocks to see if this spot is occupied by any of them
							for (unsigned short testBlockIndex = 0; testBlockIndex < Piece->numOfBlocks; testBlockIndex++)
							{

								if (testBlockIndex != otherBlockIndex)
								{
									
									if (x == Piece->blocks[testBlockIndex].X && y == Piece->blocks[testBlockIndex].Y)
									{

										occupied = true;
										break;

									}

								}

							}

						}

						//If the coord is not already in our list and it is not occupied by a block, that means it is an unoccupied neighbor
						//cell. So, store it in our list.
						if (occupied == false && alreadyInList == false)
						{

							if (numUnoccupiedNeighbors > 0)
							{

								unoccupiedX = SDL_realloc(unoccupiedX, (numUnoccupiedNeighbors + 1) * sizeof(int));
								unoccupiedY = SDL_realloc(unoccupiedY, (numUnoccupiedNeighbors + 1) * sizeof(int));

							}

							unoccupiedX[numUnoccupiedNeighbors] = x;
							unoccupiedY[numUnoccupiedNeighbors] = y;

							numUnoccupiedNeighbors = numUnoccupiedNeighbors + 1;

						}

					}

				}

			}

			//Save original coords of currentBlock
			int originalX = Piece->blocks[currentBlockIndex].X;
			int originalY = Piece->blocks[currentBlockIndex].Y;

			//Move currentBlock to a random unoccupied neighbor cell
			int randomUnoccupiedNeighborCell = MGF_rand() % numUnoccupiedNeighbors;
			Piece->blocks[currentBlockIndex].X = unoccupiedX[randomUnoccupiedNeighborCell];
			Piece->blocks[currentBlockIndex].Y = unoccupiedY[randomUnoccupiedNeighborCell];

			//Check if the move is valid
			bool moveIsValid = true;
			bool breakOut = false;

			//To check if the move is valid, we check if all neighboring blocks of the original location of currentBlock are still
			//connected to each other now that currentBlock has been moved
			if (numOccupiedNeighborBlocks > 1)
			{

				for (unsigned short i = 0; i < SDL_ceil((double)numOccupiedNeighborBlocks / (double)2); i++)
				{

					for (unsigned short j = i + 1; j < numOccupiedNeighborBlocks; j++)
					{

						if (areBlocksConnected(Piece, occupiedNeighborBlocks[i], occupiedNeighborBlocks[j], NULL, NULL) == false)
						{

							moveIsValid = false;
							breakOut = true;

						}

						if (breakOut)
							break;

					}

					if (breakOut)
						break;

				}

			}

			//If move is not valid, move it back to it's original spot
			if (moveIsValid == false)
			{

				Piece->blocks[currentBlockIndex].X = originalX;
				Piece->blocks[currentBlockIndex].Y = originalY;

			}

			//Free memory used by our lists
			SDL_free(unoccupiedX);
			SDL_free(unoccupiedY);
			SDL_free(occupiedNeighborBlocks);

			//Iterate our counter
			currentIteration = currentIteration + 1;

		}

	}

	return Piece;

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
		Piece->color = (MGF_rand() % (RED - YELLOW + 1)) + YELLOW;

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
			if (MGF_rand() % 7 == 3)
			{

				//When the game piece does spawn as a straight line, there is a 50/50 shot of whether it will
				//spawn horizontally or vertically
				if (MGF_rand() % 2 == 1)
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
						randID = MGF_rand() % i;			//Randomly choose a block to build off of
					while (isSurrounded(Piece, randID));
					
					//Here, we randomly choose a direction from our chosen block to build off from, making sure to
					//choose a direction that isnt already currently occupied
					do
						randDirection = MGF_rand() % 4;
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

				//If we are not making a straight-line piece, make this piece more random
				Piece = makePieceMoreRandom(Piece);

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

	int numValues = 0;
	char** values = extractStringsFromDelimitedBytes(string, SDL_strlen(string) + 1, &numValues, '|');

	int blockIndex = 0;
	for (unsigned short i = 0; i < numValues; i++)
	{

		if (i == 0)
			Piece->color = SDL_atoi(values[i]);
		else if (i == 1)
		{

			Piece->numOfBlocks = SDL_atoi(values[i]);
			Piece->blocks = SDL_calloc(Piece->numOfBlocks, sizeof(block));

		}
		else if (i % 2 == 0)
			Piece->blocks[blockIndex].X = SDL_atoi(values[i]);
		else if (i % 2 == 1)
		{

			Piece->blocks[blockIndex].Y = SDL_atoi(values[i]);
			blockIndex++;

		}

	}

	for (int i = 0; i < numValues; i++)
		SDL_free(values[i]);
	SDL_free(values);

	Piece->width = calcWidth(Piece);
	Piece->height = calcHeight(Piece);

	createCenterBlock(Piece);

	return Piece;	

}

//Function for checking whether two blocks in a piece are connected by a list of blocks
	//For a piece to be valid, all blocks must be connected to all other blocks
bool areBlocksConnected(piece* Piece, int blockIndex1, int blockIndex2, int* traversedBlockIndices, int* numOfTraversedBlockIndices)
{

	//Allocate space to keep track of the number of blocks that are currently in our list of traversed block on our current path
	if (numOfTraversedBlockIndices == NULL)
	{

		numOfTraversedBlockIndices = SDL_calloc(1, sizeof(int));
		*numOfTraversedBlockIndices = 0;

	}

	//Allocate space to keep track of the indices of all blocks that neighbor the block we are currently looking at
	int* neighboringBlockIndices = SDL_calloc(4, sizeof(int));
	int numOfNeighboringBlocks = 0;

	//Allocate space to store our list of blocks that have been traversed on our current path
		//Since we never repeat blocks in a given path, we only need to allocate space to store as many block as are in our piece, because
		//we know our path will never be longer than that
	if (traversedBlockIndices == NULL)
		traversedBlockIndices = SDL_calloc(Piece->numOfBlocks, sizeof(int));
	traversedBlockIndices[*numOfTraversedBlockIndices] = blockIndex1;
	*numOfTraversedBlockIndices = *numOfTraversedBlockIndices + 1;

	//Check all neighboring cells of block1
	for (unsigned short direction = 0; direction < 4; direction++)
	{

		int x = Piece->blocks[blockIndex1].X;
		int y = Piece->blocks[blockIndex1].Y;

		if (direction == 0) // Above
			y = y - 1;
		else if (direction == 1) // Right
			x = x + 1;
		else if (direction == 2) // Below
			y = y + 1;
		else if (direction == 3) // Left
			x = x - 1; 

		//Check all blocks to see if a block exists in the neighboring cell in the current direction
		for (unsigned short otherBlockIndex = 0; otherBlockIndex < Piece->numOfBlocks; otherBlockIndex++)
		{

			//Skip the block index of block1
			if (otherBlockIndex != blockIndex1)
			{

				//If the coords for otherBlock are equal to the coords of the neighboring cell of block1, that otherBlock neighbors block1
				if (x == Piece->blocks[otherBlockIndex].X && y == Piece->blocks[otherBlockIndex].Y)
				{

					//If a neighboring cell of block1 is occupied by block2, they are direct neighbors so we can return true
					if (otherBlockIndex == blockIndex2)
					{

						//numOfTraversedBlockIndices and traversedBlockIndices are shared between levels of recurssion, so we only want to
						//free them once at the very end: either when we know for sure that our initial block1 and block2 are connected or
						//we know for sure that they are not.
						SDL_free(numOfTraversedBlockIndices);
						SDL_free(traversedBlockIndices);

						//neighboringBlockIndices is unique for each level of recurssion so we need to free it every time we return from this
						//function.
						SDL_free(neighboringBlockIndices);

						return true;

					}
					else
					{

						//Otherwise, keep track of the block indices of all blocks that neighbor block1
						neighboringBlockIndices[numOfNeighboringBlocks] = otherBlockIndex;
						numOfNeighboringBlocks = numOfNeighboringBlocks + 1;

					}

				}

			}

		}

	}

	//If block2 is not a direct neighbor of block1:
	//For all neighboring blocks of block1, check if they are connected to block2
	for (unsigned short i = 0; i < numOfNeighboringBlocks; i++)
	{

		//Check if we have already traversed the neighboring block in our current path
			//If we have, we don't want to call areBlocksConnected on it again because we will get stuck in a loop
		int alreadyTraversed = false;
		for (unsigned short j = 0; j < *numOfTraversedBlockIndices; j++)
			if (neighboringBlockIndices[i] == traversedBlockIndices[j])
				alreadyTraversed = true;
		if (alreadyTraversed)
			continue;

		//If we have not traversed the neighboring block before, check if it is connected to block2.
		//If one neighboring block of block1 is connected to block2, that means that block1 is connected to block2 so we can return true.
		if (areBlocksConnected(Piece, neighboringBlockIndices[i], blockIndex2, traversedBlockIndices, numOfTraversedBlockIndices))
		{

			//neighboringBlockIndices is unique for each level of recurssion so we need to free it every time we return from this
			//function.
			SDL_free(neighboringBlockIndices);

			return true;

		}

		//If the current neighbor is not connected to block2, then "pop" it off our stack of traversedBlockIndices by reducing
		//numOfTraversedBlockIndices by 1. This is basically removing it from our path because we know it's not connected to block2.
		//We only need to reduce numOfTraversedBlockIndices by 1 to achieve popping the neighbor off, because we know that the neighbor
		//will always be at the top of our stack.
		*numOfTraversedBlockIndices = *numOfTraversedBlockIndices - 1;

	}

	//If we have gone through all neighbors of block1 and our numOfTraversedBlockIndices == 1, then that means we are currently looking at
	//our initial block1 and none of it's neighbors are connected to block2. This means that block1 is not connected to block2. So, we have
	//reached our end state of recurssion and thus need to free numOfTraversedBlockIndices and traversedBlockIndices.
	if (*numOfTraversedBlockIndices == 1)
	{

		SDL_free(numOfTraversedBlockIndices);
		SDL_free(traversedBlockIndices);

	}

	//neighboringBlockIndices is unique for each level of recurssion so we need to free it every time we return from this
	//function.
	SDL_free(neighboringBlockIndices);

	//Block1 is not a direct neighbor of block2, and none of the neighbors of block1 are connected to block2. Therefore, block1 is not
	//connected to block2.
	return false;

}