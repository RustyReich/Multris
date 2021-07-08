#include <SDL.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

#include "technicalDefinitions.h"
#include "Structures.h"

//mainLoop.c
void mainLoop(SDL_Renderer*, sprite*);

//Comp_Decomp.c
sprite* loadSprites(size_t*);

int main(int argc, char* argv[])
{

	//Set seed for random number generator
	srand((int)time(NULL));

	//SDL stuff
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Init(SDL_INIT_AUDIO);

	SDL_Window* window = SDL_CreateWindow("Multris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		CHAR_DIMENSION * HEIGHT_IN_CHARS * ASPECT_RATIO, CHAR_DIMENSION * HEIGHT_IN_CHARS, SDL_WINDOW_OPENGL);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_Event event;

	//Running state
	bool running = true;

	//All game sprites are stored here
	size_t numOfSprites;
	sprite* Sprites = loadSprites(&numOfSprites);
	
	while (running)
	{

		//Check for closing the window
		while (SDL_PollEvent(&event))
			if (event.type == SDL_QUIT)
				running = false;

		//Clear Screen with black
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		//Main game loop
		mainLoop(renderer, Sprites);

		//Render the current frame
		SDL_RenderPresent(renderer);

	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;

}