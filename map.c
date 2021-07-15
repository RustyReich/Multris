#include <stdbool.h>
#include <stdio.h>
#include <windows.h>

#include "Structures.h"
#include "technicalDefinitions.h"

//draw.c
void drawRectangle(sprite, unsigned short, unsigned short, unsigned short, unsigned short, unsigned short, bool,
	SDL_Renderer*);
void print_string(char*, unsigned short, unsigned short, unsigned short, unsigned short, SDL_Renderer*, sprite*);
void drawPiece(piece, unsigned short, unsigned short, sprite, SDL_Renderer*);
void drawSprite(sprite, unsigned short, unsigned short, unsigned short, unsigned short, SDL_Renderer*);

//generate.c
piece* generateGamePiece(unsigned short size);
void delPiece(piece** Piece);
piece* makeTitlePieces();
void copyBlocks(piece*, piece*);
void copyPiece(piece*, piece*);

//audio.c
void playSound(sound*, SDL_AudioDeviceID*, SDL_AudioSpec*);
sound* loadSound(char*, SDL_AudioSpec*);
void delSound(sound** Sound);

//texture.c
SDL_Texture* createTexture(SDL_Renderer*, unsigned short, unsigned short);
void drawTexture(SDL_Texture*, unsigned short, unsigned short, SDL_Renderer*);
SDL_Texture* createPieceTexture(piece, sprite, SDL_Renderer*);

//file.c
void saveOption(unsigned short line, unsigned short value);
bool fileExists(char* fileName);
unsigned short getOption(unsigned short line);

piece* getMovingPieces(piece*);
bool updateTitle(SDL_Texture*, piece*, double, sprite, SDL_Renderer*);
void updateModes(SDL_Texture*, unsigned short, sprite*, SDL_Renderer*);
void updateOptions(SDL_Texture*, unsigned short, sprite*, SDL_Renderer*);

