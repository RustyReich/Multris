#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include "HEADERS/MGF.h"

//draw.c
void drawRectangle(int spriteID, SDL_Texture* dstTexture, unsigned short X, 
	unsigned short Y, unsigned short Width, unsigned short Height, 
	unsigned short color, bool filled);
void printToTexture(char* string, SDL_Texture* dstTexture, int X, int Y, 
	float multiplier, unsigned short color);
void drawPiece(piece Piece, SDL_Texture* dstTexture, unsigned short X, 
	unsigned short Y);
void drawToTexture(unsigned int SpriteID, SDL_Texture* dstTexture, int X, int Y, 
	float multiplier, Uint8 color);
void intToTexture(int num, SDL_Texture* dstTexture, int X, int Y, 
	float multiplier, unsigned short color);
void clearTexture(SDL_Texture* texture);
void drawTexture(SDL_Texture* texture, int X, int Y, float multiplier);
SDL_Texture* createTexture(int width, int height);
SDL_Texture* createPieceTexture(piece Piece);
int getStringLength(char* str, float multiplier);

//generate.c
piece* generateGamePiece(unsigned short size);
void delPiece(piece** Piece);
piece** makeTitlePieces();
void copyBlocks(piece* piece1, piece* piece2);
void copyPiece(piece* piece1, piece* piece2);

//audio.c
void playSound(sound* Sound);
sound* loadSound(char* file);
void delSound(sound** Sound);
void setVolume(sound** Sound, unsigned short volume);

//file.c
void saveOption(unsigned short line, unsigned short value);
bool fileExists(char* fileName);
unsigned short getOption(unsigned short line);

//instance.c
void updateVolume();

piece** getMovingPieces(piece**);
bool updateTitle(SDL_Texture* texture, piece** movingPieces);
void updateModes(SDL_Texture* texture, unsigned short currentMode);
void updateOptions(SDL_Texture* texture, unsigned short mode);

