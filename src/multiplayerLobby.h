#ifndef MULTIPLAYERLOBBY_H_
#define MULTIPLAYERLOBBY_H_

#include "MGF.h"

unsigned short multiplayerLobby(piece** Piece, char* serverMessage);
void loadIPandPortForConnectScreen(char* ipString, char* portString);
void loadPortForHostScreen(char* portString);

#endif