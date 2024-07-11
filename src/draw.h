#ifndef DRAW_H_
#define DRAW_H_

#include "MGF.h"

unsigned short getColor_R(unsigned short color);
unsigned short getColor_G(unsigned short color);
unsigned short getColor_B(unsigned short color);
unsigned short getIntLength(int num);
void printToTexture(char* string, SDL_Texture* dstTexture, int X, int Y, float multiplier, unsigned short color);
void intToTexture(int num, SDL_Texture* dstTexture, int X, int Y, float multiplier, unsigned short color);
void drawToTexture(int SpriteID, SDL_Texture* dstTexture, int X, int Y, float multiplier, Uint8 color);
void drawRectangle(int spriteID, SDL_Texture* dstTexture, int X, int Y, int W, int H, int color, bool fill);
void drawSimpleRect(SDL_Texture* dstTexture, int x, int y, int width, int height, int color);
int getStringLength(char* str, float multiplier);
int getIntStringLength(int num, float multiplier);
void clearTexture(SDL_Texture* texture);
void clearTextureWithColor(SDL_Texture* texture, Uint8 colorR, Uint8 colorG, Uint8 colorB, Uint8 colorA);
SDL_Texture* createTexture(int width, int height);
void drawTexture(SDL_Texture* texture, int X, int Y, float multiplier);
void drawPiece(piece Piece, SDL_Texture* dstTexture, unsigned short X, unsigned short Y);
SDL_Texture* createPieceTexture(piece Piece, bool drawCenterDot);
void stringToUpper(char* str);

#endif