unsigned short drawTitle(sprite* Sprites, double frame_time, SDL_Renderer* renderer, piece** firstPiece, 
	SDL_AudioDeviceID* audioDevice, SDL_AudioSpec* wavSpec, unsigned short* returnMode)
{

	//Generate random game piece
	static piece* Piece;
	if (Piece == NULL)
		Piece = generateGamePiece(rand() % MAX_PIECE_SIZE + 1);

	//Create title card pieces
	static piece* titlePieces;
	if (titlePieces == NULL)
		titlePieces = makeTitlePieces();
	static piece* movingPieces;
	//The coordinates of each title card piece
	static unsigned short* pieceX;
	static unsigned short* pieceY;
	static double* Y;
	if (pieceX == NULL || pieceY == NULL || Y == NULL)
	{

		pieceX = malloc(NUM_OF_TITLE_PIECES * sizeof(pieceX[0]));
		pieceY = malloc(NUM_OF_TITLE_PIECES * sizeof(pieceY[0]));
		Y = malloc(sizeof(*Y));

		//Initialize all the X and Y values of each titlePiece
		if (pieceX != 0)
			memcpy(pieceX, (unsigned short[])
		{
			0, 2, 3, 4, 6, 9, 11, 14, 16, 17, 20, 20, 21, 22, 23, 25, 27, 29, 29
		},
				NUM_OF_TITLE_PIECES * sizeof pieceX[0]);
		if (pieceY != 0)
			memcpy(pieceY, (unsigned short[])
		{
			0, 2, 0, 2, 0, 0, 0, 0, 3, 0, 0, 3, 2, 0, 1, 0, 0, 0, 3
		},
				NUM_OF_TITLE_PIECES * sizeof pieceY[0]);

		//Starting Y value of the title
		*Y = 12;

	}

	//Textures
	static SDL_Texture* Texture_Title;
	if (Texture_Title == NULL)
	{

		Texture_Title = createTexture(renderer, CHAR_DIMENSION * 32, CHAR_DIMENSION * 5);

		//Draw the title card
		SDL_SetRenderTarget(renderer, Texture_Title);
		if (pieceX != NULL && pieceY != NULL)
			for (unsigned short i = 0; i < NUM_OF_TITLE_PIECES; i++)
				drawPiece(titlePieces[i], CHAR_DIMENSION * pieceX[i], CHAR_DIMENSION * pieceY[i], 
					Sprites[BLOCK_CHAR], renderer);
		SDL_SetRenderTarget(renderer, NULL);

		//Free arrays
		free(pieceX);
		pieceX = NULL;
		free(pieceY);
		pieceY = NULL;

		//Get the pieces of the title that will be falling independently of everything else once it hits the
		//bottom of the screen
		movingPieces = getMovingPieces(titlePieces);

		//free titlePieces
		for (unsigned short i = 0; i < NUM_OF_TITLE_PIECES; i++)
			delPiece((piece**)(&titlePieces[i]));
		free(titlePieces);
		titlePieces = NULL;

	}
	static SDL_Texture* Texture_Next;
	static int* nextText_Width;
	if (nextText_Width == NULL)
		nextText_Width = malloc(sizeof(*nextText_Width));
	static int* nextText_Height;
	if (nextText_Height == NULL)
		nextText_Height = malloc(sizeof(*nextText_Height));
	if (Texture_Next == NULL)
	{

		Texture_Next = createPieceTexture(*Piece, Sprites[BLOCK_CHAR], renderer);
		SDL_QueryTexture(Texture_Next, NULL, NULL, nextText_Width, nextText_Height);

	}
	static SDL_Texture* Texture_Score;
	if (Texture_Score == NULL)
	{

		Texture_Score = createTexture(renderer, 6 * (CHAR_DIMENSION + LETTER_GAP), CHAR_DIMENSION);

		SDL_SetRenderTarget(renderer, Texture_Score);
		print_string("000000", 0, 0, 1, WHITE, renderer, Sprites);
		SDL_SetRenderTarget(renderer, NULL);

	}
	static SDL_Texture* Texture_Level;
	if (Texture_Level == NULL)
	{

		Texture_Level = createTexture(renderer, 2 * (CHAR_DIMENSION + LETTER_GAP), CHAR_DIMENSION);

		SDL_SetRenderTarget(renderer, Texture_Level);
		print_string("0", 0, 0, 1, RED, renderer, Sprites);
		SDL_SetRenderTarget(renderer, NULL);

	}
	static SDL_Texture* Texture_Lines;
	if (Texture_Lines == NULL)
	{

		Texture_Lines = createTexture(renderer, CHAR_DIMENSION, CHAR_DIMENSION);
		SDL_SetRenderTarget(renderer, Texture_Lines);
		print_string("5", 0, 0, 1, ORANGE, renderer, Sprites);
		SDL_SetRenderTarget(renderer, NULL);

	}
	static SDL_Texture* Texture_Modes;
	if (Texture_Modes == NULL)
	{

		Texture_Modes = createTexture(renderer, CHAR_DIMENSION * 35, CHAR_DIMENSION * 6 + LETTER_GAP);
		SDL_SetRenderTarget(renderer, Texture_Modes);
		print_string("Mode:", 0, 2 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer, Sprites);
		print_string("Multris", 8 * CHAR_DIMENSION, 2 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer, Sprites);
		print_string("Numerical", 8 * CHAR_DIMENSION, 3 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer,
			Sprites);
		print_string("Options", 8 * CHAR_DIMENSION, 4 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer,
			Sprites);
		print_string(">", 6 * CHAR_DIMENSION, 2 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer, Sprites);
		SDL_SetRenderTarget(renderer, NULL);

	}
	static SDL_Texture* Texture_Options;
	if (Texture_Options == NULL)
	{

		Texture_Options = createTexture(renderer, 20 * (CHAR_DIMENSION + LETTER_GAP), 2 * (CHAR_DIMENSION + LETTER_GAP));
		SDL_SetRenderTarget(renderer, Texture_Options);
		print_string(">", 0, 0, 1, WHITE, renderer, Sprites);
		print_string("BLOCK SPRITE:", 2 * (CHAR_DIMENSION + LETTER_GAP), 0, 1, WHITE, renderer, Sprites);
		print_string("Ghost:", 2 * (CHAR_DIMENSION + LETTER_GAP), CHAR_DIMENSION + LETTER_GAP, 1, WHITE, renderer, Sprites);
		print_string("^", 15 * (CHAR_DIMENSION + LETTER_GAP), 1 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer, Sprites);

		//Print current GHOST option
		if (getOption(1) == 0)
			print_string("OFF", 9 * (CHAR_DIMENSION + LETTER_GAP), CHAR_DIMENSION + LETTER_GAP, 1, WHITE, renderer, Sprites);
		else if (getOption(1) == 1)
			print_string("ON", 9 * (CHAR_DIMENSION + LETTER_GAP), CHAR_DIMENSION + LETTER_GAP, 1, WHITE, renderer, Sprites);

		unsigned short color = (rand() % (RED - YELLOW + 1)) + YELLOW;
		drawSprite(Sprites[BLOCK_CHAR_1], 15 * (CHAR_DIMENSION + LETTER_GAP), 0, 1, color, renderer);
		drawSprite(Sprites[BLOCK_CHAR_2], 17 * (CHAR_DIMENSION + LETTER_GAP), 0, 1, color, renderer);

		SDL_SetRenderTarget(renderer, NULL);

	}

	//Sounds
	static sound* Sound_Move;
	static sound* Sound_Rotate;
	if (Sound_Move == NULL)
		Sound_Move = loadSound("move.wav", wavSpec);
	if (Sound_Rotate == NULL)
		Sound_Rotate = loadSound("rotate.wav", wavSpec);

	//Variables
	static bool* titleUpdating;
	if (titleUpdating == NULL)
	{

		titleUpdating = malloc(sizeof(*titleUpdating));
		if (titleUpdating != NULL)
			*titleUpdating = false;

	}
	static unsigned short* mode;
	if (mode == NULL)
	{

		mode = malloc(sizeof(*mode));
		if (mode != NULL)
			*mode = 0;

	}

	//Rendering -------------------------------------------

	drawTexture(Texture_Title, CHAR_DIMENSION * 2, CHAR_DIMENSION * (int)*Y, renderer);
	drawTexture(Texture_Next, (int)(CHAR_DIMENSION * 42.5) - *nextText_Width / 2,
		(int)(CHAR_DIMENSION * 29.5) - *nextText_Height / 2, renderer);
	drawTexture(Texture_Score, CHAR_DIMENSION * 39, CHAR_DIMENSION * 6 + LETTER_GAP, renderer);
	drawTexture(Texture_Level, CHAR_DIMENSION * 46, CHAR_DIMENSION * 10, renderer);
	drawTexture(Texture_Lines, CHAR_DIMENSION * 42, CHAR_DIMENSION * 18, renderer);
	drawTexture(Texture_Modes, CHAR_DIMENSION * 2, CHAR_DIMENSION * 5, renderer);

	if (*mode >= OPTIONS)
		drawTexture(Texture_Options, 134, 103, renderer);

	//-----------------------------------------------------

	//Title dropping
	if (*Y < HEIGHT_IN_CHARS - 6)
		*Y += INITIAL_SPEED * frame_time;
	else
	{

		//Once the title is done dropping, there are a couple pieces in the title that should continue dropping
		//These pieces are called movingPieces. There are 5 of them
			//We call updateTitle until all five of them have finished dropping as well
		if (titleUpdating != NULL)
			if (*titleUpdating == false)
				*titleUpdating = updateTitle(Texture_Title, movingPieces, frame_time, Sprites[BLOCK_CHAR], 
					renderer);

	}

	static char* inputLock;		//This prevents the user from holding down certain buttons
	if (inputLock == NULL)
	{

		inputLock = malloc(sizeof(*inputLock));
		if (inputLock != NULL)
			*inputLock = true;

	}
	else
	{

		//Exit Menu if enter key is pressed
			//This means clearing all of the memory used by the Title screen
		if (GetAsyncKeyState(VK_RETURN) && *inputLock == false)
		{

			//Play COMPLETE sound
			playSound(Sound_Rotate, audioDevice, wavSpec);

			//Return the mode 
			*returnMode = *mode;

			if (*returnMode != OPTIONS + 2)
			{

				//Free Textures
				SDL_DestroyTexture(Texture_Title);
				Texture_Title = NULL;
				SDL_DestroyTexture(Texture_Next);
				Texture_Next = NULL;
				SDL_DestroyTexture(Texture_Score);
				Texture_Score = NULL;
				SDL_DestroyTexture(Texture_Level);
				Texture_Level = NULL;
				SDL_DestroyTexture(Texture_Lines);
				Texture_Lines = NULL;
				SDL_DestroyTexture(Texture_Modes);
				Texture_Modes = NULL;
				SDL_DestroyTexture(Texture_Options);
				Texture_Options = NULL;

				//Free sounds
				delSound(&Sound_Move);
				delSound(&Sound_Rotate);

				//Free single variables
				free(Y);
				Y = NULL;
				free(inputLock);
				inputLock = NULL;
				free(titleUpdating);
				titleUpdating = NULL;
				free(nextText_Width);
				nextText_Width = NULL;
				free(nextText_Height);
				nextText_Height = NULL;
				free(mode);
				mode = NULL;

			}
			else
			{

				//Swap GHOST option
				saveOption(1, !getOption(1));
				updateOptions(Texture_Options, *mode, Sprites, renderer);

			}

			//Only copy first piece if we are not resetting, meaning not in OPTIONS
			if (*returnMode < OPTIONS)
			{

				//Copy Piece into firstPiece
				*firstPiece = Piece;
				//Copy over all blocks
				copyBlocks(Piece, *firstPiece);

			}
			else
				delPiece(&Piece);	//Otherwise, delete the piece

			if (*returnMode != OPTIONS + 2)
			{

				Piece = NULL;

				//free movingPieces
				for (unsigned short i = 0; i < 5; i++)
					delPiece((piece**)(&movingPieces[i]));
				free(movingPieces);
				movingPieces = NULL;

			}

			//Change the block sprite if thats what the player did
			if (*returnMode == MAX_PIECE_SIZE + 1)
			{

				BLOCK_CHAR = BLOCK_CHAR_1;
				//Save BLOCK_CHAR to options file
				if (fileExists("options.cfg"))
					saveOption(0, 0);
				return RESET;

			}
			else if (*returnMode == MAX_PIECE_SIZE + 2)
			{

				BLOCK_CHAR = BLOCK_CHAR_2;
				if (fileExists("options.cfg"))
					saveOption(0, 1);
				return RESET;

			}
			else if (*returnMode <= MAX_PIECE_SIZE)
				return PLAY_SCREEN;	//If thats not what they did, then theyre trying to play

			*inputLock = true;

		}	//Mode 0 = default mode, Mode 1-9 = Numerical mode, Mode 10+ = OPTIONS
		else if (GetAsyncKeyState(VK_DOWN) && *inputLock == false && mode != NULL)
		{

			if (*mode == 0)
			{

				playSound(Sound_Move, audioDevice, wavSpec);
				*mode = 1;
				updateModes(Texture_Modes, *mode, Sprites, renderer);

			}
			else if (*mode == 1)
			{

				playSound(Sound_Move, audioDevice, wavSpec);
				*mode = OPTIONS;
				updateModes(Texture_Modes, *mode, Sprites, renderer);
				updateOptions(Texture_Options, *mode, Sprites, renderer);

			}
			else if (*mode == OPTIONS || *mode == OPTIONS + 1)
			{

				playSound(Sound_Move, audioDevice, wavSpec);
				*mode = OPTIONS + 2;
				updateOptions(Texture_Options, *mode, Sprites, renderer);

			}

			*inputLock = true;

		}
		else if (GetAsyncKeyState(VK_UP) && *inputLock == false && mode != NULL)
		{

			if (*mode > 0 && *mode <= MAX_PIECE_SIZE)
			{

				playSound(Sound_Move, audioDevice, wavSpec);
				*mode = 0;
				updateModes(Texture_Modes, *mode, Sprites, renderer);

			}
			else if (*mode == OPTIONS || *mode == OPTIONS + 1)
			{

				playSound(Sound_Move, audioDevice, wavSpec);
				*mode = 1;
				updateModes(Texture_Modes, *mode, Sprites, renderer);

			}
			else if (*mode == OPTIONS + 2)
			{

				playSound(Sound_Move, audioDevice, wavSpec);
				*mode = OPTIONS;
				updateOptions(Texture_Options, *mode, Sprites, renderer);

			}

			*inputLock = true;

		}
		else if (GetAsyncKeyState(VK_LEFT) && *inputLock == false && mode != NULL)
		{

			if (*mode > 1 && *mode <= MAX_PIECE_SIZE)
			{

				playSound(Sound_Move, audioDevice, wavSpec);
				*mode -= 1;
				updateModes(Texture_Modes, *mode, Sprites, renderer);

			}
			else if (*mode == OPTIONS + 1)
			{

				playSound(Sound_Move, audioDevice, wavSpec);
				*mode -= 1;
				updateOptions(Texture_Options, *mode, Sprites, renderer);

			}

			*inputLock = true;

		}
		else if (GetAsyncKeyState(VK_RIGHT) && *inputLock == false && mode != NULL)
		{

			if (*mode < MAX_PIECE_SIZE && *mode > 0)
			{

				playSound(Sound_Move, audioDevice, wavSpec);
				*mode += 1;
				updateModes(Texture_Modes, *mode, Sprites, renderer);

			}
			else if (*mode == OPTIONS)
			{

				playSound(Sound_Move, audioDevice, wavSpec);
				*mode += 1;
				updateOptions(Texture_Options, *mode, Sprites, renderer);

			}

			*inputLock = true;

		}
		else if (!GetAsyncKeyState(VK_RETURN) && !GetAsyncKeyState(VK_DOWN) && !GetAsyncKeyState(VK_UP) &&
			!GetAsyncKeyState(VK_LEFT) && !GetAsyncKeyState(VK_RIGHT))
			*inputLock = false;

	}

	return TITLE_SCREEN;

}

