#ifndef MGF_H_
#define MGF_H_

#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <limits.h>
#include <time.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_net.h"

#include "Structures.h"
#include "Globals.h"
#include "Definitions.h"
#include "Macros.h"
#include "audio.h"
#include "controls.h"
#include "draw.h"
#include "file.h"
#include "generate.h"
#include "instance.h"
#include "layout.h"
#include "main.h"
#include "mainLoop.h"
#include "map.h"
#include "memory.h"
#include "playMode.h"
#include "rotate.h"
#include "multiplayerLobby.h"
#include "network.h"

//Fix for cross-compiling mkdir() for windows on linux
#ifdef _WIN32
#include <windows.h>
#define mkdir(dir, mode) mkdir(dir)
#endif

#endif