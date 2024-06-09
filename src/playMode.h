#ifndef PLAYMODE_H_
#define PLAYMODE_H_

#include "MGF.h"

unsigned short playMode(piece* firstPiece);
void removeSizeFromBag(SizeBag* sizeBag, unsigned short size, unsigned short mode, bool customMode, SDL_Texture* sizeBagTexture);
void removeSizeFromBagTexture(SDL_Texture* sizeBagTexture, unsigned short size);
void resetSizeBagTexture(SDL_Texture* sizeBagTexture);
double calcSpeed(unsigned short level);
unsigned short calcLinesUntilLevelup(unsigned short linesAtCurrentLevel, unsigned short currentLevel);
unsigned short calcGhostY(piece* Piece, int X, int startY, int* mapData, int mapWidth, int mapHeight);
bool playOverAnimation(SDL_Texture* foreground, unsigned short mapWidth, unsigned short mapHeight);
void updateLines(unsigned short lines, SDL_Texture** linesTexture);
void updateLevel(unsigned short level, SDL_Texture* levelTexture);
void updateScore(unsigned int score, SDL_Texture* scoreTexture);
bool playLineAnimation(SDL_Texture* foreground, unsigned short row, bool *clearingLine, int *mapData, unsigned short* numCompleted);
void removeLine(unsigned short row, int* mapData, SDL_Texture* foreground, unsigned short mapWidth);
bool lineIsComplete(int* mapData, unsigned short row, unsigned short mapWidth);
unsigned short completedLine(int* mapData, int Y, piece Piece, int** returnRows, int mapWidth, int mapHeight);
void adjustNewPiece(piece* Piece, signed short* X, unsigned short mapWidth);
bool isColliding(piece Piece, int X, double* Y, int direction, int* mapData, int mapWidth, int mapHeight);
void move(char keyPress, signed short *X, piece Piece, unsigned short mapWidth);
void sendMapToServer(int* mapData, int* lastPuleTime);
void sendScoretoServer(int score, int* lastPulseTime);
void sendNextPieceToServer(piece* nextPiece, int* lastPulseTime);
void sendLevelToServer(int level, int* lastPulseTime);
void sendCurrentPieceToServer(piece* currentPiece, int *lastPulseTime);
void sendPositionToServer(int X, int Y, int* lastPulseTime);

#endif