void updateOptions(SDL_Texture* texture, unsigned short mode, sprite* Sprites, SDL_Renderer* renderer)
{

	SDL_SetRenderTarget(renderer, texture);

	if (mode == OPTIONS)
	{

		//Erase "^" and ">"
		print_string("^", 17 * (CHAR_DIMENSION + LETTER_GAP), 1 * (CHAR_DIMENSION + LETTER_GAP), 1, BLACK, renderer, Sprites);
		print_string(">", 0, CHAR_DIMENSION + LETTER_GAP, 1, BLACK, renderer, Sprites);
		//Reprint
		print_string("^", 15 * (CHAR_DIMENSION + LETTER_GAP), 1 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer, Sprites);
		print_string(">", 0, 0, 1, WHITE, renderer, Sprites);

	}
	else if (mode == OPTIONS + 1)
	{

		//Erase "^" and ">" by printing it in black
		print_string("^", 15 * (CHAR_DIMENSION + LETTER_GAP), 1 * (CHAR_DIMENSION + LETTER_GAP), 1, BLACK, renderer, Sprites);
		print_string(">", 0, CHAR_DIMENSION + LETTER_GAP, 1, BLACK, renderer, Sprites);
		//Reprint
		print_string("^", 17 * (CHAR_DIMENSION + LETTER_GAP), 1 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer, Sprites);
		print_string(">", 0, 0, 1, WHITE, renderer, Sprites);

	}
	else if (mode == OPTIONS + 2)
	{

		//Erase "^" and ">"
		print_string("^", 15 * (CHAR_DIMENSION + LETTER_GAP), 1 * (CHAR_DIMENSION + LETTER_GAP), 1, BLACK, renderer, Sprites);
		print_string("^", 17 * (CHAR_DIMENSION + LETTER_GAP), 1 * (CHAR_DIMENSION + LETTER_GAP), 1, BLACK, renderer, Sprites);
		print_string(">", 0, 0, 1, BLACK, renderer, Sprites);

		//Print the current GHOST option
		if (getOption(1) == 0)
		{

			//Erase ON
			print_string("ON", 9 * (CHAR_DIMENSION + LETTER_GAP), CHAR_DIMENSION + LETTER_GAP, 1, BLACK, renderer, Sprites);
			//Print OFF
			print_string("OFF", 9 * (CHAR_DIMENSION + LETTER_GAP), CHAR_DIMENSION + LETTER_GAP, 1, WHITE, renderer, Sprites);

		}
		else if (getOption(1) == 1)
		{

			//Erase OFF
			print_string("OFF", 9 * (CHAR_DIMENSION + LETTER_GAP), CHAR_DIMENSION + LETTER_GAP, 1, BLACK, renderer, Sprites);
			//Print ON
			print_string("ON", 9 * (CHAR_DIMENSION + LETTER_GAP), CHAR_DIMENSION + LETTER_GAP, 1, WHITE, renderer, Sprites);

		}

		print_string(">", 0, CHAR_DIMENSION + LETTER_GAP, 1, WHITE, renderer, Sprites);

	}

	//Reset RenderTarger
	SDL_SetRenderTarget(renderer, NULL);

}

