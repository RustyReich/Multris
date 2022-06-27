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

//memory.c
void freeVars();
void declare_unsigned_short(void** ptr, unsigned short value);
void declare_double(void** ptr, double value);
void declare_int(void** ptr, int value);
void declare_char(void** ptr, char value);
void declare_bool(void** ptr, bool value);
void declare_unsigned_int(void** ptr, unsigned int value);
void declare_Piece(piece** ptr, piece* Piece);
void declare_Piece_Text(SDL_Texture** ptr, piece* Piece);
void declare_map_matrix(bool** ptr);
void declare_HUD_Text(SDL_Texture** ptr, int type);
void declare_moving_title_pieces(piece*** ptr);
void declare_UI_list(UI_list** ptr, int type);

//layout.c
bool updateTitle(SDL_Texture* texture, piece** movingPieces);
int getListSelectedEntryY(UI_list* list);
const char* getListSelectedString(UI_list* list);

void updateModes(SDL_Texture* texture, unsigned short currentMode);
void updateOptions(SDL_Texture* texture, unsigned short mode);

unsigned short drawTitle(piece** firstPiece, unsigned short* returnMode)
{

	//Variables
	static bool* titleUpdating; declare(titleUpdating, false);
	static unsigned short* mode; declare(mode , 0);
	static double* Y; declare(Y, (double)15);
	static char* inputLock; declare(inputLock, true);
	static int* nextText_Width; declare(nextText_Width, 0);
	static int* nextText_Height; declare(nextText_Height, 0);
	static int* titleText_Height; declare(titleText_Height, 0);
	static bool* firstLoop; declare(firstLoop, true);

	//Textures
	static SDL_Texture* Texture_Next; declare_Piece_Text(&Texture_Next, *firstPiece);
	static SDL_Texture* Texture_Score; declare_HUD_Text(&Texture_Score, SCORE_TEXT);
	static SDL_Texture* Texture_Level; declare_HUD_Text(&Texture_Level, LEVEL_TEXT);
	static SDL_Texture* Texture_Lines; declare_HUD_Text(&Texture_Lines, LINES_TEXT);
	static SDL_Texture* Texture_Title; declare_HUD_Text(&Texture_Title, TITLE_TEXT);
	static SDL_Texture* Texture_Cursor; declare_HUD_Text(&Texture_Cursor, CURSOR_TEXT);

	//Arrays
	static piece** movingPieces; declare_moving_title_pieces(&movingPieces);

	//UI elements
	static UI_list* modes; declare_UI_list(&modes, MODES_LIST);

	//Some stuff to do on the firstLoop
	if (*firstLoop == true)
	{

		//Get the width and height for the nextPiece texture
		SDL_QueryTexture(Texture_Next, NULL, NULL, nextText_Width, nextText_Height);
		//Get the height of the title texture
		SDL_QueryTexture(Texture_Title, NULL, NULL, NULL, titleText_Height);	
		
		*firstLoop = false;

	}
/*
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
*/
	//Rendering -------------------------------------------

	drawTexture(Texture_Title, FONT_WIDTH, FONT_HEIGHT * (int)*Y, 1.0);
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

	//Draw UI elements
	drawTexture(modes->ui->texture, modes->ui->x, modes->ui->y, 1.0);
	drawTexture(Texture_Cursor, modes->ui->x - 14, getListSelectedEntryY(modes), 1.0);
/*
	if (*mode >= OPTIONS)
		drawTexture(Texture_Options, 2 * (FONT_WIDTH + STRING_GAP), 
					4 * (FONT_WIDTH + STRING_GAP), 1.0);
*/
	//-----------------------------------------------------

	//Title dropping
	if ((int)(*Y + *titleText_Height / SPRITE_HEIGHT) <= BASE_PLAYFIELD_HEIGHT * MAX_PIECE_SIZE)
		*Y += INITIAL_SPEED * globalInstance->frame_time;
	else
	{

		//Once the title is done dropping, there are a some pieces in the title that should 
		//continue dropping
		//These pieces are called movingPieces
			//We call updateTitle until all movingPieces finished dropping as well
		if (*titleUpdating == false)
			*titleUpdating = updateTitle(Texture_Title, movingPieces);

	}

	// Control Logic -------------------------------------------------------------

	if (onPress(DOWN_BUTTON))
	{

		if(modes->ui->currentlyInteracting)
			if (modes->selected_entry < modes->num_entries - 1)
				modes->selected_entry++;

	}

	if (onPress(UP_BUTTON))
	{

		if (modes->ui->currentlyInteracting)
			if(modes->selected_entry > 0)
				modes->selected_entry--;

	}

	if (onPress(SELECT_BUTTON))
	{

		if (modes->ui->currentlyInteracting)
		{

			const char* str = getListSelectedString(modes);

			if (strcmp(str, "MULTRIS") == 0 || strcmp(str, "NUMERICAL") == 0)
			{

				freeVars();
				return PLAY_SCREEN;

			}	

		}
			

	}

	// ---------------------------------------------------------------------------

/*
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

			freeVars();				

			//Delete movingPieces if resetting
			if (*returnMode < OPTIONS + 2)
			{
			
				//free movingPieces
			//	for (unsigned short i = 0; i < 5; i++)
			//		delPiece(&movingPieces[i]);
			//	free(movingPieces);
			//	movingPieces = NULL;

			}

			//Free Textures
			//SDL_DestroyTexture(Texture_Modes);
			//Texture_Modes = NULL;
			SDL_DestroyTexture(Texture_Options);
			Texture_Options = NULL;

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
			//updateModes(Texture_Modes, *mode);

		}
		else if (*mode >= 1 && *mode <= 8)
		{

			playSound(globalInstance->sounds[MOVE_SOUND_ID]);
			*mode = OPTIONS;
			//updateModes(Texture_Modes, *mode);
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
			//updateModes(Texture_Modes, *mode);

		}
		else if (*mode == OPTIONS || *mode == OPTIONS + 1)
		{

			playSound(globalInstance->sounds[MOVE_SOUND_ID]);
			*mode = 1;
			//updateModes(Texture_Modes, *mode);

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
			//updateModes(Texture_Modes, *mode);

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
			//updateModes(Texture_Modes, *mode);

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
*/
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