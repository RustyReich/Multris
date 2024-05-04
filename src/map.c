#include "MGF.h"

unsigned short drawTitle(piece** firstPiece)
{

	//Variables
	DECLARE_VARIABLE(bool, titleUpdating, false);
	DECLARE_VARIABLE(unsigned short, mode, 0);
	DECLARE_VARIABLE(double, Y, (double)15);
	DECLARE_VARIABLE(char, inputLock, true);
	DECLARE_VARIABLE(int, nextText_Width, 0);
	DECLARE_VARIABLE(int, nextText_Height, 0);
	DECLARE_VARIABLE(int, titleText_Height, 0);
	DECLARE_VARIABLE(bool, firstLoop, true);
	DECLARE_VARIABLE(bool, editingVolume, false);
	DECLARE_VARIABLE(unsigned int, moveStart, 0);
	DECLARE_VARIABLE(bool, moveStartBool, false);
	DECLARE_VARIABLE(unsigned short, customModeSize, 1);

	//Textures
	static SDL_Texture* Texture_Next; declare_Piece_Text(&Texture_Next, *firstPiece, false);
	static SDL_Texture* Texture_Score; declare_HUD_Text(&Texture_Score, SCORE_TEXT);
	static SDL_Texture* Texture_Level; declare_HUD_Text(&Texture_Level, LEVEL_TEXT);
	static SDL_Texture* Texture_Lines; declare_HUD_Text(&Texture_Lines, LINES_TEXT);
	static SDL_Texture* Texture_Title; declare_HUD_Text(&Texture_Title, TITLE_TEXT);
	static SDL_Texture* Texture_Cursor; declare_HUD_Text(&Texture_Cursor, CURSOR_TEXT);
	static SDL_Texture* Texture_volSlide; declare_HUD_Text(&Texture_volSlide, VOLSLIDE_TEXT);
	static SDL_Texture* Texture_Values; declare_HUD_Text(&Texture_Values, VALUES_TEXT);
	static SDL_Texture* Texture_Custom; declare_HUD_Text(&Texture_Custom, CUSTOM_TEXT);

	//Arrays
	static piece** movingPieces; declare_moving_title_pieces(&movingPieces);

	//UI elements
	static UI_list* modes; declare_UI_list(&modes, MODES_LIST);
	static UI_list* numerical; declare_UI_list(&numerical, NUMERICAL_LIST);
	static UI_list* custom; declare_UI_list(&custom, CUSTOM_LIST);
	static UI_list* options; declare_UI_list(&options, OPTIONS_LIST);

	//Some stuff to do on the firstLoop
	if (*firstLoop == true)
	{

		//Get the width and height for the nextPiece texture
		SDL_QueryTexture(Texture_Next, NULL, NULL, nextText_Width, nextText_Height);
		//Get the height of the title texture
		SDL_QueryTexture(Texture_Title, NULL, NULL, NULL, titleText_Height);	

		//Start the title at a Y value that is just below whatever the lowest-reaching menu is
		int modesHeight, numericalHeight, customHeight, optionsHeight;
		SDL_QueryTexture(modes->ui->texture, NULL, NULL, NULL, &modesHeight);
		SDL_QueryTexture(numerical->ui->texture, NULL, NULL, NULL, &numericalHeight);
		SDL_QueryTexture(custom->ui->texture, NULL, NULL, NULL, &customHeight);
		SDL_QueryTexture(options->ui->texture, NULL, NULL, NULL, &optionsHeight);
		//This section basically finds the bottom Y value of the lowest-reaching menu
		*Y = SDL_ceil(SDL_max(modes->ui->y + modesHeight, numerical->ui->y + numericalHeight));
		*Y = SDL_ceil(SDL_max(*Y, custom->ui->y + customHeight));
		*Y = SDL_ceil(SDL_max(*Y, options->ui->y + optionsHeight));
		*Y = *Y / (double)(SPRITE_HEIGHT) + 1;
		
		//Default MODE is 0
		MODE = 0;

		CUSTOM_MODE = false;

		*firstLoop = false;

	}

	// Control Logic -------------------------------------------------------------

	//Keep track of the current selected mode and option
	const char* selected_mode = getListSelectedString(modes);
	const char* selected_option = getListSelectedString(options);

	//If you press DOWN
	if (onPress(DOWN_BUTTON))
	{

		//Move down only in the menu that is currently being interacted with
		if(modes->ui->currentlyInteracting)
		{

			//Do some bounds checking to make sure you dont move down off the menu
			if (modes->selected_entry < modes->num_entries - 1)
			{

				playSound(MOVE_SOUND);
				modes->selected_entry++;

			}

		}
		else if(numerical->ui->currentlyInteracting)
		{

			if (numerical->selected_entry < PROGRESS - 1)
			{

				playSound(MOVE_SOUND);
				numerical->selected_entry++;

			}
			else	//Play LAND_SOUND if player trying to select a size they dont have unlocked
				playSound(LAND_SOUND);

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

	//If you press UP
		//Basically same logic as pressing DOWN
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

	//Holding LEFT or RIGHT is only used for changing the volume
	if (onHold(LEFT_BUTTON) || onHold(RIGHT_BUTTON))
	{

		Uint32 currTicks = SDL_GetTicks();

		//Start the counter for holding the LEFT or RIGHT buttons
		if (*moveStart == 0)
		{

			*moveStart = currTicks;
			*moveStartBool = true;

		}

		//Logic for volume or custom value changing rapidly if you hold the button
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
				else if (custom->ui->currentlyInteracting && *customModeSize > 1)
				{

					*customModeSize -= 1;
					updateCustomText(Texture_Custom, *customModeSize);

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
				else if (custom->ui->currentlyInteracting && *customModeSize < MAX_CUSTOM_SIZE)
				{

					*customModeSize += 1;
					updateCustomText(Texture_Custom, *customModeSize);

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

	//When you press the SELECT_BUTTON
	if (onPress(SELECT_BUTTON))
	{

		//Play ROTATE_SOUND only if pressed when not editing volume
		if (*editingVolume == false)
			playSound(ROTATE_SOUND);

		//If pressed SELECT when in the modes menu
		if (modes->ui->currentlyInteracting)
		{

			if (SDL_strcmp(selected_mode, "MULTRIS") == 0)
			{

				//Enter MULTRIS gamemode
				MODE = 0;
				freeVars();
				return PLAY_SCREEN;

			}
			else if (SDL_strcmp(selected_mode, "NUMERICAL") == 0)
			{

				//Go into NUMERICAL menu
				modes->ui->currentlyInteracting = false;
				numerical->ui->currentlyInteracting = true;

			}
			else if (SDL_strcmp(selected_mode, "CUSTOM") == 0)
			{

				// Go into CUSTOM menu
				modes->ui->currentlyInteracting = false;
				custom->ui->currentlyInteracting = true;

			}
			else if (SDL_strcmp(selected_mode, "OPTIONS") == 0)
			{

				//Go into OPTIONS menu
				modes->ui->currentlyInteracting = false;
				options->ui->currentlyInteracting = true;

			}
			else if (SDL_strcmp(selected_mode, "EXIT") == 0)
				//Exit the game
				globalInstance->running = false;

		}
		//If pressed SELECT when in the NUMERICAL menu
		else if (numerical->ui->currentlyInteracting)
		{

			//Enter NUMERICAL gamemode with selected size
			MODE = SDL_atoi(getListSelectedString(numerical));
			freeVars();
			return PLAY_SCREEN;

		}
		else if (custom->ui->currentlyInteracting)
		{

			CUSTOM_MODE = true;
			MODE = *customModeSize;
			freeVars();
			return PLAY_SCREEN;

		}
		//If pressed SELECT when in the OPTIONS menu
		else if (options->ui->currentlyInteracting)
		{

			//Logic for what option you pressed SELECT on
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
			else if (SDL_strcmp(selected_option, "SHOW FPS") == 0)
			{

				SHOW_FPS = !SHOW_FPS;

				updateValuesText(Texture_Values);

			}
			else if (SDL_strcmp(selected_option, "CENTER DOT") == 0)
			{

				CENTER_DOT = !CENTER_DOT;

				updateValuesText(Texture_Values);

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
		else if (custom->ui->currentlyInteracting)
		{

			custom->ui->currentlyInteracting = false;
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
				saveToFile("SAVES/options.cfg", "FULLSCREEN", FULLSCREEN_MODE);
				saveToFile("SAVES/options.cfg", "VOLUME", VOLUME);
				saveToFile("SAVES/options.cfg", "LIMIT FPS", LIMIT_FPS);
				saveToFile("SAVES/options.cfg", "SHOW FPS", SHOW_FPS);
				saveToFile("SAVES/options.cfg", "CENTER DOT", CENTER_DOT);

			}

		}

	}

	// ---------------------------------------------------------------------------

	//Title dropping
	if ((int)(*Y + *titleText_Height / SPRITE_HEIGHT) <= BASE_PLAYFIELD_HEIGHT * MAX_PIECE_SIZE)
	{

		//Don't start dropping the title until the game has been running for at least 1000ms
			//This is because FPS and frame_times can be erratic the first few frames after launch
		if (SDL_GetTicks() > 1000)
			*Y += INITIAL_SPEED * globalInstance->frame_time;

	}
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
	drawTexture(Texture_Score, getScoreDrawX(MODE), getScoreDrawY(MODE), 1.0);
	drawTexture(Texture_Level, 312, 115, 1.0);
	drawTexture(Texture_Lines, 312, 189, 1.0);

	//Draw UI elements

	//Modes
	drawTexture(modes->ui->texture, modes->ui->x, modes->ui->y, 1.0);
	drawTexture(Texture_Cursor, modes->ui->x - 14, getListSelectedEntryY(modes), 1.0);

	//Only draw currently active sub-menu
		//MODES menu is always active
	if (numerical->ui->currentlyInteracting)
	{

		drawTexture(numerical->ui->texture, numerical->ui->x, numerical->ui->y, 1.0);
		drawTexture(Texture_Cursor, numerical->ui->x - 14, getListSelectedEntryY(numerical), 1);

	}
	else if (custom->ui->currentlyInteracting)
	{

		// Draw CUSTOM menu texture
		drawTexture(custom->ui->texture, custom->ui->x, custom->ui->y, 1.0);

		// Draw CUSTOM value
		int customValueX = custom->ui->x + (int)(2.5 * (double)FONT_WIDTH) + 2 * STRING_GAP - getIntStringLength(*customModeSize, 1.0) / 2;
		drawTexture(Texture_Custom, customValueX, custom->ui->y, 1.0);

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
			drawTexture(Texture_volSlide, options->ui->x + optionsWidth - FONT_WIDTH, getListSelectedEntryY(options), 1.0);

	}

	//-----------------------------------------------------

	return TITLE_SCREEN;

}