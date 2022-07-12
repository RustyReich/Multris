#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "HEADERS/MGF.h"

//mainLoop.c
void mainLoop();

//file.c
bool fileExists(char* fileName);
void createOptions();
int getFileValue(const char* file_path, const char* name);
unsigned short getLineCount(char* fileName);
bool brokenOptions();
void createControls();
bool brokenControls();
void createWindowFile();
bool brokenWindowFile();
void saveToFile(const char* file_path, const char* str, int value);

//instance.c
void initInstance(gameInstance** instance);
void scaleRenderer();
void setWindowMode(unsigned short mode);
void updateVolume();

//controls.c
void updateControls();

//layout.c
int calcMapWidth();
int calcMapHeight();

//Initialize global variables
bool UPDATE_FULLSCREEN_MODE = true;

bool FULLSCREEN_MODE = false;
unsigned short VOLUME = 10;
bool LIMIT_FPS = true;

int MODE = 0;
int MAP_WIDTH = 0;
int MAP_HEIGHT = 0;

//Global Instance
gameInstance *globalInstance;

int main(int argc, char* argv[])
{

	//Set MAP_WIDTH and MAP_HEIGHT
	MAP_WIDTH = calcMapWidth();
	MAP_HEIGHT = calcMapHeight();

	//Set seed for random number generator
	srand((int)time(NULL));

	//Initialize SDL stuff
    if( SDL_Init( SDL_INIT_VIDEO ) < 0)
	{

        fprintf( stderr, "Could not initialize SDL video: %s\n", SDL_GetError() );
        exit( -1 );

    }
	if ( SDL_Init( SDL_INIT_AUDIO ) < 0)
	{

		fprintf( stderr, "Could not initialize SDL audio: %s\n", SDL_GetError() );
		exit( -1 );

	}

	//Initialize game instance
	initInstance(&globalInstance);

	//Used to check if this is the very first frame of the game
	bool firstLoop = true;

	int frames_per_DS = 0;

	//Variables for estimating FPS every 100ms
	Uint32 prevTicks = 0;
	int deltaTicks = 0;

	//Variables for limiting FPS
	Uint32 ticksLastFrame = SDL_GetTicks();
	//targetFrameTime will be that of a montior with double the refresh rate of the
	//current monitor
	double targetFrameTime = (double)1 / (double)(globalInstance->DM.refresh_rate * 2);

	//Create options file if it doesn't exist or it is broken
	if (!fileExists("SAVES/options.cfg") || brokenOptions())
			createOptions();

	//Create controls file if it doesn't exist or it is broken
	if (!fileExists("SAVES/controls.cfg") || brokenControls())
		createControls();

	//Create window size file if it doesn't exist or it is broken
	if (!fileExists("SAVES/window.cfg") || brokenWindowFile())
		createWindowFile();

	//Load window size from window file
	if (fileExists("SAVES/window.cfg"))
	{
		
		//Get minimizedWindow size and position
		globalInstance->minimizedWindow_W = getFileValue("SAVES/window.cfg", "WIDTH");
		globalInstance->minimizedWindow_H = getFileValue("SAVES/window.cfg", "HEIGHT");
		globalInstance->minimizedWindow_X = getFileValue("SAVES/window.cfg", "X");
		globalInstance->minimizedWindow_Y = getFileValue("SAVES/window.cfg", "Y");

		//Set window size
		SDL_SetWindowSize(globalInstance->window, globalInstance->minimizedWindow_W, 
													globalInstance->minimizedWindow_H);
		//Set window position
		SDL_SetWindowPosition(globalInstance->window, globalInstance->minimizedWindow_X,
														globalInstance->minimizedWindow_Y);

	}

	//Load controls from controls file
	if (fileExists("SAVES/controls.cfg"))
	{

	    globalInstance->controls[LEFT_BUTTON].button = getFileValue("SAVES/controls.cfg", "LEFT");
		globalInstance->controls[RIGHT_BUTTON].button = getFileValue("SAVES/controls.cfg", "RIGHT");
		globalInstance->controls[SOFT_DROP_BUTTON].button = getFileValue("SAVES/controls.cfg", "SOFT DROP");
		globalInstance->controls[HARD_DROP_BUTTON].button = getFileValue("SAVES/controls.cfg", "HARD DROP");
		globalInstance->controls[HOLD_BUTTON].button = getFileValue("SAVES/controls.cfg", "HOLD");
		globalInstance->controls[ROTATE_CCW_BUTTON].button = getFileValue("SAVES/controls.cfg", "ROTATE CCW");
		globalInstance->controls[ROTATE_CW_BUTTON].button = getFileValue("SAVES/controls.cfg", "ROTATE CW");
		globalInstance->controls[MIRROR_BUTTON].button = getFileValue("SAVES/controls.cfg", "MIRROR");
		globalInstance->controls[SELECT_BUTTON].button = getFileValue("SAVES/controls.cfg", "SELECT");
		globalInstance->controls[EXIT_BUTTON].button = getFileValue("SAVES/controls.cfg", "EXIT");
		globalInstance->controls[DOWN_BUTTON].button = getFileValue("SAVES/controls.cfg", "DOWN");
		globalInstance->controls[UP_BUTTON].button = getFileValue("SAVES/controls.cfg", "UP");

	}

	//Load some options from the option file
	if (fileExists("SAVES/options.cfg"))
	{

		FULLSCREEN_MODE = getFileValue("SAVES/options.cfg", "FULLSCREEN");
		VOLUME = getFileValue("SAVES/options.cfg", "VOLUME");
		LIMIT_FPS = getFileValue("SAVES/options.cfg", "LIMIT FPS");

	}

	//Update volume to that loaded from options file
	updateVolume();
	
	while (globalInstance->running)
	{

		while (SDL_PollEvent(&globalInstance->event))
		{

			//User quit
            if (globalInstance->event.type == SDL_QUIT)
                globalInstance->running = false;

            if (globalInstance->event.type == SDL_WINDOWEVENT)
			{

                if (globalInstance->event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
					scaleRenderer();
				if (globalInstance->event.window.event == SDL_WINDOWEVENT_RESIZED && !UPDATE_FULLSCREEN_MODE)
				{

					if (!FULLSCREEN_MODE)
					{

						//Only save window size when it changed because the user actually manually resized
						//the window
						SDL_GetWindowSize(globalInstance->window, &globalInstance->minimizedWindow_W, 
																	&globalInstance->minimizedWindow_H);
						saveToFile("SAVES/window.cfg", "WIDTH", globalInstance->minimizedWindow_W);
						saveToFile("SAVES/window.cfg", "HEIGHT", globalInstance->minimizedWindow_H);

					}

				}
				if (globalInstance->event.window.event == SDL_WINDOWEVENT_MOVED && !UPDATE_FULLSCREEN_MODE)
				{

					if (!FULLSCREEN_MODE)
					{

						//Only save window position when it changed because the user actually manually moved
						//the window
						SDL_GetWindowPosition(globalInstance->window, &globalInstance->minimizedWindow_X, 
																		&globalInstance->minimizedWindow_Y);

						saveToFile("SAVES/window.cfg", "X", globalInstance->minimizedWindow_X);
						saveToFile("SAVES/window.cfg", "Y", globalInstance->minimizedWindow_Y);

					}

				}

			}

		}

		//Update 'controls' states
		updateControls();

		//Clear screen with black
        SDL_SetRenderDrawColor(globalInstance->renderer, 0, 0, 0, 255);
        SDL_RenderClear(globalInstance->renderer);

		//Calcualte frame_time and FPS-------------------------------------------
            //FPS and frame_time get updated every ~100 ms (10 times per second)
        if (!firstLoop)
        {

            frames_per_DS++;

            deltaTicks = SDL_GetTicks() - prevTicks;

            if (deltaTicks >= 100)
            {
				//printf("test\n");
                globalInstance->FPS = (int)((frames_per_DS-((float)deltaTicks-100)*frames_per_DS/deltaTicks)*10);
                frames_per_DS = 0;

                prevTicks = SDL_GetTicks();

            }

        }
        else
        {   

            prevTicks = SDL_GetTicks();

            //Scale renderer to fit window
            scaleRenderer();

            //It is no longer the first loop
            firstLoop = false;

        }

        if (globalInstance->FPS > 0)
            globalInstance->frame_time = (double)1 / (double)globalInstance->FPS;

        //-----------------------------------------------------------------------

		//Main game loop
		if (LIMIT_FPS)
		{

			//Logic for limitting the FPS
				//Basically just sleeps every frame if not enough time passed between frames
			int deltaMS = SDL_GetTicks() - ticksLastFrame;
			if (deltaMS < targetFrameTime * 1000)
				SDL_Delay(targetFrameTime * 1000 - deltaMS);
			ticksLastFrame = SDL_GetTicks();

			mainLoop();

		}
		else
			mainLoop();

		//Render the current frame
		SDL_RenderPresent(globalInstance->renderer);

		//Update window in and out of fullscreen
			//For some reason this has to be done at the end of the frame or else it wont 
			//update correctly on launch
		if (UPDATE_FULLSCREEN_MODE)
		{

			setWindowMode(FULLSCREEN_MODE);

			//We are not loger updating the fullscreen mode
			UPDATE_FULLSCREEN_MODE = false;

		}

	}

	SDL_DestroyRenderer(globalInstance->renderer);
	SDL_DestroyWindow(globalInstance->window);
	SDL_Quit();

	return 0;

}