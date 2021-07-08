#include <time.h>
#include <stdbool.h>

#include "technicalDefinitions.h"
#include "Structures.h"

//draw.c
void print_string(char*, unsigned short, unsigned short, unsigned short, unsigned short, SDL_Renderer*, sprite*);
void print_int(int, unsigned short, unsigned short, unsigned short, unsigned short, SDL_Renderer*, sprite*);
unsigned short getIntLength(int num);

//audio.c
SDL_AudioDeviceID* prepareAudioDevice(SDL_AudioSpec**);

//map.c
unsigned short drawTitle(sprite*, double, SDL_Renderer*, piece**, SDL_AudioDeviceID*, SDL_AudioSpec*, 
	unsigned short*);
void drawBorders(sprite, SDL_Renderer*);

//playMode.c
unsigned short playMode(sprite*, double, SDL_Renderer*, piece*, SDL_AudioDeviceID*, SDL_AudioSpec*, unsigned short);

//file.c
unsigned int loadTop();

//generate.c
void delPiece(piece** Piece);
piece* generateGamePiece(unsigned short size);

void mainLoop(SDL_Renderer* renderer, sprite* Sprites)
{
	
	//Used to check if this is the very first frame of the game
	static firstLoop = true;

	//Texture storing everything on screen that never changes
		//This includes things like the border walls, strings, etc.
	static SDL_Texture* background = NULL;
	if (background == NULL)
	{

		background = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
			CHAR_DIMENSION * 50, CHAR_DIMENSION * 50);

		SDL_SetRenderTarget(renderer, background);

		//Draw borders
		drawBorders(Sprites[WALL_CHAR], renderer);
		//Print all strings

		print_string("TOP", CHAR_DIMENSION * 39, CHAR_DIMENSION * 2, 1, WHITE, renderer, Sprites);
		
		//print the top score
		char* top;
		unsigned short zeroLength = 6 - getIntLength(loadTop());
		top = malloc(zeroLength * sizeof(*top));
		*(top + zeroLength) = '\0';
		for (unsigned short i = 0; i < zeroLength; i++)
			*(top + i) = '0';
		print_string(top, CHAR_DIMENSION * 39, CHAR_DIMENSION * 3 + LETTER_GAP, 1, WHITE, renderer, Sprites);
		free(top);
		print_int(loadTop(), CHAR_DIMENSION * 39 + zeroLength * (CHAR_DIMENSION + LETTER_GAP), CHAR_DIMENSION * 3 + LETTER_GAP, 1, WHITE, renderer, Sprites);

		print_string("SCORE", CHAR_DIMENSION * 39, CHAR_DIMENSION * 5, 1, WHITE, renderer, Sprites);
		print_string("NEXT", CHAR_DIMENSION * 40, CHAR_DIMENSION * 23, 1, WHITE, renderer, Sprites);
		print_string("LEVEL:", CHAR_DIMENSION * 39, CHAR_DIMENSION * 10, 1, WHITE, renderer, Sprites);
		print_string("lines", (int)(CHAR_DIMENSION * 39.5), CHAR_DIMENSION * 14, 1, WHITE, renderer, Sprites);
		print_string("until", (int)(CHAR_DIMENSION * 39.5), CHAR_DIMENSION * 15 + LETTER_GAP, 1, WHITE, renderer, Sprites);
		print_string("levelup", (int)(CHAR_DIMENSION * 38.5), CHAR_DIMENSION * 16 + 2 * LETTER_GAP, 1, WHITE, renderer, Sprites);
		print_string("FPS:", CHAR_DIMENSION * 39, CHAR_DIMENSION * 47, 1, WHITE, renderer, Sprites);

		SDL_SetRenderTarget(renderer, NULL);
		SDL_RenderCopy(renderer, background, NULL, NULL);

	}
	else
		SDL_RenderCopy(renderer, background, NULL, NULL);	//Draw the background

	//Audio Technicals
	static SDL_AudioDeviceID* audioDevice;
	static SDL_AudioSpec* wavSpec;
	if (audioDevice == NULL)
		audioDevice = prepareAudioDevice(&wavSpec);

	//Hold the first piece that is generated in the title screen and will be the first piece played
	static piece* firstPiece;

	//Stores the amount of time, in seconds, between this frame and the last frame
	static double frame_time;

	//Stores the current game state
	static unsigned short game_state = TITLE_SCREEN;

	//For determing what mode to launch playMode in
	static unsigned short mode;

	//Calculate frame time
	static clock_t currTime;
	if (!firstLoop)
	{

		frame_time = (float)(clock() - currTime) / CLOCKS_PER_SEC;

		//Pause the program for 1 MS if getting more than 999 FPS
		if (frame_time == 0)
			SDL_Delay(1);
		frame_time = (float)(clock() - currTime) / CLOCKS_PER_SEC;

	}
	currTime = clock();

	//Calculate and display frame rate
	if (frame_time != 0.0)
		print_int((int)(1 / frame_time), CHAR_DIMENSION * 43 + LETTER_GAP, CHAR_DIMENSION * 47, 1, YELLOW, 
			renderer, Sprites);

	if (game_state == TITLE_SCREEN)
		game_state = drawTitle(Sprites, frame_time, renderer, &firstPiece, audioDevice, wavSpec, &mode);
	else if (game_state == PLAY_SCREEN)
	{

		if (firstPiece != NULL)
		{

			if (mode != 0)
			{

				delPiece(&firstPiece);
				firstPiece = generateGamePiece(mode);

			}
			game_state = playMode(Sprites, frame_time, renderer, firstPiece, audioDevice, wavSpec,
				mode);

			//We can delete the firstPiece after passing it to playMode
			delPiece(&firstPiece);

		}
		else     //The firstPiece has been deleted since it is no longer needed
			game_state = playMode(Sprites, frame_time, renderer, NULL, audioDevice, wavSpec, mode);	//So we just pass NULL its place

		//This implies we just lost the game
		if (game_state == TITLE_SCREEN)
		{

			//So lets reset the background just in case the top score has been updated
			SDL_SetRenderTarget(renderer, background);
			SDL_SetRenderDrawColor(renderer,0, 0, 0, 0);
			SDL_RenderClear(renderer);
			SDL_DestroyTexture(background);
			background = NULL;
			SDL_SetRenderTarget(renderer, NULL);

		}

	}

	//It is no longer the first loop
	if (firstLoop)
		firstLoop = false;

}