#ifndef STRUCTURES_H_
#define STRUCTURES_H_

#include "MGF.h"

typedef struct UI
{

    SDL_Texture* texture;

    //Position of the UI element, relative to the renderer
    int x;
    int y;

    //Wether the current UI element is currently being interacted with by the player
    bool currentlyInteracting;

} UI;

typedef struct UI_list
{

    UI* ui;
    char** entry_texts;
    int selected_entry;
    int num_entries;

} UI_list;

typedef struct varVector
{

    void** ptrs;
    unsigned short* types;

    int count;

} varVector;

typedef struct control
{

    Uint16 button;

    bool onPress;
    bool onHold;

} control;

typedef struct sound
{

	Uint32 length;
	Uint8* buffer;

} sound;

typedef struct gameInstance
{

    SDL_Window* window;
    SDL_DisplayMode DM;
    SDL_Renderer* renderer;
    SDL_Event event;
    SDL_Surface* windowSurface;

    int minimizedWindow_W;
    int minimizedWindow_H;
    int minimizedWindow_X;
    int minimizedWindow_Y;

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

    Uint8* keys;

    control* controls;

    sound** sounds;         //This holds the volume adjusted sounds
    sound** masterSounds;   //This holds the original sonuds, that are not volume adjusted

} gameInstance;

typedef struct block
{

	signed short X;
	signed short Y;

} block;

typedef struct piece
{

	block* blocks;

    block* centerBlock;

	unsigned short numOfBlocks;
	unsigned short color;

	unsigned short width;
	unsigned short height;

	signed short minX;
	signed short minY;

} piece;

typedef struct SizeBag
{

    unsigned short size;
    unsigned short* sizesInBag;

} SizeBag;

#endif