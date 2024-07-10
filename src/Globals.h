#ifndef GLOBALS_H_
#define GLOBALS_H_

#include "MGF.h"

extern gameInstance *globalInstance;

//Global variables
extern bool UPDATE_FULLSCREEN_MODE;

extern bool FULLSCREEN_MODE;
extern unsigned short VOLUME;
extern unsigned short MUSIC_VOLUME;
extern bool LIMIT_FPS;
extern bool SHOW_FPS;
extern bool CENTER_DOT;
extern bool GRAPHICS;

extern int PROGRESS;

extern int MODE;
extern int MAP_WIDTH;
extern int MAP_HEIGHT;

extern bool CUSTOM_MODE;

extern int BLOCK_SPRITE_ID;
extern int WALL_SPRITE_ID;

extern bool DEBUG;

extern bool MULTIPLAYER;

#endif