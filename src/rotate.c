#include "HEADERS/MGF.h"

#define swap(a, b, t) { t tmp = a; a = b; b = tmp; } 

void inversePiece(piece* Piece);
void mirrorPieceOverY(piece* Piece);

//Rotate a piece
void rotatePiece(piece *Piece, unsigned short direction) {

	//If rotating clockwise, we inverse the piece and then mirror it
	if (direction == CW) {

		inversePiece(Piece);
		mirrorPieceOverY(Piece);

	}
	else if (direction == CCW) {	//If rotating counter-clockwise, we mirror and then inverse

		mirrorPieceOverY(Piece);
		inversePiece(Piece);

	}

}

//Inverse the x and y values of every block in a piece
void inversePiece(piece* Piece) {

	//Swap all X and Y values
	for (unsigned short i = 0; i < Piece->numOfBlocks; i++)
		swap(Piece->blocks[i].X, Piece->blocks[i].Y, signed short);

	//Swap width & height, minX & minY
	swap(Piece->width, Piece->height, unsigned short);
	swap(Piece->minX, Piece->minY, unsigned short);

}

//Mirror the piece over a vertical center line
void mirrorPieceOverY(piece* Piece) {

	signed short center = Piece->minX + Piece->width / 2;

	for (unsigned short i = 0; i < Piece->numOfBlocks; i++) {

		Piece->blocks[i].X = center - Piece->blocks[i].X;
		if (i == 0 || Piece->blocks[i].X < Piece->minX)
			Piece->minX = Piece->blocks[i].X;

	}

}