unsigned short drawTitle(piece** firstPiece, unsigned short* returnMode)
{

	//Generate random game piece
	static piece* Piece;
	if (Piece == NULL)
		Piece = generateGamePiece(rand() % MAX_PIECE_SIZE + 1);

	//Create title card pieces
	static piece** titlePieces;
	if (titlePieces == NULL)
		titlePieces = makeTitlePieces();
	static piece** movingPieces;
	
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
		*Y = 15;

	}

	//Textures
	static SDL_Texture* Texture_Title;
	if (Texture_Title == NULL)
	{

		Texture_Title = createTexture(FONT_WIDTH * 32, FONT_HEIGHT * 5);

		//Draw the title card
		if (pieceX != NULL && pieceY != NULL)
			for (unsigned short i = 0; i < NUM_OF_TITLE_PIECES; i++)
				drawPiece(*titlePieces[i], Texture_Title, 
					FONT_WIDTH * pieceX[i], FONT_HEIGHT * pieceY[i]);

		//Free arrays
		free(pieceX);
		pieceX = NULL;
		free(pieceY);
		pieceY = NULL;

		//Get the pieces of the title that will be falling independently of everything else 
		//once it hits the bottom of the screen
		movingPieces = getMovingPieces(titlePieces);

		//free titlePieces
		for (unsigned short i = 0; i < NUM_OF_TITLE_PIECES; i++)
			delPiece(&titlePieces[i]);
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

		Texture_Next = createPieceTexture(*Piece);
		SDL_QueryTexture(Texture_Next, NULL, NULL, nextText_Width, 
			nextText_Height);

	}
	static SDL_Texture* Texture_Score;
	if (Texture_Score == NULL)
	{

		Texture_Score = createTexture(6 * (FONT_WIDTH + STRING_GAP), 
			FONT_HEIGHT);

		printToTexture("000000", Texture_Score, 0, 0, 1, WHITE);

	}
	static SDL_Texture* Texture_Level;
	if (Texture_Level == NULL)
	{

		Texture_Level = createTexture(2 * (FONT_WIDTH + STRING_GAP), 
			FONT_HEIGHT);

		printToTexture("0", Texture_Level, 0, 0, 1, RED);

	}
	static SDL_Texture* Texture_Lines;
	if (Texture_Lines == NULL)
	{

		Texture_Lines = createTexture(FONT_WIDTH, FONT_HEIGHT);
		printToTexture("5", Texture_Lines, 0, 0, 1, ORANGE);

	}
	static SDL_Texture* Texture_Modes;
	if (Texture_Modes == NULL)
	{

		Texture_Modes = createTexture(FONT_WIDTH * 35, 
			FONT_HEIGHT * 6 + STRING_GAP);

		printToTexture("MULTRIS", Texture_Modes, 
			FONT_WIDTH + STRING_GAP, 2 * (FONT_HEIGHT + STRING_GAP), 1, WHITE);
		printToTexture("NUMERICAL", Texture_Modes, 
			FONT_WIDTH + STRING_GAP, 3 * (FONT_HEIGHT + STRING_GAP), 1, WHITE);
		printToTexture("OPTIONS", Texture_Modes, 
			FONT_WIDTH + STRING_GAP, 4 * (FONT_HEIGHT + STRING_GAP), 1, WHITE);
		printToTexture(">", Texture_Modes, 0, 
			2 * (FONT_HEIGHT + STRING_GAP), 1, WHITE);


	}
	static SDL_Texture* Texture_Options;
	if (Texture_Options == NULL)
	{

		Texture_Options = createTexture(20 * (FONT_WIDTH + STRING_GAP), 
			5 * (FONT_HEIGHT + STRING_GAP));

		printToTexture(">", Texture_Options, 0, 0, 1, WHITE);
		printToTexture("BLOCK:", Texture_Options, 
			(FONT_WIDTH + STRING_GAP), 0, 1, WHITE);
		printToTexture("GHOST:", Texture_Options, 
			(FONT_WIDTH + STRING_GAP), FONT_HEIGHT + STRING_GAP, 1, WHITE);
		printToTexture("FULLSCREEN:", Texture_Options, 
			(FONT_WIDTH + STRING_GAP), 2 * (FONT_HEIGHT + STRING_GAP), 1, 
			WHITE);
		printToTexture("VOLUME:", Texture_Options, 
			(FONT_WIDTH + STRING_GAP), 3 * (FONT_HEIGHT + STRING_GAP), 1, 
			WHITE);
		printToTexture("^", Texture_Options, 12 * (FONT_WIDTH + STRING_GAP), 
			1 * (FONT_HEIGHT + STRING_GAP), 1, WHITE);
		printToTexture("CONTROLS", Texture_Options, 
			(FONT_WIDTH + STRING_GAP), 4 * (FONT_HEIGHT + STRING_GAP), 1, 
			WHITE);

		//Print current GHOST option
		if (getOption(1) == 0)
			printToTexture("OFF", Texture_Options,
				8 * (FONT_WIDTH + STRING_GAP), FONT_HEIGHT + STRING_GAP, 1, 
				RED);
		else if (getOption(1) == 1)
			printToTexture("ON", Texture_Options, 
				8 * (FONT_WIDTH + STRING_GAP), FONT_HEIGHT + STRING_GAP, 1, 
				GREEN);

		//Print the current FULLSCREEN options
		if (getOption(2) == 0)
			printToTexture("OFF", Texture_Options, 
				13 * (FONT_WIDTH + STRING_GAP), 2 * (FONT_HEIGHT + STRING_GAP), 
				1, RED);
		else if (getOption(2) == 1)
			printToTexture("ON", Texture_Options, 
				13 * (FONT_WIDTH + STRING_GAP), 2 * (FONT_HEIGHT + STRING_GAP),
				1, GREEN);

		//Print the current VOLUME option
		intToTexture(getOption(3), 
			Texture_Options, 9 * (FONT_WIDTH + STRING_GAP), 3 * (FONT_HEIGHT + STRING_GAP), 
			1, YELLOW);

		//Draw BLOCK_CHAR options
		unsigned short color = (rand() % (RED - YELLOW + 1)) + YELLOW;
		drawToTexture(BLOCK_SPRITE_ID_1, Texture_Options, 
			12 * (FONT_WIDTH + STRING_GAP), 0, 1, color);
		drawToTexture(BLOCK_SPRITE_ID_2, Texture_Options,
			14 * (FONT_WIDTH + STRING_GAP), 0, 1, color);

	}

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

	//drawTexture(Texture_Title, FONT_WIDTH * 2, FONT_HEIGHT * (int)*Y, 1.0);
	drawTexture(Texture_Next, 
				318 - (*nextText_Width / 2),
				282 - (*nextText_Height / 2), 
				1.0);
	drawTexture(Texture_Score, 
				SPRITE_WIDTH * (round(BASE_PLAYFIELD_WIDTH * MAX_PIECE_SIZE) + 1) +
				FONT_WIDTH + 0.5 * (SPRITE_WIDTH * 9 - getStringLength("000000", 1.0)), 
				FONT_HEIGHT * 2 + (FONT_WIDTH + STRING_GAP) * 4 - 10, 1.0);
	drawTexture(Texture_Level, 
				SPRITE_WIDTH * (round(BASE_PLAYFIELD_WIDTH * MAX_PIECE_SIZE) + 1) +
				SPRITE_WIDTH + 0.5 * (SPRITE_WIDTH * 9 - getStringLength("0", 1.0)), 
				FONT_HEIGHT * 9 + (SPRITE_HEIGHT + STRING_GAP) - 7, 1.0);
	drawTexture(Texture_Lines, 
				SPRITE_WIDTH * (round(BASE_PLAYFIELD_WIDTH * MAX_PIECE_SIZE) + 1) +
				FONT_WIDTH + 0.5 * (SPRITE_WIDTH * 9 - getStringLength("5", 1.0)), 
				FONT_HEIGHT * 15 + (SPRITE_HEIGHT + STRING_GAP) - 5, 1.0);
	drawTexture(Texture_Modes, FONT_WIDTH + STRING_GAP, FONT_HEIGHT + STRING_GAP, 1.0);

	if (*mode >= OPTIONS)
		drawTexture(Texture_Options, 2 * (FONT_WIDTH + STRING_GAP), 
					4 * (FONT_WIDTH + STRING_GAP), 1.0);

	//-----------------------------------------------------

	//Title dropping
	if (*Y < BASE_PLAYFIELD_HEIGHT * MAX_PIECE_SIZE)
		*Y += INITIAL_SPEED * globalInstance->frame_time;
	else
	{

		//Once the title is done dropping, there are a couple pieces in the title that should continue dropping
		//These pieces are called movingPieces. There are 5 of them
			//We call updateTitle until all five of them have finished dropping as well
		if (titleUpdating != NULL)
			if (*titleUpdating == false)
				*titleUpdating = updateTitle(Texture_Title, movingPieces);

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
		if (globalInstance->controls[SELECT_BUTTON_ID].onPress && *inputLock == false)
		{

			//Play ROTATE sound
			playSound(globalInstance->sounds[ROTATE_SOUND_ID]);

			//Return the mode 
			*returnMode = *mode;

			//Delete all title screen elements from memory if an option is selected that 
			//leaves or RESETS the title screen
			if (*returnMode < OPTIONS + 2 || *returnMode == OPTIONS + 5)
			{

				//Only copy first piece if we are not resetting
				if (*returnMode != OPTIONS)
					//Copy Piece into firstPiece
					*firstPiece = Piece;
				else
					delPiece(&Piece);	//Otherwise, delete the piece

				//Delete movingPieces if resetting
				if (*returnMode < OPTIONS + 2)
				{

					Piece = NULL;
				
					//free movingPieces
					for (unsigned short i = 0; i < 5; i++)
						delPiece(&movingPieces[i]);
					free(movingPieces);
					movingPieces = NULL;

				}

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
			else if (*returnMode == OPTIONS + 2)
			{

				//Swap GHOST option
				saveOption(1, !getOption(1));
				updateOptions(Texture_Options, *mode);

				GHOST_MODE_ENABLED = getOption(1);

			}
			else if (*returnMode == OPTIONS + 3)
			{

				//Swap FULLSCREEN options
				saveOption(2, !getOption(2));
				updateOptions(Texture_Options, *mode);

				//update the fullscreen mode
				UPDATE_FULLSCREEN_MODE = 1;

			}

			//Change the block sprite if thats what the player did
			if (*returnMode == OPTIONS)
			{

				BLOCK_SPRITE_ID = BLOCK_SPRITE_ID_1;
				//Save BLOCK_CHAR to options file
				if (fileExists("SAVES/options.cfg"))
					saveOption(0, 0);
				return RESET;

			}
			else if (*returnMode == OPTIONS + 1)
			{

				BLOCK_SPRITE_ID = BLOCK_SPRITE_ID_2;
				if (fileExists("SAVES/options.cfg"))
					saveOption(0, 1);
				return RESET;

			}
			else if (*returnMode == OPTIONS + 5)
				return CONTROLS_SCREEN;
			else if (*returnMode <= MAX_PIECE_SIZE)
				return PLAY_SCREEN;	//If thats not what they did, then theyre trying to play

			if (inputLock != NULL)
				*inputLock = true;

		}	//Mode 0 = default mode, Mode 1-8 = Numerical mode, Mode 9+ = OPTIONS
		else if (globalInstance->keys[SDL_SCANCODE_DOWN] && *inputLock == false && 
				mode != NULL)
		{

			if (*mode == 0)
			{

				playSound(globalInstance->sounds[MOVE_SOUND_ID]);
				*mode = 1;
				updateModes(Texture_Modes, *mode);

			}
			else if (*mode >= 1 && *mode <= 8)
			{

				playSound(globalInstance->sounds[MOVE_SOUND_ID]);
				*mode = OPTIONS;
				updateModes(Texture_Modes, *mode);
				updateOptions(Texture_Options, *mode);

			}
			else if (*mode == OPTIONS || *mode == OPTIONS + 1)
			{

				playSound(globalInstance->sounds[MOVE_SOUND_ID]);
				*mode = OPTIONS + 2;
				updateOptions(Texture_Options, *mode);

			}
			else if (*mode == OPTIONS + 2)
			{

				playSound(globalInstance->sounds[MOVE_SOUND_ID]);
				*mode += 1;
				updateOptions(Texture_Options, *mode);

			}
			else if (*mode == OPTIONS + 3)
			{

				playSound(globalInstance->sounds[MOVE_SOUND_ID]);
				*mode += 1;
				updateOptions(Texture_Options, *mode);

			}
			else if (*mode == OPTIONS + 4)
			{

				playSound(globalInstance->sounds[MOVE_SOUND_ID]);
				*mode += 1;
				updateOptions(Texture_Options, *mode);

			}

			*inputLock = true;

		}
		else if (globalInstance->keys[SDL_SCANCODE_UP] && *inputLock == false && 
				mode != NULL)
		{

			if (*mode > 0 && *mode <= MAX_PIECE_SIZE)
			{

				playSound(globalInstance->sounds[MOVE_SOUND_ID]);
				*mode = 0;
				updateModes(Texture_Modes, *mode);

			}
			else if (*mode == OPTIONS || *mode == OPTIONS + 1)
			{

				playSound(globalInstance->sounds[MOVE_SOUND_ID]);
				*mode = 1;
				updateModes(Texture_Modes, *mode);

			}
			else if (*mode == OPTIONS + 2)
			{

				playSound(globalInstance->sounds[MOVE_SOUND_ID]);
				*mode = OPTIONS;
				updateOptions(Texture_Options, *mode);

			}
			else if (*mode == OPTIONS + 3)
			{

				playSound(globalInstance->sounds[MOVE_SOUND_ID]);
				*mode -= 1;
				updateOptions(Texture_Options, *mode);

			}
			else if (*mode == OPTIONS + 4)
			{

				playSound(globalInstance->sounds[MOVE_SOUND_ID]);
				*mode -= 1;
				updateOptions(Texture_Options, *mode);

			}
			else if (*mode == OPTIONS + 5)
			{

				playSound(globalInstance->sounds[MOVE_SOUND_ID]);
				*mode -= 1;
				updateOptions(Texture_Options, *mode);

			}

			*inputLock = true;

		}
		else if (globalInstance->keys[SDL_SCANCODE_LEFT] && *inputLock == false && 
				mode != NULL)
		{

			if (*mode > 1 && *mode <= MAX_PIECE_SIZE)
			{

				playSound(globalInstance->sounds[MOVE_SOUND_ID]);
				*mode -= 1;
				updateModes(Texture_Modes, *mode);

			}
			else if (*mode == OPTIONS + 1)
			{

				playSound(globalInstance->sounds[MOVE_SOUND_ID]);
				*mode -= 1;
				updateOptions(Texture_Options, *mode);

			}
			else if (*mode == OPTIONS + 4)
			{

				if (globalInstance->global_volume > 0)
				{

					//Lower volume in OPTIONS file
					saveOption(3, getOption(3) - 1);
					//Update volume in globalInstance, converting it to a percentage
					globalInstance->global_volume = ((float)getOption(3) / 9) * 100;
					//Now update volumes of the actual sounds
					updateVolume();

					playSound(globalInstance->sounds[MOVE_SOUND_ID]);

				}

				updateOptions(Texture_Options, *mode);

			}

			*inputLock = true;

		}
		else if (globalInstance->keys[SDL_SCANCODE_RIGHT] && *inputLock == false && 
				mode != NULL)
		{

			if (*mode < MAX_PIECE_SIZE && *mode > 0)
			{

				playSound(globalInstance->sounds[MOVE_SOUND_ID]);
				*mode += 1;
				updateModes(Texture_Modes, *mode);

			}
			else if (*mode == OPTIONS)
			{

				playSound(globalInstance->sounds[MOVE_SOUND_ID]);
				*mode += 1;
				updateOptions(Texture_Options, *mode);

			}
			else if (*mode == OPTIONS + 4)
			{

				if (globalInstance->global_volume < 100)
				{

					//Raise volume in OPTIONS file
					saveOption(3, getOption(3) + 1);
					//Update volume in globalInstance, converting it to a percentage
					globalInstance->global_volume = ((float)getOption(3) / 9) * 100;
					//Now update volumes of the actual sounds
					updateVolume();

					playSound(globalInstance->sounds[MOVE_SOUND_ID]);

				}

				updateOptions(Texture_Options, *mode);

			}

			*inputLock = true;

		}
		else if (!globalInstance->keys[SDL_SCANCODE_UP] && 
				!globalInstance->keys[SDL_SCANCODE_DOWN] && 
				!globalInstance->keys[SDL_SCANCODE_LEFT] && 
				!globalInstance->keys[SDL_SCANCODE_RIGHT] &&
				!globalInstance->keys[SDL_SCANCODE_RETURN])
			*inputLock = false;

	}

	return TITLE_SCREEN;

}

