#ifndef NETWORK_H_
#define NETWORK_H_

#include "MGF.h"

void sendMapToServer(int* mapData, int* lastPuleTime);
void sendScoretoServer(int score, int* lastPulseTime);
void sendNextPieceToServer(piece* nextPiece, int* lastPulseTime);
void sendLevelToServer(int level, int* lastPulseTime);
void sendCurrentPieceToServer(piece* currentPiece, int *lastPulseTime);
void sendPositionToServer(int X, int Y, int* lastPulseTime);
void sendLinesToServer(int lines, int* lastPulseTime);
void sendHoldPieceToServer(piece* holdPiece, int* lastPulseTime);

#endif