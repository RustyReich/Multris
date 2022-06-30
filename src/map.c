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
void _playSound(int id);
sound* loadSound(char* file);
void delSound(sound** Sound);
void setVolume(sound** Sound, unsigned short volume);

//file.c
bool fileExists(char* fileName);
void saveOption(const char* str, int value);
int getOptionValue(const char* str);

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
void updateValuesText(SDL_Texture* texture);

unsigned short drawTitle(piece** firstPiece)
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
	static bool* editingVolume; declare(editingVolume, false);
	static Uint32* moveStart; declare(moveStart, 0);
	static bool* moveStartBool; declare(moveStartBool, false);

	//Textures
	static SDL_Texture* Texture_Next; declare_Piece_Text(&Texture_Next, *firstPiece);
	static SDL_Texture* Texture_Score; declare_HUD_Text(&Texture_Score, SCORE_TEXT);
	static SDL_Texture* Texture_Level; declare_HUD_Text(&Texture_Level, LEVEL_TEXT);
	static SDL_Texture* Texture_Lines; declare_HUD_Text(&Texture_Lines, LINES_TEXT);
	static SDL_Texture* Texture_Title; declare_HUD_Text(&Texture_Title, TITLE_TEXT);
	static SDL_Texture* Texture_Cursor; declare_HUD_Text(&Texture_Cursor, CURSOR_TEXT);
	static SDL_Texture* Texture_volSlide; declare_HUD_Text(&Texture_volSlide, VOLSLIDE_TEXT);
	static SDL_Texture* Texture_Values; declare_HUD_Text(&Texture_Values, VALUES_TEXT);

	//Arrays
	static piece** movingPieces; declare_moving_title_pieces(&movingPieces);

	//UI elements
	static UI_list* modes; declare_UI_list(&modes, MODES_LIST);
	static UI_list* numerical; declare_UI_list(&numerical, NUMERICAL_LIST);
	static UI_list* options; declare_UI_list(&options, OPTIONS_LIST);

	//Some stuff to do on the firstLoop
	if (*firstLoop == true)
	{

		//Get the width and height for the nextPiece texture
		SDL_QueryTexture(Texture_Next, NULL, NULL, nextText_Width, nextText_Height);
		//Get the height of the title texture
		SDL_QueryTexture(Texture_Title, NULL, NULL, NULL, titleText_Height);	
		
		*firstLoop = false;

	}

	// Control Logic -------------------------------------------------------------

	const char* selected_mode = getListSelectedString(modes);
	const char* selected_option = getListSelectedString(options);

	if (onPress(DOWN_BUTTON))
	{

		if(modes->ui->currentlyInteracting)
		{

			if (modes->selected_entry < modes->num_entries - 1)
			{

				playSound(MOVE_SOUND);
				modes->selected_entry++;

			}

		}
		else if(numerical->ui->currentlyInteracting)
		{

			if (numerical->selected_entry < numerical->num_entries - 1)
			{

				playSound(MOVE_SOUND);
				numerical->selected_entry++;

			}

		}
		else if(options->ui->currentlyInteracting && *editingVolume == false)
		{

			if (options->selected_entry < options->num_entries - 1)
			{

				playSound(MOVE_SOUND);
				options->selected_entry++;

			}

		}

	}

	if (onPress(UP_BUTTON))
	{

		if (modes->ui->currentlyInteracting)
		{

			if(modes->selected_entry > 0)
			{

				playSound(MOVE_SOUND);
				modes->selected_entry--;

			}

		}
		else if (numerical->ui->currentlyInteracting)
		{

			if(numerical->selected_entry > 0)
			{

				playSound(MOVE_SOUND);
				numerical->selected_entry--;

			}

		}
		else if (options->ui->currentlyInteracting && *editingVolume == false)
		{

			if(options->selected_entry > 0)
			{

				playSound(MOVE_SOUND);
				options->selected_entry--;

			}

		}

	}

	if (onHold(LEFT_BUTTON) || onHold(RIGHT_BUTTON))
	{

		Uint32 currTicks = SDL_GetTicks();

		//Start the counter for holding the LEFT or RIGHT buttons
		if (*moveStart == 0)
		{

			*moveStart = currTicks;
			*moveStartBool = true;

		}

		if (*moveStartBool || (currTicks - *moveStart) >= (MOVEMENT_WAIT + MOVEMENT_TIME))
		{

			if (onHold(LEFT_BUTTON))
			{

				if (*editingVolume && VOLUME > 0)
				{

					VOLUME--;
					updateValuesText(Texture_Values);

					updateVolume();
					playSound(MOVE_SOUND);

				}

			}
			
			if (onHold(RIGHT_BUTTON))
			{

				if (*editingVolume && VOLUME < 100)
				{

					VOLUME++;
					updateValuesText(Texture_Values);

					updateVolume();
					playSound(MOVE_SOUND);

				}

			}

			*moveStartBool = false;

			if ((currTicks - *moveStart) >= (MOVEMENT_WAIT + MOVEMENT_TIME))
				*moveStart = currTicks - MOVEMENT_WAIT;

		}

	}
	else
		*moveStart = 0;

	if (onPress(SELECT_BUTTON))
	{

		if (*editingVolume == false)
			playSound(ROTATE_SOUND);

		if (modes->ui->currentlyInteracting)
		{

			if (SDL_strcmp(selected_mode, "MULTRIS") == 0)
			{

				//Enter MULTRIS gamemode
				freeVars();
				return PLAY_SCREEN;

			}
			else if (SDL_strcmp(selected_mode, "NUMERICAL") == 0)
			{

				modes->ui->currentlyInteracting = false;
				numerical->ui->currentlyInteracting = true;

			}
			else if (SDL_strcmp(selected_mode, "OPTIONS") == 0)
			{

				modes->ui->currentlyInteracting = false;
				options->ui->currentlyInteracting = true;

			}
			else if (SDL_strcmp(selected_mode, "EXIT") == 0)
				globalInstance->running = false;

		}
		else if (numerical->ui->currentlyInteracting)
		{

			//Enter NUMERICAL gamemode with selected size
			MODE = SDL_atoi(getListSelectedString(numerical));
			freeVars();
			return PLAY_SCREEN;

		}
		else if (options->ui->currentlyInteracting)
		{

			if (SDL_strcmp(selected_option, "LIMIT FPS") == 0)
			{

				LIMIT_FPS = !LIMIT_FPS;

				updateValuesText(Texture_Values);

			}
			else if (SDL_strcmp(selected_option, "FULLSCREEN") == 0)
			{

				FULLSCREEN_MODE = !FULLSCREEN_MODE;
				UPDATE_FULLSCREEN_MODE = true;

				updateValuesText(Texture_Values);

			}
			else if (SDL_strcmp(selected_option, "VOLUME") == 0)
				*editingVolume = true;
			else if (SDL_strcmp(selected_option, "CONTROLS") == 0)
			{

				//Go to the controls screen
				freeVars();
				return CONTROLS_SCREEN;

			}

		}

	}

	//Exit out of the current sub-menu if the EXIT_BUTTON is pressed
	if (onPress(EXIT_BUTTON))
	{

		//We're not exiting out of any menu if were in the modes menu when we press EXIT.
			//So only play this sound if we're not in the modes menu
		if (modes->ui->currentlyInteracting == false)
			playSound(LAND_SOUND);

		if (numerical->ui->currentlyInteracting)
		{

			numerical->ui->currentlyInteracting = false;
			modes->ui->currentlyInteracting = true;

		}
		else if (options->ui->currentlyInteracting)
		{

			if (*editingVolume == true)
				*editingVolume = false;
			else
			{

				options->ui->currentlyInteracting = false;
				modes->ui->currentlyInteracting = true;

				//Save option values to optionsFile when player leaves options menu
				saveOption("FULLSCREEN", FULLSCREEN_MODE);
				saveOption("VOLUME", VOLUME);
				saveOption("LIMIT FPS", LIMIT_FPS);

			}

		}

	}

	// ---------------------------------------------------------------------------

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

	//Rendering -------------------------------------------

	drawTexture(Texture_Title, FONT_WIDTH, FONT_HEIGHT * (int)*Y, 1.0);
	drawTexture(Texture_Next, 318 - (*nextText_Width / 2), 282 - (*nextText_Height / 2), 1.0);
	drawTexture(Texture_Score, 277, 70, 1.0);
	drawTexture(Texture_Level, 312, 115, 1.0);
	drawTexture(Texture_Lines, 312, 189, 1.0);

	//Draw UI elements

	//Modes
	drawTexture(modes->ui->texture, modes->ui->x, modes->ui->y, 1.0);
	drawTexture(Texture_Cursor, modes->ui->x - 14, getListSelectedEntryY(modes), 1.0);

	//Only draw currently active sub-menu
	if (numerical->ui->currentlyInteracting)
	{

		drawTexture(numerical->ui->texture, numerical->ui->x, numerical->ui->y, 1.0);
		drawTexture(Texture_Cursor, numerical->ui->x - 14, getListSelectedEntryY(numerical), 1);

	}
	else if (options->ui->currentlyInteracting)
	{

		//Draw options menu
		drawTexture(options->ui->texture, options->ui->x, options->ui->y, 1.0);
		drawTexture(Texture_Cursor, options->ui->x - 14, getListSelectedEntryY(options), 1);		
	
		//Draw options values
		int optionsWidth;
		SDL_QueryTexture(options->ui->texture, NULL, NULL, &optionsWidth, NULL);
		drawTexture(Texture_Values, options->ui->x + optionsWidth, options->ui->y, 1.0);

		//If editing the volume, draw cursors to left and right of volume
		if (*editingVolume)
			drawTexture(Texture_volSlide, options->ui->x + optionsWidth - FONT_WIDTH, 
						getListSelectedEntryY(options), 1.0);

	}

	//-----------------------------------------------------

	return TITLE_SCREEN;

}