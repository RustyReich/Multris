#include <stdlib.h>
#include <stdbool.h>

#include "HEADERS/Structures.h"
#include "HEADERS/technicalDefinitions.h"

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
	
}

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

	free((*Piece)->blocks);
	(*Piece)->blocks = NULL;

	free(*Piece);
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

unsigned short calcWidth(piece* Piece)
{

	signed short minX = 0;
	signed short maxX = 0;

	for (unsigned short i = 1; i < Piece->numOfBlocks; i++)
	{

		if (Piece->blocks[i].X < minX)
			minX = Piece->blocks[i].X;
		else if (Piece->blocks[i].X > maxX)
			maxX = Piece->blocks[i].X;

	}

	Piece->minX = minX;

	return ((maxX - minX) + 1);

}

unsigned short calcHeight(piece* Piece)
{

	signed short minY = 0;
	signed short maxY = 0;

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

piece* generateGamePiece(unsigned short size)
{

	piece* Piece = NULL;

	Piece = malloc(sizeof(*Piece));
	
	if (Piece != NULL)
	{

		Piece->numOfBlocks = 0;
		
		Piece->color = (rand() % (RED - YELLOW + 1)) + YELLOW;

		if (size > 0)
			Piece->blocks = malloc(size * sizeof(*Piece->blocks));
		
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

				//When the game piece does spawn as a straight line, there is a 50/50 shot of whether it will spawn
				//horizontally or vertically
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
						randDirection = rand() % 4;					//Randomly choose a direction to build off of
					while (isOccupied(Piece, randID, randDirection));	//from our block

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

		}
		
	}

	return Piece;

}

