#include "HEADERS/MGF.h"

//draw.c
void intToTexture(int num, SDL_Texture* dstTexture, int X, int Y, float multiplier, unsigned short color);
SDL_Texture* createTexture(int width, int height);
void clearTexture(SDL_Texture* texture);
void drawTexture(SDL_Texture* texture, int X, int Y, float multiplier);
int getStringLength(char* str, float multiplier);
void clearTextureWithColor(SDL_Texture* texture, Uint8 colorR, Uint8 colorG, Uint8 colorB, Uint8 colorA);
int getIntStringLength(int num, float multiplier);

//map.c
unsigned short drawTitle(piece** firstPiece);

//playMode.c
unsigned short playMode(piece* firstPiece);

//generate.c
void delPiece(piece** Piece);
piece* generateGamePiece(unsigned short size);

//controls.c
unsigned short controlsScreen(piece** Piece);

//layout.c
void drawPlayField(SDL_Texture* background, unsigned short size);
void drawScoreBox(SDL_Texture* background, unsigned short size, bool inCustomMode);
void drawLevelBox(SDL_Texture* background, unsigned short size);
void drawUntilBox(SDL_Texture* background, unsigned short size);
void drawNextBox(SDL_Texture* background, unsigned short size);
void drawHoldBox(SDL_Texture* background, unsigned short size);
void drawFPSBox(SDL_Texture* background, unsigned short size);
int calcMapWidth();
int calcMapHeight();
void drawBackgroundExtras(SDL_Texture* background, unsigned short size);
int getGameWidth(unsigned short size);
int getGameHeight(unsigned short size);

//instance.c
void scaleRenderer();

//The main game loop
void mainLoop()
{

	static SDL_Texture* Texture_Background;
	if (Texture_Background == NULL) {

		//Set renderer to correect dimensions for selected size
		int width = getGameWidth(MODE);
		int height = getGameHeight(MODE);
		SDL_RenderSetLogicalSize(globalInstance->renderer, width, height);
		scaleRenderer();

		//Create background texture
		Texture_Background = createTexture(width, height);

		//Draw all layout stuff
		drawPlayField(Texture_Background, MODE);
		drawScoreBox(Texture_Background, MODE, CUSTOM_MODE);
		drawLevelBox(Texture_Background, MODE);
		drawUntilBox(Texture_Background, MODE);
		drawNextBox(Texture_Background, MODE);
		drawHoldBox(Texture_Background, MODE);
		drawFPSBox(Texture_Background, MODE);
		drawBackgroundExtras(Texture_Background, MODE);

	}

	static bool firstFrame = true;
	
	static float fpsSizeMultiplier = 0.75;
	static int fpsTextureWidth;

	if (firstFrame)
	{

		fpsTextureWidth = getStringLength("00000", fpsSizeMultiplier);

		firstFrame = false;

	}

	//Create texture for FPS
	static SDL_Texture* Texture_FPS;
	if (Texture_FPS == NULL)
		Texture_FPS = createTexture(fpsTextureWidth, FONT_HEIGHT * fpsSizeMultiplier);

	//The piece that shows up in the NEXT box on the title screen will be the first piece that is placed if
	//playing a mode that supports the size of that piece
	static bool generateFirst = true;
	static piece* firstPiece;
	if (generateFirst == true)
	{

		firstPiece = generateGamePiece(rand() % MAX_PIECE_SIZE + 1);
		generateFirst = false;
		
	}

	//Stores the current game state
	static unsigned short game_state = TITLE_SCREEN;
	
	//Display frame rate
		// Get width of current frame rate in pixels
	int fpsWidth = getIntStringLength(globalInstance->FPS, fpsSizeMultiplier);
	if (globalInstance->frame_time != 0.0)
	{

		// Clear FPS texture with black
		clearTextureWithColor(Texture_FPS, 0, 0, 0, 255);
		// Print current FPS to the right side of the FPS texture
		intToTexture(globalInstance->FPS, Texture_FPS, fpsTextureWidth - fpsWidth, 0, fpsSizeMultiplier, YELLOW);

	}

	// RENDERING --------------------------------------------------------------
	
	drawTexture(Texture_Background, 0, 0, 1.0);
	// Draw FPS texture so that any unusued space is outside of the drawable area
	drawTexture(Texture_FPS, 0 - (fpsTextureWidth - fpsWidth), 0, 1.0);

	if (game_state == TITLE_SCREEN) {

		game_state = drawTitle(&firstPiece);

		//If MODE != 0, then NUMERICAL mode was selected. So we need to redraw the background layout.
			//If Texture_Background is set to NULL, it will be re-drawn on the next frame
		if (game_state == PLAY_SCREEN && MODE != 0) {

			//Clear and delete background texture
			clearTexture(Texture_Background);
			SDL_DestroyTexture(Texture_Background);
			Texture_Background = NULL;

		}

	}
	else if (game_state == PLAY_SCREEN) {

		//The first loop into playMode
			//We can tell it's the first loop because firstPiece hasn't yet been set back to NULL
		if (firstPiece != NULL) {

			//Save the map width and height into global variables so that it doesn't have to be re-calculated
			//every time we need it
			MAP_WIDTH = calcMapWidth();
			MAP_HEIGHT = calcMapHeight();

			//If the firstPiece is not a valid piece for the selected game mode, delete it and generate one
			//that is valid
			if (MODE != 0 && MODE != firstPiece->numOfBlocks) {

				delPiece(&firstPiece);
				firstPiece = generateGamePiece(MODE);

			}
			game_state = playMode(firstPiece);

			//We can delete the firstPiece after passing it to playMode
			delPiece(&firstPiece);

		}
		else     //The firstPiece has been deleted since it is no longer needed
			game_state = playMode(NULL);	//So we just pass NULL in its place

	}
	else if (game_state == CONTROLS_SCREEN)
		game_state = controlsScreen(&firstPiece);
	else if (game_state == RESET)	//Reset the game to the main menu
	{
		
		//Reset MODE back to 0
		MODE = 0;

		//Lets also reset the background again
		clearTexture(Texture_Background);
		SDL_DestroyTexture(Texture_Background);
		Texture_Background = NULL;

		//Return to the TITLE_SCREEN when the game is reset
		game_state = TITLE_SCREEN;

		// We need to re-generate the firstPiece when we RESET
		generateFirst = true;

	}

}