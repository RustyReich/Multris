#include <stdbool.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include <stdbool.h>

#define STRUCTURES_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

typedef struct control
{

    Uint16 button;

    bool onPress;
    bool onHold;

} control;

typedef struct gameInstance
{

    SDL_Window* window;
    SDL_DisplayMode DM;
    SDL_Renderer* renderer;
    SDL_Event event;
    SDL_Surface* windowSurface;

    int minimizedWindow_W;
    int minimizedWindow_H;

    SDL_AudioDeviceID* audioDevice;
    SDL_AudioSpec* wavSpec;

    SDL_Texture* stringSS;
    int stringSS_W;
	int stringSS_H;
    SDL_Texture* gameSS;
    int gameSS_W;
    int gameSS_H;

    bool running;

    int FPS;
    double frame_time;

    unsigned short global_volume;

    Uint8* keys;

    control* controls;

} gameInstance;

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