void updateModes(SDL_Texture* texture, unsigned short currentMode, sprite* Sprites, SDL_Renderer* renderer)
{

	//Clear modes texture
	SDL_SetRenderTarget(renderer, texture);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	//Redraw the modes texture, changing it depending on what the current mode is
	print_string("Mode:", 0, 2 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer, Sprites);
	print_string(">", 6 * CHAR_DIMENSION, 2* (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer, Sprites);

	if (currentMode == 0)
	{

		print_string("Multris", 8 * CHAR_DIMENSION, 2 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer, Sprites);
		print_string("Numerical", 8 * CHAR_DIMENSION, 3 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer,
			Sprites);
		print_string("Options", 8 * CHAR_DIMENSION, 4 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer,
			Sprites);

	}
	else if (currentMode > 0 && currentMode <= MAX_PIECE_SIZE)
	{

		print_string("Multris", 8 * CHAR_DIMENSION, CHAR_DIMENSION + LETTER_GAP, 1, WHITE, renderer, Sprites);
		print_string("Numerical", 8 * CHAR_DIMENSION, 2 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer, Sprites);
		print_string("Options", 8 * CHAR_DIMENSION, 3 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer,
			Sprites);
		print_string("^", 22 * CHAR_DIMENSION + 2 * LETTER_GAP, 4 * CHAR_DIMENSION, 1, WHITE, renderer, Sprites);

		if (currentMode == 1)
			print_string("  1 2", 20 * CHAR_DIMENSION, 2 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer, Sprites);
		else if (currentMode < MAX_PIECE_SIZE)
		{

			char string[6] = { '0' + currentMode - 1, ' ', '0' + currentMode, ' ', '0' + currentMode + 1, '\0' };
			print_string(string, 20 * CHAR_DIMENSION, 2 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer, Sprites);

		}
		else
			print_string("8 9  ", 20 * CHAR_DIMENSION, 2 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer, Sprites);

	}
	else if (currentMode >= OPTIONS)
	{

		print_string("Multris", 8 * CHAR_DIMENSION, 0, 1, WHITE, renderer, Sprites);
		print_string("Numerical", 8 * CHAR_DIMENSION, 1 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer, Sprites);
		print_string("Options", 8 * CHAR_DIMENSION, 2 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer,
			Sprites);

	}

	//Reset RenderTarger
	SDL_SetRenderTarget(renderer, NULL);

}

//This function is for when the title has hit the floor, but a few pieces of it should still continue falling
bool updateTitle(SDL_Texture* texture, piece* movingPieces, double frame_time, sprite Sprite, 
	SDL_Renderer* renderer)
{
	
	//Return if the "update" animation is finished playing
	bool updateOver = false;

	//X values of each piece that moves after the rest of the title has finished dropping
	unsigned short X[5] = { 2, 17, 22, 23, 27 };
	unsigned short endY[5] = { 4, 4, 1, 2, 3 };	//The Y value that each movingPiece should stop at
	static double* Y;
	if (Y == NULL)
	{

		//Initialize the starting Y values of each movingPiece
		Y = malloc(5 * sizeof(Y[0]));
		if (Y != 0)
			memcpy(Y, (double[])
		{
			2, 0, 0, 1, 0
		},
				5 * sizeof Y[0]);

	}

	SDL_SetRenderTarget(renderer, texture);

	for (unsigned i = 0; i < 5; i++)
	{

		//Save the color for the current piece, and then set the color to black
		unsigned short color = movingPieces[i].color;
		movingPieces[i].color = BLACK;

		if ((Y + i) != NULL)
		{

			//We set the color to black so that we can redraw the same piece on top of itself, effectively
			//erasing it since it is being drawn over with a BLACK copy of itself
				//We only need to do this if the piece is about to drop down 1 cell during this frame
					//That is what we are checking for with this if statement
			if ((int)(*(Y + i) + INITIAL_SPEED * frame_time) > (int)(*(Y + i)))
				drawPiece(movingPieces[i], CHAR_DIMENSION * X[i], CHAR_DIMENSION * (int)*(Y + i),
					Sprite, renderer);

			//Restore the color of the piece
			movingPieces[i].color = color;

			//Drop the piece
			if (*(Y + i) <= endY[i])
				*(Y + i) += INITIAL_SPEED * frame_time;

			//Now redraw the piece
				//Again, we only redraw it if it actually changed cells over this frame
			if ((int)(*(Y + i) - INITIAL_SPEED * frame_time) < (int)(*(Y + i)))
				drawPiece(movingPieces[i], CHAR_DIMENSION * X[i], CHAR_DIMENSION * (int)*(Y + i),
					Sprite, renderer);

		}

	}

	if (Y != NULL)
	{

		//movingPiece[1] has the farthest distance to fall, so once its finished falling we know that the
		//animation is complete
		if (Y[1] >= endY[1])
		{

			//Now that the animation is complete, we can free these Y values and set updateOver to true
			free(Y);
			Y = NULL;

			updateOver = true;

		}

	}

	//Restore the rendering target
	SDL_SetRenderTarget(renderer, NULL);

	return updateOver;

}

piece* getMovingPieces(piece* titlePieces)
{

	static piece* movingPieces;
	movingPieces = malloc(5 * sizeof(*movingPieces));
	
	if (movingPieces != NULL)
	{

		//Allocate memory for the block array of each moving piece
			//titlepieces[1], [9], [13], [14], [16] are the pieces that must continue falling even after the rest
			//of the title has finished
		movingPieces[0].blocks = malloc(titlePieces[1].numOfBlocks * sizeof(*movingPieces[0].blocks));
		movingPieces[1].blocks = malloc(titlePieces[9].numOfBlocks * sizeof(*movingPieces[0].blocks));
		movingPieces[2].blocks = malloc(titlePieces[13].numOfBlocks * sizeof(*movingPieces[0].blocks));
		movingPieces[3].blocks = malloc(titlePieces[14].numOfBlocks * sizeof(*movingPieces[0].blocks));
		movingPieces[4].blocks = malloc(titlePieces[16].numOfBlocks * sizeof(*movingPieces[0].blocks));

		//Copy the pieces over for each movingPiece
		copyPiece(&titlePieces[1], &movingPieces[0]);
		copyPiece(&titlePieces[9], &movingPieces[1]);
		copyPiece(&titlePieces[13], &movingPieces[2]);
		copyPiece(&titlePieces[14], &movingPieces[3]);
		copyPiece(&titlePieces[16], &movingPieces[4]);

	}

	return movingPieces;

}

void drawBorders(sprite sprite, SDL_Renderer* renderer)
{

	//Draw all borders
	drawRectangle(sprite, 0, 0, 50, 1, GRAY, true, renderer);
	drawRectangle(sprite, 0, CHAR_DIMENSION, 1, 49, GRAY, true, renderer);
	drawRectangle(sprite, CHAR_DIMENSION, CHAR_DIMENSION * 49, 49, 1, GRAY, true, renderer);
	drawRectangle(sprite, CHAR_DIMENSION * 36, CHAR_DIMENSION * 35, 13, 11, GRAY, true, renderer);
	drawRectangle(sprite, CHAR_DIMENSION * 49, CHAR_DIMENSION, 1, 48, GRAY, true, renderer);
	drawRectangle(sprite, CHAR_DIMENSION * 35, CHAR_DIMENSION, 1, 48, GRAY, true, renderer);
	drawRectangle(sprite, CHAR_DIMENSION * 36, CHAR_DIMENSION * 8, 13, 1, GRAY, true, renderer);
	drawRectangle(sprite, CHAR_DIMENSION * 36, CHAR_DIMENSION * 12, 13, 1, GRAY, true, renderer);
	drawRectangle(sprite, CHAR_DIMENSION * 36, CHAR_DIMENSION * 20, 13, 2, GRAY, true, renderer);

}