void updateOptions(SDL_Texture* texture, unsigned short mode)
{

	if (mode == OPTIONS)
	{

		//Erase "^" and ">"
		printToTexture("^", texture, 14 * (FONT_WIDTH + STRING_GAP), 
			1 * (FONT_HEIGHT + STRING_GAP), 1, BLACK);
		printToTexture(">", texture, 0, FONT_HEIGHT + STRING_GAP, 1, BLACK);
		//Reprint
		printToTexture("^", texture, 12 * (FONT_WIDTH + STRING_GAP), 
			1 * (FONT_HEIGHT + STRING_GAP), 1, WHITE);
		printToTexture(">", texture, 0, 0, 1, WHITE);

	}
	else if (mode == OPTIONS + 1)
	{

		//Erase "^" and ">" by printing it in black
		printToTexture("^", texture, 12 * (FONT_WIDTH + STRING_GAP), 
			1 * (FONT_HEIGHT + STRING_GAP), 1, BLACK);
		printToTexture(">", texture, 0, FONT_HEIGHT + STRING_GAP, 1, BLACK);
		//Reprint
		printToTexture("^", texture, 14 * (FONT_WIDTH + STRING_GAP), 
			1 * (FONT_HEIGHT + STRING_GAP), 1, WHITE);
		printToTexture(">", texture, 0, 0, 1, WHITE);

	}
	else if (mode == OPTIONS + 2)
	{

		//Erase "^" and ">"
		printToTexture("^", texture, 12 * (FONT_WIDTH + STRING_GAP), 
			1 * (FONT_HEIGHT + STRING_GAP), 1, BLACK);
		printToTexture("^", texture, 14 * (FONT_WIDTH + STRING_GAP), 
			1 * (FONT_HEIGHT + STRING_GAP), 1, BLACK);
		printToTexture(">", texture, 0, 0, 1, BLACK);
		printToTexture(">", texture, 0, 2 * (FONT_WIDTH + STRING_GAP), 1, BLACK);

		//Print the current GHOST option
		if (getOption(1) == 0)
		{

			//Erase ON
			printToTexture("ON", texture, 8 * (FONT_WIDTH + STRING_GAP), 
				FONT_HEIGHT + STRING_GAP, 1, BLACK);
			//Print OFF
			printToTexture("OFF", texture, 8 * (FONT_WIDTH + STRING_GAP), 
				FONT_HEIGHT + STRING_GAP, 1, RED);

		}
		else if (getOption(1) == 1)
		{

			//Erase OFF
			printToTexture("OFF", texture, 8 * (FONT_WIDTH + STRING_GAP), 
				FONT_HEIGHT + STRING_GAP, 1, BLACK);
			//Print ON
			printToTexture("ON", texture, 8 * (FONT_WIDTH + STRING_GAP), 
				FONT_HEIGHT + STRING_GAP, 1, GREEN);

		}

		printToTexture(">", texture, 0, FONT_HEIGHT + STRING_GAP, 1, WHITE);

	}
	else if (mode == OPTIONS + 3)
	{

		//Erasures
		printToTexture(">", texture, 0, FONT_HEIGHT + STRING_GAP, 1, BLACK);
		printToTexture(">", texture, 0, 3 * (FONT_HEIGHT + STRING_GAP), 1, BLACK);

		//Print the current FULLSCREEN option
		if (getOption(2) == 0)
		{

			//Erase ON
			printToTexture("ON", texture, 13 * (FONT_WIDTH + STRING_GAP), 
				2 * (FONT_HEIGHT + STRING_GAP), 1, BLACK);
			//Print OFF
			printToTexture("OFF", texture, 13 * (FONT_WIDTH + STRING_GAP), 
				2 * (FONT_HEIGHT + STRING_GAP), 1, RED);

		}
		else if (getOption(2) == 1)
		{

			//Erase OFF
			printToTexture("OFF", texture, 13 * (FONT_WIDTH + STRING_GAP), 
				2 * (FONT_HEIGHT + STRING_GAP), 1, BLACK);
			//Print ON
			printToTexture("ON", texture, 13 * (FONT_HEIGHT + STRING_GAP), 
				2 * (FONT_HEIGHT + STRING_GAP), 1, GREEN);

		}

		printToTexture(">", texture, 0, 2 * (FONT_HEIGHT + STRING_GAP), 1, WHITE);

	}
	else if (mode == OPTIONS + 4)
	{

		//Erasures
		printToTexture(">", texture, 0, 2 * (FONT_HEIGHT + STRING_GAP), 1, BLACK);
		intToTexture(getOption(3) + 1, texture, 
			9 * (FONT_WIDTH + STRING_GAP), 3 * (FONT_HEIGHT + STRING_GAP), 1, 
			BLACK);
		intToTexture(getOption(3) - 1, texture, 
			9 * (FONT_WIDTH + STRING_GAP), 3 * (FONT_HEIGHT + STRING_GAP), 1, 
			BLACK);
		printToTexture(">", texture, 0, 4 * (FONT_WIDTH + STRING_GAP), 1, BLACK);

		//Print current volume
		intToTexture((int)(getOption(3)), texture, 
			9 * (FONT_WIDTH + STRING_GAP), 3 * (FONT_HEIGHT + STRING_GAP), 1, 
			YELLOW);

		printToTexture(">", texture, 0, 3 * (FONT_WIDTH + STRING_GAP), 1, WHITE);

	}
	else if (mode == OPTIONS + 5)
	{

		//Erasures
		printToTexture(">", texture, 0, 3 * (FONT_HEIGHT + STRING_GAP), 1, 
			BLACK);

		printToTexture(">", texture, 0, 4 * (FONT_HEIGHT + STRING_GAP), 1, 
			WHITE);

	}

}

