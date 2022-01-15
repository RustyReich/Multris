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
unsigned short getOption(unsigned short line);
unsigned short getLineCount(char* fileName);

//instance.c
void initInstance(gameInstance** instance);
void scaleRenderer();
void setWindowMode(unsigned short mode);

//controls.c
void updateControls();

//Initialize global variables
unsigned short BLOCK_SPRITE_ID = BLOCK_SPRITE_ID_1;
unsigned short UPDATE_FULLSCREEN_MODE = true;

//Global Instance
gameInstance *globalInstance;

int main(int argc, char* argv[])
{

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

	//Used to check if this is the very first frame of the game
	bool firstLoop = true;

	int frames_per_DS = 0;

	Uint32 prevTicks = 0;
	int deltaTicks = 0;

	//Initialize game instance
	initInstance(&globalInstance);

	//Create options file if it doesn't exist or has the wrong number of lines
	if (!fileExists("SAVES/options.cfg") || getLineCount("SAVES/options.cfg") != NUM_OF_OPTIONS)
		createOptions();

	if (fileExists("SAVES/options.cfg"))
	{

		//Load the BLOCK_CHAR from options file
		if (getOption(0) == 0)
			BLOCK_SPRITE_ID = BLOCK_SPRITE_ID_1;
		else if (getOption(0) == 1)
			BLOCK_SPRITE_ID = BLOCK_SPRITE_ID_2;

		//Load volume from options file
		globalInstance->global_volume = getOption(3);

	}
	
	while (globalInstance->running)
	{

		 while (SDL_PollEvent(&globalInstance->event))
		 {

			//User quit
            if (globalInstance->event.type == SDL_QUIT)
                globalInstance->running = false;

            //Window resized
            if (globalInstance->event.type == SDL_WINDOWEVENT)
			{

                if (globalInstance->event.window.event == 
                    SDL_WINDOWEVENT_SIZE_CHANGED)
                        scaleRenderer();

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

                globalInstance->FPS = (int)((frames_per_DS - 
                    ((float)deltaTicks - 100) * frames_per_DS / deltaTicks) * 10);
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
		mainLoop();

		//Render the current frame
		SDL_RenderPresent(globalInstance->renderer);

		//Update window in and out of fullscreen
			//For some reason this has to be done at the end of the frame or else it wont update correctly on launch
		if (UPDATE_FULLSCREEN_MODE)
		{

			setWindowMode(getOption(2));

			//We are not loger updating the fullscreen mode
			UPDATE_FULLSCREEN_MODE = false;

		}

	}

	SDL_DestroyRenderer(globalInstance->renderer);
	SDL_DestroyWindow(globalInstance->window);
	SDL_Quit();

	return 0;

}