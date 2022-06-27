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
int getIntStringLength(int num, float multiplier);

//map.c
unsigned short drawTitle(piece** firstPiece, unsigned short* returnMode);

//playMode.c
unsigned short playMode(piece* firstPiece);

//file.c
unsigned int loadTop();

//generate.c
void delPiece(piece** Piece);
piece* generateGamePiece(unsigned short size);

//controls.c
unsigned short controlsScreen();

//layout.c
void drawPlayField(SDL_Texture* background, unsigned short size);
void drawScoreBox(SDL_Texture* background, unsigned short size);
void drawLevelBox(SDL_Texture* background, unsigned short size);
void drawUntilBox(SDL_Texture* background, unsigned short size);
void drawNextBox(SDL_Texture* background, unsigned short size);
void drawHoldBox(SDL_Texture* background, unsigned short size);
void drawFPSBox(SDL_Texture* background, unsigned short size);
int calcMapWidth();
int calcMapHeight();

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

		//Draw layout
		drawPlayField(Texture_Background, MAX_PIECE_SIZE);
		drawScoreBox(Texture_Background, MAX_PIECE_SIZE);
		drawLevelBox(Texture_Background, MAX_PIECE_SIZE);
		drawUntilBox(Texture_Background, MAX_PIECE_SIZE);
		drawNextBox(Texture_Background, MAX_PIECE_SIZE);
		drawHoldBox(Texture_Background, MAX_PIECE_SIZE);
		drawFPSBox(Texture_Background, MAX_PIECE_SIZE);

	}
	
	static SDL_Texture* Texture_FPS;
	if (Texture_FPS == NULL)
		Texture_FPS = createTexture(getStringLength("0000", 1.0), FONT_HEIGHT);

	//Hold the first piece that is generated in the title screen and will be the first 
	//piece played
	static piece* firstPiece;
	if (firstPiece == NULL)
		firstPiece = generateGamePiece(rand() % MAX_PIECE_SIZE + 1);

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

	// RENDERING --------------------------------------------------------------
	drawTexture(Texture_Background, 0, 0, 1.0);
	drawTexture(Texture_FPS, 
				SPRITE_WIDTH * (round(BASE_PLAYFIELD_WIDTH * MAX_PIECE_SIZE) + 7) - 6 -
				getIntStringLength(globalInstance->FPS, 1.0) / 2, 
				FONT_HEIGHT * 39 + STRING_GAP * 3, 1.0);

	//Exit game if user presses EXIT_BUTTON
	if (globalInstance->controls[EXIT_BUTTON].onPress)
		globalInstance->running = false;

	if (game_state == TITLE_SCREEN)
	{

		game_state = drawTitle(&firstPiece, &mode);
		MODE = mode;

	}
	else if (game_state == PLAY_SCREEN)
	{

		//The first loop into playMode
		if (firstPiece != NULL)
		{

			MAP_WIDTH = calcMapWidth();
			MAP_HEIGHT = calcMapHeight();

			if (mode != 0)
			{

				delPiece(&firstPiece);
				firstPiece = generateGamePiece(mode);

			}
			game_state = playMode(firstPiece);

			//We can delete the firstPiece after passing it to playMode
			delPiece(&firstPiece);

		}
		else     //The firstPiece has been deleted since it is no longer needed
			game_state = playMode(NULL);	//So we just pass NULL in its place

	}
	else if (game_state == CONTROLS_SCREEN)
		game_state = controlsScreen();
	else if (game_state == RESET)	//Reset the game to the main menu
	{
		
		//So lets reset the background just in case the top score has been updated
		clearTexture(Texture_Background);
		SDL_DestroyTexture(Texture_Background);
		Texture_Background = NULL;

		//Return to the TITLE_SCREEN when the game is reset
		game_state = TITLE_SCREEN;

	}

}