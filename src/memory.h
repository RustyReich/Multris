#ifndef MEMORY_H_
#define MEMORY_H_

#include "MGF.h"

varVector** pushAddress(void** ptr, unsigned short type);
bool inVector(void** ptr);
void declare_short(void** ptr, short value);
void declare_unsigned_short(void** ptr, unsigned short value);
void declare_double(void** ptr, double value);
void declare_int(void** ptr, int value);
void declare_char(void** ptr, char value);
void declare_bool(void** ptr, bool value);
void declare_unsigned_int(void** ptr, unsigned int value);
void declare_Piece(piece** ptr, piece* Piece);
void declare_Piece_Text(SDL_Texture** ptr, piece* Piece, bool drawCenterDot);
void declare_HUD_Text(SDL_Texture** ptr, int type);
void declare_map_matrix(int** ptr);
void declare_size_bag(SizeBag** ptr, unsigned short mode, bool customMode);
void declare_moving_title_pieces(piece*** ptr);
void declare_UI_list(UI_list** ptr, int type);
void freeVars();
char** extractStringsFromDelimitedBytes(char* data, int dataLength, int* numValues, char delim);

#endif