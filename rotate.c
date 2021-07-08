#include "Structures.h"
#include "technicalDefinitions.h"

void inversePiece(piece*);
void mirrorPieceOverY(piece*);

void swap(signed short*, signed short*);

void rotatePiece(piece *Piece, unsigned short direction)
{

	//If rotating clockwise, we inverse the piece and then mirror it
	if (direction == CW)
	{

		inversePiece(Piece);
		mirrorPieceOverY(Piece);

	}
	else if (direction == CCW)
	{
		//If rotating counter-clockwise, we mirror and then inverse
		mirrorPieceOverY(Piece);
		inversePiece(Piece);

	}

}

void inversePiece(piece* Piece)
{

	//Swap all X and Y values
	for (unsigned short i = 0; i < Piece->numOfBlocks; i++)
		swap(&Piece->blocks[i].X, &Piece->blocks[i].Y);

	//Swap width & height, minX & minY
	swap(&Piece->width, &Piece->height);
	swap(&Piece->minX, &Piece->minY);

}

//Mirror the piece over a vertical center line
void mirrorPieceOverY(piece* Piece)
{

	signed short center = Piece->minX + Piece->width / 2;

	for (unsigned short i = 0; i < Piece->numOfBlocks; i++)
	{

		Piece->blocks[i].X = center - Piece->blocks[i].X;
		if (i == 0 || Piece->blocks[i].X < Piece->minX)
			Piece->minX = Piece->blocks[i].X;

	}

}

//Swap two numbers
void swap(signed short* A, signed short* B)
{

	signed short temp = *A;
	*A = *B;
	*B = temp;

}