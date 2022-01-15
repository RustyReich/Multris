#include "HEADERS/MGF.h"

void inversePiece(piece* Piece);
void mirrorPieceOverY(piece* Piece);

void swap_signed(signed short* A, signed short* B);
void swap_unsigned(unsigned short* A, unsigned short* B);

//Rotate a piece
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

//Inverse the x and y values of every block in a piece
void inversePiece(piece* Piece)
{

	//Swap all X and Y values
	for (unsigned short i = 0; i < Piece->numOfBlocks; i++)
		swap_signed(&Piece->blocks[i].X, &Piece->blocks[i].Y);

	//Swap width & height, minX & minY
	swap_unsigned(&Piece->width, &Piece->height);
	swap_signed(&Piece->minX, &Piece->minY);

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

//Swap two signed numbers
void swap_signed(signed short* A, signed short* B)
{

	signed short temp = *A;
	*A = *B;
	*B = temp;

}

//Swap two unsigned numbers
void swap_unsigned(unsigned short* A, unsigned short* B)
{

	unsigned short temp = *A;
	*A = *B;
	*B = temp;

}