void updateModes(SDL_Texture* texture, unsigned short currentMode)
{

	//Clear modes texture
	clearTexture(texture);

	//Redraw the modes texture, changing it depending on what the current mode is
	printToTexture(">", texture, 0, 2* (FONT_HEIGHT + STRING_GAP), 
		1.0, WHITE);

	if (currentMode == 0)
	{

		printToTexture("MULTRIS", texture, FONT_WIDTH + STRING_GAP, 
			2 * (FONT_HEIGHT + STRING_GAP), 1, WHITE);
		printToTexture("NUMERICAL", texture, FONT_WIDTH + STRING_GAP, 
			3 * (FONT_HEIGHT + STRING_GAP), 1, WHITE);
		printToTexture("OPTIONS", texture, FONT_WIDTH + STRING_GAP, 
			4 * (FONT_HEIGHT + STRING_GAP), 1, WHITE);

	}
	else if (currentMode > 0 && currentMode <= MAX_PIECE_SIZE)
	{

		printToTexture("MULTRIS", texture, FONT_WIDTH + STRING_GAP, 
			FONT_HEIGHT + STRING_GAP, 1, WHITE);
		printToTexture("NUMERICAL", texture, FONT_WIDTH + STRING_GAP, 
			2 * (FONT_HEIGHT + STRING_GAP), 1, WHITE);
		printToTexture("OPTIONS", texture, FONT_WIDTH + STRING_GAP, 
			3 * (FONT_HEIGHT + STRING_GAP), 1, WHITE);
		printToTexture("^", texture, 15 * FONT_WIDTH + 2 * STRING_GAP, 
			4 * FONT_HEIGHT, 1, WHITE);

		if (currentMode == 1)
			printToTexture("  1 2", texture, 13 * FONT_WIDTH, 
							2 * (FONT_HEIGHT + STRING_GAP), 1, WHITE);
		else if (currentMode < MAX_PIECE_SIZE)
		{

			char string[6] = { '0' + currentMode - 1, ' ', '0' + currentMode, ' ', 
								'0' + currentMode + 1, '\0' };
			printToTexture(string, texture, 13 * FONT_WIDTH, 
				2 * (FONT_HEIGHT + STRING_GAP), 1, WHITE);

		}
		else
			printToTexture("7 8  ", texture, 13 * FONT_WIDTH, 
				2 * (FONT_HEIGHT + STRING_GAP), 1, WHITE);

	}
	else if (currentMode >= OPTIONS)
	{

		printToTexture("MULTRIS", texture, FONT_WIDTH + STRING_GAP, 0, 1, WHITE);
		printToTexture("NUMERICAL", texture, FONT_WIDTH + STRING_GAP, 
			1 * (FONT_HEIGHT + STRING_GAP), 1, WHITE);
		printToTexture("OPTIONS", texture, FONT_WIDTH + STRING_GAP, 
			2 * (FONT_HEIGHT + STRING_GAP), 1, WHITE);

	}

}

