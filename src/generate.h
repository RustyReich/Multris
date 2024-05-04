#ifndef GENERATE_H_
#define GENERATE_H_

#include "MGF.h"

void createCenterBlock(piece* piece);
void copyBlocks(piece* piece1, piece* piece2);
void copyPiece(piece* piece1, piece* piece2);
void delPiece(piece** Piece);
bool isOccupied(piece* Piece, unsigned short blockID, unsigned short direction);
bool isSurrounded(piece* Piece, unsigned short blockID);
unsigned short calcWidth(piece* Piece);
unsigned short calcHeight(piece* Piece);
piece* generateGamePiece(unsigned short size);

#endif