//All the title pieces are hard-coded
piece** makeTitlePieces()
{

	piece** titlePieces = malloc(NUM_OF_TITLE_PIECES * sizeof(**titlePieces));

	piece* currentPiece = NULL;

	unsigned short sizes[] = { 6, 1, 3, 3, 9, 3, 7, 5, 2, 2, 4, 2, 5, 1, 1, 8, 1, 7, 4 };

	for (unsigned short i = 0; i < NUM_OF_TITLE_PIECES; i++)
	{

		currentPiece = malloc(sizeof(*currentPiece));
		if (currentPiece != NULL)
		{

			currentPiece->numOfBlocks = sizes[i];
			if (currentPiece->numOfBlocks > 0)
				currentPiece->blocks = malloc(currentPiece->numOfBlocks * sizeof(*currentPiece->blocks));

		}

		if (currentPiece != NULL && currentPiece->blocks != NULL)
		{

			currentPiece->color = (rand() % (RED - YELLOW + 1) + YELLOW);

			if (i == 0)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;

				for (unsigned short j = 1; j < currentPiece->numOfBlocks; j++)
				{

					if (j < currentPiece->numOfBlocks - 1)
					{

						currentPiece->blocks[j].X = currentPiece->blocks[j - 1].X;
						currentPiece->blocks[j].Y = currentPiece->blocks[j - 1].Y + 1;

					}
					else
					{

						currentPiece->blocks[j].X = currentPiece->blocks[0].X + 1;
						currentPiece->blocks[j].Y = currentPiece->blocks[0].Y + 1;

					}

				}

			}
			else if (i == 1)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;

			}
			else if (i == 2)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;
				currentPiece->blocks[1].X = 1;
				currentPiece->blocks[1].Y = 0;
				currentPiece->blocks[2].X = 1;
				currentPiece->blocks[2].Y = -1;

			}
			else if (i == 3)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;
				currentPiece->blocks[1].X = 0;
				currentPiece->blocks[1].Y = 1;
				currentPiece->blocks[2].X = 0;
				currentPiece->blocks[2].Y = 2;

			}
			else if (i == 4)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;

				for (unsigned short j = 1; j < currentPiece->numOfBlocks; j++)
				{

					if (j < 5)
					{

						currentPiece->blocks[j].X = currentPiece->blocks[j - 1].X;
						currentPiece->blocks[j].Y = currentPiece->blocks[j - 1].Y + 1;

					}
					else if (j < 8)
					{

						currentPiece->blocks[j].X = currentPiece->blocks[j - 1].X + 1;
						currentPiece->blocks[j].Y = currentPiece->blocks[j - 1].Y;

					}
					else
					{

						currentPiece->blocks[j].X = currentPiece->blocks[j - 1].X;
						currentPiece->blocks[j].Y = currentPiece->blocks[j - 1].Y - 1;

					}

				}

			}
			else if (i == 5)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;

				for (unsigned short j = 1; j < currentPiece->numOfBlocks; j++)
				{

					currentPiece->blocks[j].X = currentPiece->blocks[j - 1].X;
					currentPiece->blocks[j].Y = currentPiece->blocks[j - 1].Y - 1;

				}
			
			}
			else if (i == 6)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;

				for (unsigned short j = 1; j < currentPiece->numOfBlocks; j++)
				{

					if (j < 5)
					{

						currentPiece->blocks[j].X = currentPiece->blocks[j - 1].X;
						currentPiece->blocks[j].Y = currentPiece->blocks[j - 1].Y + 1;

					}
					else
					{

						currentPiece->blocks[j].X = currentPiece->blocks[j - 1].X + 1;
						currentPiece->blocks[j].Y = currentPiece->blocks[j - 1].Y;

					}

				}

			}
			else if (i == 7)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;

				for (unsigned short j = 1; j < currentPiece->numOfBlocks; j++)
				{

					if (j < 3)
					{

						currentPiece->blocks[j].X = currentPiece->blocks[j - 1].X + 1;
						currentPiece->blocks[j].Y = currentPiece->blocks[j - 1].Y;

					}
					else
					{

						currentPiece->blocks[j].X = currentPiece->blocks[j - 1].X;
						currentPiece->blocks[j].Y = currentPiece->blocks[j - 1].Y + 1;

					}

				}

			}
			else if (i == 8)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;
				currentPiece->blocks[1].X = 0;
				currentPiece->blocks[1].Y = 1;

			}
			else if (i == 9)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;
				currentPiece->blocks[1].X = 1;
				currentPiece->blocks[1].Y = 0;

			}
			else if (i == 10)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;
				currentPiece->blocks[1].X = 1;
				currentPiece->blocks[1].Y = 0;
				currentPiece->blocks[2].X = 0;
				currentPiece->blocks[2].Y = 1;
				currentPiece->blocks[3].X = 0;
				currentPiece->blocks[3].Y = 2;

			}
			else if (i == 11)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;
				currentPiece->blocks[1].X = 0;
				currentPiece->blocks[1].Y = 1;

			}
			else if (i == 12)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;
				currentPiece->blocks[1].X = 1;
				currentPiece->blocks[1].Y = 0;
				currentPiece->blocks[2].X = 1;
				currentPiece->blocks[2].Y = 1;
				currentPiece->blocks[3].X = 2;
				currentPiece->blocks[3].Y = 1;
				currentPiece->blocks[4].X = 2;
				currentPiece->blocks[4].Y = 2;

			}
			else if (i == 13)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;

			}
			else if (i == 14)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;

			}
			else if (i == 15)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;
				currentPiece->blocks[1].X = 1;
				currentPiece->blocks[1].Y = 0;

				for (unsigned short j = 2; j < 6; j++)
				{

					currentPiece->blocks[j].X = currentPiece->blocks[j - 1].X;
					currentPiece->blocks[j].Y = currentPiece->blocks[j - 1].Y + 1;

				}

				currentPiece->blocks[6].X = 0;
				currentPiece->blocks[6].Y = 4;
				currentPiece->blocks[7].X = 2;
				currentPiece->blocks[7].Y = 4;

			}
			else if (i == 16)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;

			}
			else if (i == 17)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;
				currentPiece->blocks[1].X = 1;
				currentPiece->blocks[1].Y = 0;
				currentPiece->blocks[2].X = 2;
				currentPiece->blocks[2].Y = 0;
				currentPiece->blocks[3].X = 0;
				currentPiece->blocks[3].Y = 1;
				currentPiece->blocks[4].X = 0;
				currentPiece->blocks[4].Y = 2;
				currentPiece->blocks[5].X = 1;
				currentPiece->blocks[5].Y = 2;
				currentPiece->blocks[6].X = 2;
				currentPiece->blocks[6].Y = 2;

			}
			else if (i == 18)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;
				currentPiece->blocks[1].X = 1;
				currentPiece->blocks[1].Y = 0;
				currentPiece->blocks[2].X = 2;
				currentPiece->blocks[2].Y = 0;
				currentPiece->blocks[3].X = 2;
				currentPiece->blocks[3].Y = -1;

			}

			//Calculate the width and height of the current piece
			currentPiece->width = calcWidth(currentPiece);
			currentPiece->height = calcHeight(currentPiece);

			if (titlePieces != NULL)
				titlePieces[i] = currentPiece;

		}

	}

	return titlePieces;

}