//This function is for when the title has hit the floor, but a few pieces of it should still continue falling
bool updateTitle(SDL_Texture* texture, piece** movingPieces)
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

	for (unsigned i = 0; i < 5; i++)
	{

		//Save the color for the current piece, and then set the color to black
		unsigned short color = movingPieces[i]->color;
		movingPieces[i]->color = BLACK;

		if ((Y + i) != NULL)
		{

			//We set the color to black so that we can redraw the same piece on top of itself, effectively
			//erasing it since it is being drawn over with a BLACK copy of itself
				//We only need to do this if the piece is about to drop down 1 cell during this frame
					//That is what we are checking for with this if statement
			if ((int)(*(Y + i) + INITIAL_SPEED * globalInstance->frame_time) > 
				(int)(*(Y + i)))
				drawPiece(*movingPieces[i], texture, SPRITE_WIDTH * X[i],
					SPRITE_HEIGHT * (int)*(Y + i));

			//Restore the color of the piece
			movingPieces[i]->color = color;

			//Drop the piece
			if (*(Y + i) <= endY[i])
				*(Y + i) += INITIAL_SPEED * globalInstance->frame_time;

			//Now redraw the piece
				//Again, we only redraw it if it actually changed cells over this frame
			if ((int)(*(Y + i) - INITIAL_SPEED * globalInstance->frame_time) < 
				(int)(*(Y + i)))
				drawPiece(*movingPieces[i], texture, SPRITE_WIDTH * X[i], 
					SPRITE_HEIGHT * (int)*(Y + i));

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

	return updateOver;

}

piece** getMovingPieces(piece** titlePieces)
{

	piece** movingPieces = malloc(5 * sizeof(**movingPieces));
	
	if (movingPieces != NULL)
	{
		
		//Allocate memory for the block array of each moving piece
			//titlepieces[1], [9], [13], [14], [16] are the pieces that must continue falling even after the rest
			//of the title has finished
		for (unsigned short i = 0; i < 5; i++)
			movingPieces[i] = malloc(sizeof(*movingPieces[i]));
		movingPieces[0]->blocks = malloc(titlePieces[1]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[1]->blocks = malloc(titlePieces[9]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[2]->blocks = malloc(titlePieces[13]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[3]->blocks = malloc(titlePieces[14]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[4]->blocks = malloc(titlePieces[16]->numOfBlocks * sizeof(*movingPieces[0]->blocks));

		//Copy the pieces over for each movingPiece
		copyPiece(titlePieces[1], movingPieces[0]);
		copyPiece(titlePieces[9], movingPieces[1]);
		copyPiece(titlePieces[13], movingPieces[2]);
		copyPiece(titlePieces[14], movingPieces[3]);
		copyPiece(titlePieces[16], movingPieces[4]);
		
	}

	return movingPieces;

}