#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "HEADERS/technicalDefinitions.h"
#include "HEADERS/Structures.h"

//mainLoop.c
void mainLoop(SDL_Renderer*, sprite*);

//Comp_Decomp.c
sprite* loadSprites(size_t*);

//file.c
bool fileExists(char*);
void createOptions();
unsigned short getOption(unsigned short);
unsigned short getLineCount(char*);

//window.c
void setWindowMode(unsigned short, SDL_Renderer*, SDL_Window*, SDL_DisplayMode);

//Initialize global variables
unsigned short BLOCK_CHAR = BLOCK_CHAR_1;
unsigned short UPDATE_FULLSCREEN_MODE = true;
unsigned short GLOBAL_VOLUME = 9;
unsigned short RUNNING = true;

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

	SDL_Window* window = SDL_CreateWindow("Multris v1.0.7 | By: Russell Reich", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WINDOW_BOX_DIMENSION, WINDOW_BOX_DIMENSION, SDL_WINDOW_OPENGL);

	//Get the screen resolution
	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	//All game sprites are stored here
	size_t numOfSprites;
	sprite* Sprites = loadSprites(&numOfSprites);

	//Create options file if it doesn't exist or has the wrong number of lines
	if (!fileExists("SAVES/options.cfg") || getLineCount("SAVES/options.cfg") != NUM_OF_OPTIONS)
		createOptions();

	if (fileExists("SAVES/options.cfg"))
	{

		//Load the BLOCK_CHAR from options file
		if (getOption(0) == 0)
			BLOCK_CHAR = BLOCK_CHAR_1;
		else if (getOption(0) == 1)
			BLOCK_CHAR = BLOCK_CHAR_2;

		//Load volume from options file
		GLOBAL_VOLUME = getOption(3);

	}

	SDL_Event event;
	
	while (RUNNING)
	{

		 while (SDL_PollEvent(&event))
			if (event.type == SDL_QUIT)
				RUNNING = false;

		//Clear Screen with black
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		//Main game loop
		mainLoop(renderer, Sprites);

		//Render the current frame
		SDL_RenderPresent(renderer);

		//Update window in and out of fullscreen
			//For some reason this has to be done at the end of the frame or else it wont update correctly on launch
		if (UPDATE_FULLSCREEN_MODE)
		{

			setWindowMode(getOption(2), renderer ,window, DM);

			//We are not loger updating the fullscreen mode
			UPDATE_FULLSCREEN_MODE = false;

		}

	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;

}