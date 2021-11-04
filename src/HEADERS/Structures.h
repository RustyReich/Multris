#include "SDL2/SDL.h"

typedef struct rectangle
{

	unsigned short Width;
	unsigned short Height;

	unsigned short X;
	unsigned short Y;

} rectangle;

typedef struct sprite
{

	rectangle* rectangles;
	
	unsigned short numOfRect;

} sprite;

typedef struct block
{

	signed short X;
	signed short Y;

} block;

typedef struct piece
{

	block* blocks;

	unsigned short numOfBlocks;
	unsigned short color;

	unsigned short width;
	unsigned short height;

	signed short minX;
	signed short minY;

} piece;

typedef struct sound
{

	Uint32 length;
	Uint8* buffer;

} sound;