#ifndef MGF_H_
#define MGF_H_

#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <limits.h>
#include <time.h>
#include <pthread.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_net.h"
#include "SDL2/SDL_mixer.h"

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

// OS-specific includes/macros
#ifdef _WIN32
#include <windows.h>
#define mkdir(dir, mode) mkdir(dir)
#define pthread_cancel(thread_id) TerminateThread(OpenThread(THREAD_TERMINATE, false, thread_id), 0)
#endif

#endif