#ifndef CONTROLS_H_
#define CONTROLS_H_

#include "MGF.h"

unsigned short controlsScreen(piece** Piece);
void updateControls();
int getPressedKey();
bool invalidKey(int key);

#endif