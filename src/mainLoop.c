#include <time.h>
#include <stdbool.h>

#include "HEADERS/MGF.h"

//draw.c
void printToTexture(char* string, SDL_Texture* dstTexture, int X, int Y, 
	float multiplier, unsigned short color);
void intToTexture(int num, SDL_Texture* dstTexture, int X, int Y, 
	float multiplier, unsigned short color);
unsigned short getIntLength(int num);
SDL_Texture* createTexture(int width, int height);
void clearTexture(SDL_Texture* texture);
void drawTexture(SDL_Texture* texture, int X, int Y, float multiplier);
int getStringLength(char* str, float multiplier);

//map.c
unsigned short drawTitle(piece** firstPiece, unsigned short* returnMode);
void drawBorders(SDL_Texture* background);

//playMode.c
unsigned short playMode(piece* firstPiece, unsigned short size);

//file.c
unsigned int loadTop();

//generate.c
void delPiece(piece** Piece);
piece* generateGamePiece(unsigned short size);

//controls.c
unsigned short controlsScreen();

//The main game loop
void mainLoop()
{

	//Texture storing everything on screen that never changes
		//This includes things like the border walls, strings, etc.
	static SDL_Texture* Texture_Background = NULL;
	if (Texture_Background == NULL)
	{

		//Get renderer dimensions
		int renderWidth;
		int renderHeight;
		SDL_RenderGetLogicalSize(globalInstance->renderer, &renderWidth, 
			&renderHeight);

		Texture_Background = createTexture(renderWidth, renderHeight);

		//Draw borders
		drawBorders(Texture_Background);

		//Print all strings
		printToTexture("TOP", Texture_Background, FONT_WIDTH * 39, 
			FONT_HEIGHT * 2, 1, WHITE);
		//print the top score
		char* top;
		unsigned short zeroLength = 6 - getIntLength(loadTop());
		top = malloc(zeroLength * sizeof(*top));
		*(top + zeroLength) = '\0';
		for (unsigned short i = 0; i < zeroLength; i++)
			*(top + i) = '0';
		printToTexture(top, Texture_Background, FONT_WIDTH * 39, 
			FONT_HEIGHT * 3 + STRING_GAP, 1, WHITE);
		free(top);
		intToTexture(loadTop(), Texture_Background, 
			FONT_WIDTH * 39 + zeroLength * (FONT_WIDTH + STRING_GAP), 
			FONT_HEIGHT * 3 + STRING_GAP, 1, WHITE);

		printToTexture("SCORE", Texture_Background, FONT_WIDTH * 39, 
			FONT_HEIGHT * 5, 1, WHITE);
		printToTexture("NEXT", Texture_Background, FONT_WIDTH * 40, 
			FONT_HEIGHT * 23, 1, WHITE);
		printToTexture("HOLD", Texture_Background, FONT_WIDTH * 40, 
			FONT_HEIGHT * 37, 1, WHITE);
		printToTexture("LEVEL:", Texture_Background, FONT_WIDTH * 39, 
			FONT_HEIGHT * 10, 1, WHITE);
		printToTexture("LINES", Texture_Background, (int)(FONT_WIDTH * 39.5), 
			FONT_HEIGHT * 14, 1, WHITE);
		printToTexture("UNTIL", Texture_Background, (int)(FONT_WIDTH * 39.5), 
			FONT_HEIGHT * 15 + STRING_GAP, 1, WHITE);
		printToTexture("LEVELUP", Texture_Background, (int)(FONT_WIDTH * 38.5), 
			FONT_HEIGHT * 16 + 2 * STRING_GAP, 1, WHITE);
		printToTexture("FPS:", Texture_Background, FONT_WIDTH * 39, 
			FONT_HEIGHT * 47, 1, WHITE);

	}
	
	static SDL_Texture* Texture_FPS;
	if (Texture_FPS == NULL)
	{

		Texture_FPS = createTexture(getStringLength("0000", 1.0), FONT_HEIGHT);

	}

	//Hold the first piece that is generated in the title screen and will be the first 
	//WWpiece played
	static piece* firstPiece;

	//Stores the current game state
	static unsigned short game_state = TITLE_SCREEN;

	//For determing what mode to launch playMode in
	static unsigned short mode;

	//Display frame rate
	if (globalInstance->frame_time != 0.0)
	{

		clearTexture(Texture_FPS);
		intToTexture(globalInstance->FPS, Texture_FPS, 0, 0, 1.0, YELLOW);

	}

	//Exit game if user presses EXIT_BUTTON
	if (globalInstance->controls[EXIT_BUTTON_ID].onPress)
		globalInstance->running = false;

	if (game_state == TITLE_SCREEN)
		game_state = drawTitle(&firstPiece, &mode);
	else if (game_state == PLAY_SCREEN)
	{

		if (firstPiece != NULL)
		{

			if (mode != 0)
			{

				delPiece(&firstPiece);
				firstPiece = generateGamePiece(mode);

			}
			game_state = playMode(firstPiece, mode);

			//We can delete the firstPiece after passing it to playMode
			delPiece(&firstPiece);

		}
		else     //The firstPiece has been deleted since it is no longer needed
			game_state = playMode(NULL, mode);	//So we just pass NULL in its place

	}
	else if (game_state == CONTROLS_SCREEN)
		game_state = controlsScreen();
	else if (game_state == RESET)	//Reset the game to the main menu
	{
		
		//So lets reset the background just in case the top score has been updated
		clearTexture(Texture_Background);
		SDL_DestroyTexture(Texture_Background);
		Texture_Background = NULL;

		game_state = TITLE_SCREEN;

	}

	// RENDERING --------------------------------------------------------------
	drawTexture(Texture_Background, 0, 0, 1.0);
	drawTexture(Texture_FPS, FONT_WIDTH * 43 + STRING_GAP, FONT_HEIGHT * 47, 
		1.0);

}