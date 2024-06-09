#include "MGF.h"

unsigned short playMode(piece* firstPiece)
{

	//Game pieces
	static piece* currentPiece; declare_Piece(&currentPiece, firstPiece);
	static piece* opponentCurrentPiece; declare_Piece(&opponentCurrentPiece, NULL);
	static piece* holdPiece; declare_Piece(&holdPiece, NULL);
	static piece* nextPiece; declare_Piece(&nextPiece, NULL);
	static piece* opponentNextPiece; declare_Piece(&opponentNextPiece, NULL);
	
	//Variables
	DECLARE_VARIABLE(signed short, X, 0);
	DECLARE_VARIABLE(signed short, opponentX, 0);
	DECLARE_VARIABLE(double, Y, 0.0);
	DECLARE_VARIABLE(int, lastYInt, 0);
	DECLARE_VARIABLE(int, lastXInt, 0);
	DECLARE_VARIABLE(signed short, opponentY, 0);
	DECLARE_VARIABLE(double, speed, INITIAL_SPEED);
	DECLARE_VARIABLE(bool, softDrop, false);
	DECLARE_VARIABLE(bool, gameOver, false);
	DECLARE_VARIABLE(bool, clearingLine, false);
	DECLARE_VARIABLE(bool, paused, false);
	DECLARE_VARIABLE(bool, overAnimation, false);
	DECLARE_VARIABLE(bool, justHeld, false);
	DECLARE_VARIABLE(unsigned short, numCompleted, 0);
	DECLARE_VARIABLE(unsigned int, Score, 0);
	DECLARE_VARIABLE(unsigned int, opponentScore, 0);
	DECLARE_VARIABLE(short, Level, 0);
	DECLARE_VARIABLE(short, opponentLevel, 0);
	DECLARE_VARIABLE(short, linesAtCurrentLevel, 0);
	DECLARE_VARIABLE(short, ghostY, MAP_HEIGHT - currentPiece->height);
	DECLARE_VARIABLE(unsigned int, moveStart, 0);
	DECLARE_VARIABLE(bool, moveStartBool, false);
	DECLARE_VARIABLE(int, nextText_Width, 0);
	DECLARE_VARIABLE(int, nextText_Height, 0);
	DECLARE_VARIABLE(int, opponentNextText_Width, 0);
	DECLARE_VARIABLE(int, opponentNextText_Height, 0);
	DECLARE_VARIABLE(int, holdText_Width, 0);
	DECLARE_VARIABLE(int, holdText_Height, 0);
	DECLARE_VARIABLE(bool, firstLoop, true);
	DECLARE_VARIABLE(int, foregroundX, 0);
	DECLARE_VARIABLE(int, foregroundY, 0);
	DECLARE_VARIABLE(int, opponentForegroundX, 0);
	DECLARE_VARIABLE(double, pausedMulti, 1.0);
	DECLARE_VARIABLE(bool, playing_progress_sound, false);
	DECLARE_VARIABLE(unsigned int, progress_sound_start, 0);
	DECLARE_VARIABLE(int, length_of_progress_sound, 0);
	DECLARE_VARIABLE(bool, aboutToExit, false);
	DECLARE_VARIABLE(int, lastPulseTime, 0);

	//Texutures
	static SDL_Texture* Texture_Current; declare_Piece_Text(&Texture_Current, currentPiece, CENTER_DOT);
	static SDL_Texture* Texture_OpponentCurrent; declare_Piece_Text(&Texture_OpponentCurrent, opponentCurrentPiece, false);
	static SDL_Texture* Texture_Ghost; declare_Piece_Text(&Texture_Ghost, currentPiece, CENTER_DOT);
	static SDL_Texture* Texture_Next; declare_Piece_Text(&Texture_Next, nextPiece, false);
	static SDL_Texture* Texture_OpponentNext; declare_Piece_Text(&Texture_OpponentNext, opponentNextPiece, false);
	static SDL_Texture* Texture_Hold; declare_Piece_Text(&Texture_Hold, holdPiece, false);
	static SDL_Texture* Texture_Score; declare_HUD_Text(&Texture_Score, SCORE_TEXT);
	static SDL_Texture* Texture_OpponentScore; declare_HUD_Text(&Texture_OpponentScore, SCORE_TEXT);
	static SDL_Texture* Texture_Level; declare_HUD_Text(&Texture_Level, LEVEL_TEXT);
	static SDL_Texture* Texture_OpponentLevel; declare_HUD_Text(&Texture_OpponentLevel, LEVEL_TEXT);
	static SDL_Texture* Texture_Lines; declare_HUD_Text(&Texture_Lines, LINES_TEXT);
	static SDL_Texture* Texture_Paused; declare_HUD_Text(&Texture_Paused, PAUSED_TEXT);
	static SDL_Texture* Texture_SizeBag; declare_HUD_Text(&Texture_SizeBag, SIZEBAG_TEXT);
	static SDL_Texture* foreground; declare_HUD_Text(&foreground, FOREGROUND_TEXT);
	static SDL_Texture* opponentForeground; declare_HUD_Text(&opponentForeground, FOREGROUND_TEXT);

	//Arrays
		//Initialize completedRows to only include a row that is offscreen
	static int* completedRows; declareStart(completedRows, MAP_HEIGHT + 1);
		// mapData[i][j] = 0 if nothing at that coord. Otherwise = integer equivalent of the color of the block at that coord.
	static int* mapData; declare_map_matrix(&mapData);

	// Declare the bag holding the possible sizes for bag shuffling of sizes
	static SizeBag* sizeBag; declare_size_bag(&sizeBag, MODE, CUSTOM_MODE);
	
	//First frame
	if (*firstLoop)
	{

		//Get X and Y for foreground
		*foregroundX = getForegroundX(MODE);
		*foregroundY = getForegroundY(MODE);

		// The X coordinate of the opponents foreground is offset from the regular foreground by gameWidth / 2.
		*opponentForegroundX = *foregroundX + getGameWidth(MODE, MULTIPLAYER) / 2;

		//Calculate multiplier for PAUSED text when it would be wider than the playfield
		if (MODE != 0)
			if (getStringLength("PAUSED", 1.0) > SDL_round(BASE_PLAYFIELD_WIDTH * MODE) * SPRITE_WIDTH)
				*pausedMulti = SDL_round(BASE_PLAYFIELD_WIDTH * MODE) * SPRITE_WIDTH / getStringLength("PAUSED", 1.0);

		// Calc speed at start incase want to start at different level (for debugging)
		*speed = calcSpeed(*Level);

		// Remove the size of the firstPiece from the sizeBag
		removeSizeFromBag(sizeBag, firstPiece->numOfBlocks, MODE, CUSTOM_MODE, Texture_SizeBag);

		if (MULTIPLAYER)
			sendCurrentPieceToServer(currentPiece, lastPulseTime);

		*firstLoop = false;

	}

	//CONTROLS ---------------------------------------------------------------

	//These controls only work if it is not a game over and the game is not paused and the clearing animation is not
	//playing
	if (!*gameOver && !*clearingLine && !*paused)
	{

		//Left and right movement
		if (onHold(LEFT_BUTTON) || onHold(RIGHT_BUTTON))
		{

			Uint32 currTicks = SDL_GetTicks();

			//Start the counter for holding the movement keys
			if (*moveStart == 0)
			{

				*moveStart = currTicks;
				*moveStartBool = true;

			}

			if (*moveStartBool || (currTicks - *moveStart) >= (MOVEMENT_WAIT + MOVEMENT_TIME))
			{

				if (onHold(LEFT_BUTTON) && !isColliding(*currentPiece, *X, Y, LEFT, mapData, MAP_WIDTH, MAP_HEIGHT))
				{

					//Only play sound if can actually move
					if (*X != 0)
						playSound(MOVE_SOUND);

					move(LEFT, X, *currentPiece, MAP_WIDTH);

					//Recalculate ghostY
					*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData, MAP_WIDTH, MAP_HEIGHT);

				}
				else if (onHold(RIGHT_BUTTON) && !isColliding(*currentPiece, *X, Y, RIGHT, mapData, MAP_WIDTH, MAP_HEIGHT))
				{

					if (*X + currentPiece->width < MAP_WIDTH)
						playSound(MOVE_SOUND);

					move(RIGHT, X, *currentPiece, MAP_WIDTH);

					//Recalculate ghostY
					*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData, MAP_WIDTH, MAP_HEIGHT);

				}

				*moveStartBool = false;

				//Reset moveStart
					//The way this works is this:
						//When the player pushes an arrow, the piece will move once
						//If they hold the arrow for longer than (MOVEMENT_WAIT + 
						//MOVEMENT_TIME) ms, the piece will move again
						//If they continue to hold it past that point, the piece 
						//will move every MOVEMENT_TIME ms
				if ((currTicks - *moveStart) >= (MOVEMENT_WAIT + MOVEMENT_TIME))
					*moveStart = currTicks - MOVEMENT_WAIT;

			}

		}
		else
			*moveStart = 0;

		if (onPress(ROTATE_CCW_BUTTON) || onPress(ROTATE_CW_BUTTON) || onPress(MIRROR_BUTTON))
		{

			// Save the previous X and Y of the cursor
			int previousX = *X;
			double previousY = *Y;

			// Calculate the current center of the piece
			int previousCenterX = *X + (currentPiece->centerBlock->X - currentPiece->minX);
			int previousCenterY = (int)*Y + (currentPiece->centerBlock->Y - currentPiece->minY);

			// Perform the rotation on the piece
			if (onPress(ROTATE_CCW_BUTTON))
				rotatePiece(currentPiece, CCW);
			else if (onPress(ROTATE_CW_BUTTON))
				rotatePiece(currentPiece, CW);
			else if (onPress(MIRROR_BUTTON))
				mirrorPieceOverY(currentPiece);

			// Calculate the new center of the piece
			int newCenterX = *X + (currentPiece->centerBlock->X - currentPiece->minX);
			int newCenterY = (int)*Y + (currentPiece->centerBlock->Y - currentPiece->minY);

			// Adjust X and Y of cursors so that the centerBlock is in the same place, that way it looks like we are rotating around it
			*X -= newCenterX - previousCenterX;
			*Y -= newCenterY - previousCenterY;

			// Check if the piece is now colliding with something
			if (isColliding(*currentPiece, *X, Y, NONE, mapData, MAP_WIDTH, MAP_HEIGHT))
			{

				// Boolean for storing whether a wall kick is possible
				bool ableToKick = false;

				// Store the furthest colliding X and Y values from the center of the piece
				int furthestX = 0;
				int furthestY = 0;

				// Get the minimum X and Y values of all blocks in the piece
				int minX = currentPiece->minX;
				int minY = currentPiece->minY;

				// Go through each piece
				for (unsigned short i = 0; i < currentPiece->numOfBlocks; i++)
				{

					int blockX = currentPiece->blocks[i].X;
					int blockY = currentPiece->blocks[i].Y;
					int offsetX = blockX - minX;
					int offsetY = blockY - minY;
					double currY = *Y;

					// Check if any block in the piece goes outside the bounds of the playable area
					if ((*X + offsetX < 0) || (*X + offsetX >= MAP_WIDTH) || (currY + offsetY < 0) || (currY + offsetY >= MAP_HEIGHT))
					{

						// If they do, check whether their X or Y values are further from the center than our current furthestX and
						// furthestY values. Save them if they are.
						if (SDL_abs(blockX - currentPiece->centerBlock->X) > SDL_abs(furthestX))
							furthestX = blockX - currentPiece->centerBlock->X;

						if (SDL_abs(blockY - currentPiece->centerBlock->Y) > SDL_abs(furthestY))
							furthestY = blockY - currentPiece->centerBlock->Y;

					}	// Check whether the current block is colliding with a different piece that has already been placed
					else if (*(mapData + ((int)*Y + (blockY - currentPiece->minY)) * MAP_WIDTH + (*X + (blockX - currentPiece->minX))) != 0)
					{

						// If it is, check wether the collision is to the left or right of the center. If there is a collision, then
						// we know we would need to shift the piece over by the entire distance from the center to the side of the piece
						// that the collision was on in order to ensure there would no longer be a collion.
							// If this distance is larger in magnitude than furthestX, overwrite furthestX.
						if (blockX < currentPiece->centerBlock->X)
						{

							if (SDL_abs(minX - currentPiece->centerBlock->X) > SDL_abs(furthestX))
								furthestX = minX - currentPiece->centerBlock->X;

						}
						else if (blockX > currentPiece->centerBlock->X)
						{

							if (SDL_abs((minX + currentPiece->width) - currentPiece->centerBlock->X) > SDL_abs(furthestX))
								furthestX = (minX + currentPiece->width) - currentPiece->centerBlock->X;

						}

						// Do the same thing as above but on the Y-axis.
						if (blockY < currentPiece->centerBlock->Y)
						{

							if (SDL_abs(minY - currentPiece->centerBlock->Y) > SDL_abs(furthestY))
								furthestY = minY - currentPiece->centerBlock->Y;

						}
						else if (blockY > currentPiece->centerBlock->Y)
						{

							if (SDL_abs((minY + currentPiece->height) - currentPiece->centerBlock->Y) > SDL_abs(furthestY))
								furthestY = (minY + currentPiece->height) - currentPiece->centerBlock->Y;

						}

					}

				}

				// Now we have calcuated the furthest we could possibly shift the piece in the X or Y directions to attempt a wall kick

				// Now, we try every permutation of X and Y shifts within these bounds to see if any would avoid the piece from colliding
				// with anything.
				for (unsigned short i = 0; i <= SDL_abs(furthestX); i++)
				{

					int currentOffsetX = 0;
					int currentOffsetY = 0;

					if (furthestX != 0)
						currentOffsetX = i * furthestX / SDL_abs(furthestX);

					for (unsigned short j = 0; j <= SDL_abs(furthestY); j++)
					{

						if (furthestY != 0)
							currentOffsetY = j * furthestY / SDL_abs(furthestY);

						double tempY = *Y - currentOffsetY;

						if (!isColliding(*currentPiece, *X - currentOffsetX, &tempY, NONE, mapData, MAP_WIDTH, MAP_HEIGHT))
						{

							// Once we find a permuation of shift amounts that could avoid a collision, we shift the piece by that much
							*X -= currentOffsetX;
							*Y = tempY;

							// And then we exit this double loop
							ableToKick = true;
							goto EndKickCheck;

						}

					}

				}

				EndKickCheck:;

				// If the piece was not able to be shifted with a wall kick
				if (!ableToKick)
				{

					// Reverse the move by rotating it back in the opposite direction
					if (onPress(ROTATE_CCW_BUTTON))
						rotatePiece(currentPiece, CW);
					else if (onPress(ROTATE_CW_BUTTON))
						rotatePiece(currentPiece, CCW);
					else if (onPress(MIRROR_BUTTON))
						mirrorPieceOverY(currentPiece);

					// And restoring the X and Y values of the cursor
					*X = previousX;
					*Y = previousY;

				}
				else
				{

					//Only play sound if it actually rotated
					playSound(ROTATE_SOUND);
					SDL_DestroyTexture(Texture_Current);
					Texture_Current = NULL;
					SDL_DestroyTexture(Texture_Ghost);
					Texture_Ghost = NULL;

				}

			}
			else
			{

				//Only play sound if it actually rotated
				playSound(ROTATE_SOUND);
				SDL_DestroyTexture(Texture_Current);
				Texture_Current = NULL;
				SDL_DestroyTexture(Texture_Ghost);
				Texture_Ghost = NULL;

			}

			if (MULTIPLAYER)
				sendCurrentPieceToServer(currentPiece, lastPulseTime);

			//Recalculate ghostY
			*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData, MAP_WIDTH, MAP_HEIGHT);

		}

		//Holding
			//You cannot hold twice in a row
		if (onPress(HOLD_BUTTON) && !*justHeld)
		{

			playSound(HOLD_SOUND);

			if (holdPiece == NULL)
			{

				//Create holdPiece by copying currentPiece
				holdPiece = SDL_malloc(sizeof(*holdPiece));
				holdPiece->blocks = SDL_malloc(currentPiece->numOfBlocks * sizeof(*holdPiece->blocks));
				holdPiece->centerBlock = SDL_calloc(1, sizeof(block));
				copyPiece(currentPiece, holdPiece);
				Texture_Hold = createPieceTexture(*holdPiece, false);
				SDL_QueryTexture(Texture_Hold, NULL, NULL, holdText_Width, holdText_Height);

				//Delete currentPiece
				delPiece(&currentPiece);
				SDL_DestroyTexture(Texture_Current);
				Texture_Current = NULL;
				SDL_DestroyTexture(Texture_Ghost);
				Texture_Ghost = NULL;

				//Move nextPiece to currentPiece
				currentPiece = nextPiece;
				adjustNewPiece(currentPiece, X, MAP_WIDTH);

				if (MULTIPLAYER)
					sendCurrentPieceToServer(currentPiece, lastPulseTime);

				//NextPiece no longer exists
				nextPiece = NULL;
				//Destory nextPiece
				SDL_DestroyTexture(Texture_Next);
				Texture_Next = NULL;

				//Reset Y
				*Y = 0;

				//Recalculate ghostY
				*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData, MAP_WIDTH, MAP_HEIGHT);

			}
			else //Swap holdPiece and currentPiece
			{

				//Store current holdPiece in tempPeiece
				piece* tempPiece;
				tempPiece = SDL_malloc(sizeof(*tempPiece));
				tempPiece->blocks = SDL_malloc(holdPiece->numOfBlocks * sizeof(*tempPiece->blocks));
				tempPiece->centerBlock = SDL_calloc(1, sizeof(block));
				copyPiece(holdPiece, tempPiece);

				//Recreate holdPiece
				delPiece(&holdPiece);
				SDL_DestroyTexture(Texture_Hold);
				holdPiece = SDL_malloc(sizeof(*holdPiece));
				holdPiece->blocks = SDL_malloc(currentPiece->numOfBlocks * sizeof(*holdPiece->blocks));
				holdPiece->centerBlock = SDL_calloc(1, sizeof(block));
				copyPiece(currentPiece, holdPiece);
				Texture_Hold = createPieceTexture(*holdPiece, false);
				SDL_QueryTexture(Texture_Hold, NULL, NULL, holdText_Width, holdText_Height);

				//Delete currentPiece
				delPiece(&currentPiece);
				SDL_DestroyTexture(Texture_Current);
				Texture_Current = NULL;
				SDL_DestroyTexture(Texture_Ghost);
				Texture_Ghost = NULL;

				//Copy tempPiece to currentPiece
				currentPiece = SDL_malloc(sizeof(*currentPiece));
				currentPiece->blocks = SDL_malloc(tempPiece->numOfBlocks * sizeof(*tempPiece->blocks));
				currentPiece->centerBlock = SDL_calloc(1, sizeof(block));
				copyPiece(tempPiece, currentPiece);
				adjustNewPiece(currentPiece, X, MAP_WIDTH);

				if (MULTIPLAYER)
					sendCurrentPieceToServer(currentPiece, lastPulseTime);

				//Delete tempPeice
				delPiece(&tempPiece);

				//Reset Y
				*Y = 0;

				//Recalculate ghostY
				*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData, MAP_WIDTH, MAP_HEIGHT);

			}

			*justHeld = true;

		}

		//Hard drop
		if (onPress(HARD_DROP_BUTTON))
			*Y = *ghostY + 1;
			
		//Soft drop
		if (onHold(SOFT_DROP_BUTTON))
			*softDrop = true;
		else
			*softDrop = false;

	}
	
	if (!*gameOver)	//These controls work even when paused, as long as the game is
	{				//not over

		//Pausing
		if (onPress(SELECT_BUTTON))
		{

			// If aboutToExit and press SELECT_BUTTON, exit to main menu
			if (*aboutToExit == true)
			{

				freeVars();
				return RESET;

			}
			else {

				// If not aboutToExit, just pause the game normally
				playSound(PAUSE_SOUND);
				*paused = !*paused;	

			}

		}
		// Also pause if window loses focus
		if (!(SDL_GetWindowFlags(globalInstance->window) & SDL_WINDOW_INPUT_FOCUS) && !*paused)
		{
		
			playSound(PAUSE_SOUND);
			*paused = true;

		}

		if (*paused && onPress(EXIT_BUTTON))
		{

			// When press EXIT_BUTTON while paused, switch between aboutToExit and not
			*aboutToExit = !*aboutToExit;

			playSound(ROTATE_SOUND);

			// Updpate the PAUSED texture with the aboutToExit value
			updatePausedText(Texture_Paused, *aboutToExit);

			// If you press EXIT_BUTTON while aboutToExit, leave the pause menu
			if (*aboutToExit == false)
			{

				playSound(PAUSE_SOUND);
				*paused = !*paused;

			}

		}

	}

	//-----------------------------------------------------------------------

	//LOGIC ------------------------------------------------------------------------------

	//If nextPiece does not exist, generate a new one
	if (nextPiece == NULL)
	{

		// Pick random size from the sizeBag to generate next piece
		nextPiece = generateGamePiece(sizeBag->sizesInBag[rand() % sizeBag->size]);

		//And create a new texture for it as well
		if (Texture_Next == NULL)
		{
			
			Texture_Next = createPieceTexture(*nextPiece, false);
			SDL_QueryTexture(Texture_Next, NULL, NULL, nextText_Width, nextText_Height);

		}

		// Remove size of nextPiece from sizeBag
		removeSizeFromBag(sizeBag, nextPiece->numOfBlocks, MODE, CUSTOM_MODE, Texture_SizeBag);

		// If in a muliplayer game, send the NEXT piece to the server
		if (MULTIPLAYER)
			sendNextPieceToServer(nextPiece, lastPulseTime);

	}

	//Gravity
	if (currentPiece != NULL && !*gameOver && !*paused)
	{
		
		bool pieceIsPlaced = false;

		//Piece is falling
		if (!isColliding(*currentPiece, *X, Y, DOWN, mapData, MAP_WIDTH, MAP_HEIGHT))
		{

			//Don't drop piece while the clearingLine animation is in progress
			if (!*clearingLine)
			{

				// Soft drop drops at 1 cell per 2 frames (in 60HZ)
					// Soft drop doesn't work past level 28 because base speed is higher than soft drop
				if (*softDrop == false || *Level > 28)
					*Y = *Y + *speed * globalInstance->frame_time;
				else if (*softDrop == true)
					*Y = *Y + ((double)60.0988 / (double)2) * globalInstance->frame_time;

				// Check again if piece now colliding after moving down and place piece if it is
				if (isColliding(*currentPiece, *X, Y, DOWN, mapData, MAP_WIDTH, MAP_HEIGHT))
					pieceIsPlaced = true;

			}

		}
		else
			pieceIsPlaced = true;


		if (pieceIsPlaced)     //Piece is placed
		{

			playSound(LAND_SOUND);

			drawPiece(*currentPiece, foreground, *X * SPRITE_WIDTH, (int)*Y * SPRITE_HEIGHT);

			//Store piece in mapData
			for (unsigned short i = 0; i < currentPiece->numOfBlocks; i++) {

				int row = (int)*Y + (currentPiece->blocks[i].Y - currentPiece->minY);
				int column = *X + (currentPiece->blocks[i].X - currentPiece->minX);

				*(mapData + row * MAP_WIDTH + column) = currentPiece->color;

			}

			//Check if the player just completed a line
			if (numCompleted != NULL)
				*numCompleted = completedLine(mapData, (unsigned short)*Y, *currentPiece, &completedRows, MAP_WIDTH, MAP_HEIGHT);

			if (*numCompleted > 0)
			{

				//This enables the clearing animation to play
				*clearingLine = true;

				//Scoring formula was derived by plotting the scoring table from the NES 
				//version of tetris and then finding a best-fit line
				*Score += ((93.333 * SDL_pow(*numCompleted, 3) - 490 * SDL_pow(*numCompleted, 2) + 876.67 * *numCompleted - 440) * (*Level + 1));
				updateScore(*Score, Texture_Score);

				// Send score to the server if in a multiplayer game
				if (MULTIPLAYER)
					sendScoretoServer(*Score, lastPulseTime);

				//Keep track of the number of lines that have been cleared on the current 
				//level
				*linesAtCurrentLevel += *numCompleted;
				// Loop in case of passing multiple levels with the same placement
				while (*linesAtCurrentLevel >= LEVELUP_INCREASE * (*Level + 1))
				{
					
					//If the player completes more lines than is needed to level up, the completed lines will
					//carry-over between levels
					*linesAtCurrentLevel = *linesAtCurrentLevel - LEVELUP_INCREASE * (*Level + 1);

					//Increase level count and update Texture_Level
					*Level += 1;
					updateLevel(*Level, Texture_Level);
					
					// Send level to server if in a multiplayer game
					if (MULTIPLAYER)
						sendLevelToServer(*Level, lastPulseTime);

					//If player reaches the appropriate level to unlock a new size in numerical mode
					if (*Level >= MODE && MODE == PROGRESS && CUSTOM_MODE == false)
					{
					
						playSound(UNLOCK_SOUND);

						//Keep track of the time at which the PROGRESS sound started playing
						*playing_progress_sound = true;
						*progress_sound_start = SDL_GetTicks();
						*length_of_progress_sound = getAudioLengthInMS(globalInstance->sounds[UNLOCK_SOUND]);
						
						PROGRESS++;

					}

					//Increase speed
					*speed = calcSpeed(*Level);

				}

				//Update the texture displaying how many more lines are needed to reach the next level
				updateLines(calcLinesUntilLevelup(*linesAtCurrentLevel, *Level), &Texture_Lines);

			}

			//Delete texture for currentPiece
			delPiece(&currentPiece);
			SDL_DestroyTexture(Texture_Current);
			Texture_Current = NULL;
			SDL_DestroyTexture(Texture_Ghost);
			Texture_Ghost = NULL;

			//Grab the next piece and store it in currentPiece
			currentPiece = nextPiece;
			adjustNewPiece(currentPiece, X, MAP_WIDTH);

			if (MULTIPLAYER)
				sendCurrentPieceToServer(currentPiece, lastPulseTime);

			//NextPiece no longer exists
			nextPiece = NULL;
			//So delete texture for next piece
			SDL_DestroyTexture(Texture_Next);
			Texture_Next = NULL;

			//Reset Y
			*Y = 0;

			//Player can hold again
			if (*justHeld)
				*justHeld = false;

			//Recalculate ghostY
			*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData, MAP_WIDTH, MAP_HEIGHT);

			//Player lost
			if (isColliding(*currentPiece, *X, Y, NONE, mapData, MAP_WIDTH, MAP_HEIGHT))
			{

				playSound(OVER_SOUND);
				*gameOver = true;

			}

			// If in a multiplayer game
				// Send your MAP data to the server whenever you place a piece
			if (MULTIPLAYER)
				sendMapToServer(mapData, lastPulseTime);

		}

	}

	// If in a multiplayer game
	if (MULTIPLAYER)
	{

		if ((*lastXInt != *X) || (*lastYInt != (int)*Y))
		{

			sendPositionToServer(*X, (int)*Y, lastPulseTime);
			*lastXInt = *X;
			*lastYInt = (int)*Y;

		}

		// Check for incoming data from the server
		if (SDLNet_CheckSockets(globalInstance->serverSocketSet, 0))
		{

			// Capture data from server in 1024-byte chunks and combine them together into data pointer
			char* data = NULL;
			int dataLen = 0;
			while(SDLNet_CheckSockets(globalInstance->serverSocketSet, 0))
			{

				char currentData[1024];
				int currentLen = SDLNet_TCP_Recv(globalInstance->serverSocket, currentData, 1024);

				if (data == NULL)
				{

					data = SDL_calloc(currentLen, sizeof(char));
					SDL_memcpy(data, currentData, currentLen);
					dataLen = currentLen;

				}
				else
				{

					data = SDL_realloc(data, dataLen + currentLen);
					SDL_memcpy(&data[dataLen], currentData, currentLen);
					dataLen += currentLen;

				}

			}

			// Split data into "packets". A packet is basically a set of data for a specific variable.
				// For example,"MAP=...NEXT=...SCORE=..." could all be received at the same time, and be stored
				// in the data pointer at the same time, and here we would split it into three separate packets.
			char** packets = NULL;
			int numPackets = 0;
			char* currentStringValue = NULL;
			for (int dataIndex = 0; dataIndex < dataLen; dataIndex++)
			{

				if (data[dataIndex] != '\0')
				{

					if (currentStringValue == NULL)
					{

						currentStringValue = SDL_calloc(2, sizeof(char));
						currentStringValue[0] = data[dataIndex];
						currentStringValue[1] = '\0';

					}
					else
					{

						int newLen = SDL_strlen(currentStringValue) + 1 + 1;
						currentStringValue = SDL_realloc(currentStringValue, newLen * sizeof(char));
						SDL_strlcat(currentStringValue, &(data[dataIndex]), newLen);

					}

				}
				else
				{

					if (numPackets == 0)
					{

						packets = SDL_calloc(1, sizeof(char*));
						numPackets++;

					}
					else
					{

						packets = SDL_realloc(packets, (numPackets + 1) * sizeof(char*));
						numPackets++;                                

					}

					packets[numPackets - 1] = SDL_calloc(SDL_strlen(currentStringValue) + 1, sizeof(char));
					SDL_strlcpy(packets[numPackets - 1], currentStringValue, SDL_strlen(currentStringValue) + 1);

					SDL_free(currentStringValue);
					currentStringValue = NULL;

				}

			}

			if (currentStringValue != NULL)
			{

				SDL_free(currentStringValue);
				currentStringValue = NULL;

			}

			// Now go through each packet and process it
			for (unsigned short packetIndex = 0; packetIndex < numPackets; packetIndex++)
			{

				// If the data received is MAP data
				if (SDL_strstr(packets[packetIndex], "MAP") != NULL)
				{

					int stringIndex = SDL_strlen("MAP=");
					int mapDataIndex = 0;

					// Clear the opponents background texture
					clearTexture(opponentForeground);

					// Iterate through the MAP data and place the pieces on the opponents foreground texture
					while (packets[packetIndex][stringIndex] != '\0')
					{

						if (packets[packetIndex][stringIndex] != '0')
						{

							int X = mapDataIndex % MAP_WIDTH * SPRITE_WIDTH;
							int Y = mapDataIndex / MAP_WIDTH * SPRITE_HEIGHT;
							// Color of the block can be determined based on the digit that is present in the string
							drawToTexture(BLOCK_SPRITE_ID, opponentForeground, X, Y, 1.0, (Uint8)packets[packetIndex][stringIndex] - '0');

						}

						stringIndex++;
						mapDataIndex++;

					}

				}	// If the data receveid is SCORE data
				else if (SDL_strstr(packets[packetIndex], "SCORE") != NULL)
				{

					// Extract the score from the received data and update the Texture_OpponentScore
					int opponentScore = SDL_atoi(SDL_strstr(packets[packetIndex], "SCORE=") + SDL_strlen("SCORE=") * sizeof(char));
					updateScore(opponentScore, Texture_OpponentScore);

				}
				else if (SDL_strstr(packets[packetIndex], "NEXT") != NULL)	// If the data received is a NEXT piece from the opponent
				{

					// Delete the current opponents NEXT piece
					if (opponentNextPiece != NULL)
					{

						delPiece(&opponentNextPiece);
						SDL_DestroyTexture(Texture_OpponentNext);
						Texture_OpponentNext = NULL;

					}

					// And create a new one
					char* pieceString = SDL_strstr(packets[packetIndex], "NEXT=") + SDL_strlen("NEXT=") * sizeof(char);
					opponentNextPiece = createPieceFromString(pieceString);
					Texture_OpponentNext = createPieceTexture(*opponentNextPiece, false);
					SDL_QueryTexture(Texture_OpponentNext, NULL, NULL, opponentNextText_Width, opponentNextText_Height);

				}
				else if (SDL_strstr(packets[packetIndex], "LEVEL") != NULL)
				{

					// Extract the level from the received data and update the Texture_OpponentLevel
					*opponentLevel = SDL_atoi(SDL_strstr(packets[packetIndex], "LEVEL=") + SDL_strlen("LEVEL=") * sizeof(char));
					updateLevel(*opponentLevel, Texture_OpponentLevel);

				}
				else if (SDL_strstr(packets[packetIndex], "CURRENT") != NULL)	// If the data is a CURRENT piece from
				{																// the opponent

					// Delete the current CURRENT piece for the opponent
					if (opponentCurrentPiece != NULL)
					{

						delPiece(&opponentCurrentPiece);
						SDL_DestroyTexture(Texture_OpponentCurrent);
						Texture_OpponentCurrent = NULL;

					}

					// And create a new one
					char* pieceString = SDL_strstr(packets[packetIndex], "CURRENT=") + SDL_strlen("CURRENT=") * sizeof(char);
					opponentCurrentPiece = createPieceFromString(pieceString);
					Texture_OpponentCurrent = createPieceTexture(*opponentCurrentPiece, false);

				}
				else if (SDL_strstr(packets[packetIndex], "POSITION") != NULL)
				{

					// Parse the X and Y values from the payload. Given in the format "POSITION=X|Y|"
					int valueIndex = 0;
					int stringIndex = SDL_strlen("POSITION=");
					char* currentStringValue = NULL;
					while (packets[packetIndex][stringIndex] != '\0')
					{

						if (packets[packetIndex][stringIndex] != '|')
						{

							// Build a string for the current value we are reading
							if (currentStringValue == NULL)
							{

								currentStringValue = SDL_calloc(2, sizeof(char));
								currentStringValue[0] = packets[packetIndex][stringIndex];
								currentStringValue[1] = '\0';

							}
							else
							{

								int newLen = SDL_strlen(currentStringValue) + 1 + 1;
								currentStringValue = SDL_realloc(currentStringValue, newLen * sizeof(char));
								SDL_strlcat(currentStringValue, &(packets[packetIndex][stringIndex]), newLen);

							}

						}
						else	// Once we hit a delimiter, we know we are done reading the current value
						{

							// So apply it to the currect attribute for the piece depending on the current valueIndex
							if (valueIndex == 0)
								*opponentX = SDL_atoi(currentStringValue);
							else if (valueIndex == 1)
								*opponentY = SDL_atoi(currentStringValue);

							valueIndex++;

							// Free the currentStringValue so that we can start reading the next value
							SDL_free(currentStringValue);
							currentStringValue = NULL;

						}

						stringIndex++;	

					}

					// Ensure we free the memory for the currentStringValue
					if (currentStringValue != NULL)
					{

						SDL_free(currentStringValue);
						currentStringValue = NULL;

					}

				}

			}

			// Free our data and packets to avoid memory leaks
			for (int i = 0; i < numPackets; i++)
				SDL_free(packets[i]);
			SDL_free(packets);
			SDL_free(data);

		}

		if ((SDL_GetTicks() - *lastPulseTime) / 1000 > MULTIPLAYER_PULSE_DELAY_SECONDS)
		{

			char message[] = "PULSE";
			SDLNet_TCP_Send(globalInstance->serverSocket, message, SDL_strlen(message) + 1);
		

			*lastPulseTime = SDL_GetTicks();

		}

	}

	// RENDERING -------------------------------------------------------------

	//Draw various textures that don't move
	if (CUSTOM_MODE && MODE > MAX_PIECE_SIZE)
	{

		float multi = (float)MAX_PIECE_SIZE / (float)MODE;
		int width = SDL_ceil((float)*nextText_Width * multi);
		int height = SDL_ceil((float)*nextText_Height * multi);

		drawTexture(Texture_Next, getNextX(MODE, width), getNextY(MODE, height), multi);

	}
	else
		drawTexture(Texture_Next, getNextX(MODE, *nextText_Width), getNextY(MODE, *nextText_Height), 1.0);


	if (CUSTOM_MODE && MODE > MAX_PIECE_SIZE)
	{

		float multi = (float)MAX_PIECE_SIZE / (float)MODE;
		int width = SDL_ceil((float)*holdText_Width * multi);
		int height = SDL_ceil((float)*holdText_Height * multi);

		drawTexture(Texture_Hold, getHoldX(MODE, width), getHoldY(MODE, height), HOLD_TEXTURE_MULTI * multi);

	}
	else
		drawTexture(Texture_Hold, getHoldX(MODE, *holdText_Width), getHoldY(MODE, *holdText_Height), HOLD_TEXTURE_MULTI);

	drawTexture(Texture_Score, getScoreDrawX(MODE), getScoreDrawY(MODE), 1.0);
	drawTexture(Texture_Level, getLevelX(MODE, *Level), getLevelY(MODE), 1.0);
	drawTexture(Texture_Lines, getLinesX(MODE, calcLinesUntilLevelup(*linesAtCurrentLevel, *Level)), getLinesY(MODE), 1.0);
	drawTexture(Texture_SizeBag, getSizeBagX(MODE, getSizeBagMulti(MODE)), getSizeBagY(MODE), getSizeBagMulti(MODE));

	//Draw the foreground
	if (CUSTOM_MODE == false || MODE < MAX_PIECE_SIZE)
		drawTexture(foreground, *foregroundX, *foregroundY, 1.0);
	else	// In SIZE > MAX_PIECE_SIZE, the foreground is rendered smaller to fit within the same sized play area as MULTRIS mode.
		drawTexture(foreground, *foregroundX, *foregroundY, (float)MAX_PIECE_SIZE / (float)MODE);

	// If in a multiplayer game, draw the opponents textures
	if (MULTIPLAYER)
	{

		drawTexture(opponentForeground, *opponentForegroundX, *foregroundY, 1.0);
		drawTexture(Texture_OpponentScore, getScoreDrawX(MODE) + getGameWidth(MODE, MULTIPLAYER) / 2, getScoreDrawY(MODE), 1);
		int X = getNextX(MODE, *opponentNextText_Width) + getGameWidth(MODE, MULTIPLAYER) / 2;
		drawTexture(Texture_OpponentNext, X, getNextY(MODE, *opponentNextText_Height), 1.0);
		X = getLevelX(MODE, *opponentLevel) + getGameWidth(MODE, MULTIPLAYER) / 2;
		drawTexture(Texture_OpponentLevel, X, getLevelY(MODE), 1.0);
		X = SDL_ceil((float)FONT_WIDTH * (float)(*opponentX) + (float)*opponentForegroundX);
		int  Y = SDL_ceil((float)FONT_HEIGHT * (float)(*opponentY) + (float)*foregroundY);
		drawTexture(Texture_OpponentCurrent, X, Y, 1.0);

	}

	//Draw current piece if game isnt over
	if (*gameOver == false)
	{

		// If playing CUSTOM_MODE with a size > MAX_PIECE_SIZE, we need to scale Texture_current.
		float multiplier = 1.0;
		if (CUSTOM_MODE && MODE > MAX_PIECE_SIZE)
			multiplier = (float)MAX_PIECE_SIZE / (float)MODE;

		int drawX = SDL_ceil((float)FONT_WIDTH * (float)(*X) * multiplier + (float)*foregroundX);
		int drawY = SDL_ceil((float)FONT_HEIGHT * (float)((int)*Y) * multiplier + (float)*foregroundY);

		drawTexture(Texture_Current, drawX, drawY, multiplier);
		
		drawY = SDL_ceil((float)FONT_HEIGHT * (float)(int)*ghostY * multiplier + (float)*foregroundY);

		//Make Texture_Ghost transparent
		SDL_SetTextureAlphaMod(Texture_Ghost, 255 / 3);
		//Draw Texture_Ghost at ghostY
		if ((int)*ghostY >= 0)
			drawTexture(Texture_Ghost, drawX, drawY, multiplier);
		//Reset Texture_Ghost opacity
		SDL_SetTextureAlphaMod(Texture_Ghost, 255);

	}

	//Draw PAUSED if game is paused
		//Center the text
	if (*paused)
		drawTexture(Texture_Paused, getPausedX(MODE, *pausedMulti), getPausedY(MODE, *pausedMulti), *pausedMulti);

	//Animations -----------------------------------------------------------------

	//Line-clearing animation -------------------
	if (*clearingLine == true && numCompleted != NULL)
	{
		
		//Keep track of the number of lines completed prior to drawing the next frame of the line animation
			//This is because the playLineAnimation() function modifies the numCompleted variable
		unsigned short prevNumCompleted = *numCompleted;
		
		//Play a single frame of the line animation
		if (playLineAnimation(foreground, *completedRows, clearingLine, mapData, numCompleted)) {

			//This if statement only gets entered after the completion of a single line being cleared in animation
			
			//Set the playing_progress_sound value to false once the progress sound is done playing
				//This is so that we can start playing COMPLETE_SOUND again
			if ((int)(SDL_GetTicks() - *progress_sound_start) > *length_of_progress_sound)
				*playing_progress_sound = false;

			//Play COMPLETE_SOUND as long as the PROGRESS_SOUND is not playing
			if (!*playing_progress_sound)
				playSound(COMPLETE_SOUND);

			// Send the mapData to the server now that a row has been removed.
			if (MULTIPLAYER)
				sendMapToServer(mapData, lastPulseTime);

		}

		// This is added because otherwise the last removed line will be missed
		if (*clearingLine == false && MULTIPLAYER)
			sendMapToServer(mapData, lastPulseTime);

		//Remove first element in completedRows array
			//Also resize completedRows array
		if (prevNumCompleted != *numCompleted && *numCompleted > 0)
		{

			unsigned short* tempRows;
			tempRows = SDL_malloc(*numCompleted * sizeof(*tempRows));
			for (unsigned short i = 0; i < *numCompleted; i++)
				*(tempRows + i) = *(completedRows + i + 1);

			SDL_free(completedRows);
			completedRows = SDL_malloc(*numCompleted * sizeof(*completedRows));
			for (unsigned short i = 0; i < *numCompleted; i++)
				*(completedRows + i) = *(tempRows + i);

			SDL_free(tempRows);

		}

		//Recalculate ghostY
		*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData, MAP_WIDTH, MAP_HEIGHT);
		
	}

	//If game is over
	if (*gameOver == true)
	{

		//If the overAnimation isnt playing
		if (*overAnimation == false)
		{

			*overAnimation = playOverAnimation(foreground, MAP_WIDTH, MAP_HEIGHT);

			//Once animation is over, print GAME OVER to screen
			if (*overAnimation == true)
			{

				//Calculate multiplier, width, and height for "GAME" and "OVER"
				float multi = (float)(MAP_WIDTH * SPRITE_WIDTH) / (float)(getStringLength("GAME", 1)) * 0.9;
				int width = getStringLength("GAME", multi);
				int height = multi * FONT_HEIGHT;

				//Calculate X and Y value to print "GAME" at
				int x = 0.5 * (MAP_WIDTH * SPRITE_WIDTH - getStringLength("GAME", multi));
				int y = (0.5 * (MAP_HEIGHT * SPRITE_HEIGHT - (height * 2 + SDL_round(multi * STRING_GAP))));

				//Print "GAME"
					//Draw black rectangle behind it 
				drawSimpleRect(foreground, x, y, width, height, BLACK);
				printToTexture("GAME", foreground, x, y, multi, WHITE);
				
				//Draw black rectangle between GAME and OVER
				y = y + height;
				drawSimpleRect(foreground, x, y, width, SDL_round(multi * STRING_GAP), BLACK);

				//Calculate Y value for "OVER" and print it
					//Also draw a black box behind it
				y = y + SDL_round(multi * STRING_GAP);
				drawSimpleRect(foreground, x, y, width, height, BLACK);
				printToTexture("OVER", foreground, x, y, multi, WHITE);

				//Save score once overAnimation is finished playing
				if (*Score > loadTop(MODE, CUSTOM_MODE))
					saveTop(*Score, MODE, CUSTOM_MODE);

				//Also save the Progress to disk
				if (PROGRESS > (int)(loadProgress()))
					saveProgress();

			}

		}
		else
		{

			//Once the game is over, pressing the SELECT_BUTTON will exit back to the main menu
			if (onPress(SELECT_BUTTON))
			{

				//Free all memory taken by PLAYMODE -----------------------------------

				freeVars();
				return RESET;

			}

		}

	}

	//----------------------------------------------------------------------------------

	//------------------------------------------------------------------------------

	return PLAY_SCREEN;

}

// Function for sending current position to the server
void sendPositionToServer(int X, int Y, int* lastPulseTime)
{

	// Convert X value into a string
	char* xString = SDL_calloc(getIntLength(X) + 1, sizeof(char));
	SDL_itoa(X, xString, 10);
	xString[getIntLength(X)] = '\0';

	// Conver Y value into a string
	char* yString = SDL_calloc(getIntLength(Y) + 1, sizeof(char));
	SDL_itoa(Y, yString, 10);
	yString[getIntLength(Y)] = '\0';

	// Send the position to the server in the format "POSITION=X|Y|"
	int len = SDL_strlen("POSITION=") + SDL_strlen(xString) + SDL_strlen("|") + SDL_strlen(yString) + SDL_strlen("|") + 1;
	char* data = SDL_calloc(len, sizeof(char));
	SDL_strlcpy(data, "POSITION=", len);
	SDL_strlcat(data, xString, len);
	SDL_strlcat(data, "|", len);
	SDL_strlcat(data, yString, len);
	SDL_strlcat(data, "|", len);

	SDL_free(xString);
	SDL_free(yString);

	SDLNet_TCP_Send(globalInstance->serverSocket, data, len);

	*lastPulseTime = SDL_GetTicks();

	SDL_free(data);

}

// Function for sending level data to the server
void sendLevelToServer(int level, int* lastPulseTime)
{

	// Convert the level into a string
	char* levelString = SDL_calloc(getIntLength(level) + 1, sizeof(char));
	SDL_itoa(level, levelString, 10);
	levelString[getIntLength(level)] = '\0';

	// Prepend the "LEVEL=" header to the data string
	int len = SDL_strlen("LEVEL=") + SDL_strlen(levelString) + 1;
	char* data = SDL_calloc(len, sizeof(char));
	SDL_strlcpy(data, "LEVEL=", len);
	SDL_strlcat(data, levelString, len);

	// Ensure data ends in null-byte
	data[len] = '\0';

	// Send data to server
	SDLNet_TCP_Send(globalInstance->serverSocket, data, len);

	// Keep track of when last communication with server was
	*lastPulseTime = SDL_GetTicks();

	// Free the levelString to avoid memory leaks
	SDL_free(levelString);

	SDL_free(data);

}

// Send the current CURRENT piece to the server
void sendCurrentPieceToServer(piece* currentPiece, int *lastPulseTime)
{

	// Convert the piece to a string
	char* currentPieceAsString = convertPieceToString(currentPiece);

	// Pre-pend "CURRENT=" to the string
	int len = SDL_strlen("CURRENT=") + SDL_strlen(currentPieceAsString) + 1;
	char* data = SDL_calloc(len, sizeof(char));
	SDL_strlcpy(data, "CURRENT=", len);
	SDL_strlcat(data, currentPieceAsString, len);

	// Send the string to the server
	SDLNet_TCP_Send(globalInstance->serverSocket, data, len);

	// Keep track of when last communication with server was
	*lastPulseTime = SDL_GetTicks();

	// Free the strings to avoid memory leaks
	SDL_free(currentPieceAsString);
	SDL_free(data);

}

// Function for sending a NEXT piece to the server
void sendNextPieceToServer(piece* nextPiece, int* lastPulseTime)
{

	// Conver the piece to a string
	char* nextPieceAsString = convertPieceToString(nextPiece);

	// Pre-pend "NEXT=" to the string
	int len = SDL_strlen("NEXT=") + SDL_strlen(nextPieceAsString) + 1;
	char* data = SDL_calloc(len, sizeof(char));
	SDL_strlcpy(data, "NEXT=", len);
	SDL_strlcat(data, nextPieceAsString, len);

	// Send the string to the server
	SDLNet_TCP_Send(globalInstance->serverSocket, data, len);

	// Keep track of the last time data was sent to the server
	*lastPulseTime = SDL_GetTicks();

	// Free the strings to avoid memory leaks
	SDL_free(nextPieceAsString);
	SDL_free(data);

}

// Function for sending score data to the server
void sendScoretoServer(int score, int* lastPulseTime)
{

	// Convert the score into a string
	char* scoreString = SDL_calloc(getIntLength(score) + 1, sizeof(char));
	SDL_itoa(score, scoreString, 10);
	scoreString[getIntLength(score)] = '\0';

	// Prepend the "SCORE=" header to the data string
	int len = SDL_strlen("SCORE=") + SDL_strlen(scoreString) + 1;
	char* data = SDL_calloc(len, sizeof(char));
	SDL_strlcpy(data, "SCORE=", len);
	SDL_strlcat(data, scoreString, len);

	// Ensure data ends in null-byte
	data[len] = '\0';

	// Send data to server
	SDLNet_TCP_Send(globalInstance->serverSocket, data, len);

	// Keep track of when last communication with server was
	*lastPulseTime = SDL_GetTicks();

	// Free the scoreString to avoid memory leaks
	SDL_free(scoreString);

	SDL_free(data);

}

// Function for sending mapData to the server
void sendMapToServer(int* mapData, int* lastPuleTime)
{

	int len = MAP_WIDTH * MAP_HEIGHT + SDL_strlen("MAP=") + 1;
	char* data = SDL_calloc(len, sizeof(char));
	SDL_strlcpy(data, "MAP=", len);
	
	int index = SDL_strlen("MAP=");

	for (unsigned short i = 0; i < MAP_HEIGHT; i++)
	{

		for (unsigned short j = 0; j < MAP_WIDTH; j++)
		{

			// MAP data is sent as a string of digits
			data[index] = '0' + (char)*(mapData + i * MAP_WIDTH + j);

			index++;

		}

	}

	// MAP data ends with a null-terminator
	data[len] = '\0';

	// Send the data
	SDLNet_TCP_Send(globalInstance->serverSocket, data, len);

	// Keep track of the last time we sent data to the server.
	*lastPuleTime = SDL_GetTicks();

	// Then free the data to avoid memory leaks
	SDL_free(data);

}

// Function for removing a size from the sizeBag
	// Also resets the sizeBag if the last size is removed
void removeSizeFromBag(SizeBag* sizeBag, unsigned short size, unsigned short mode, bool customMode, SDL_Texture* sizeBagTexture)
{

	// If in NUMERICAL mode, don't take size out of bag
	if (customMode == false && mode != 0)
		return;

	// If there is more that one size currently in the bag
	if (sizeBag->size > 1)
	{

		// Create a new array to hold the sizes. Size is decreased by 1 since we are removing a size
		unsigned short* newSizesInBag = SDL_calloc(sizeBag->size - 1, sizeof(unsigned short));

		// Then copy all sizes from old array to new array, skipping the specified size we are removing
		unsigned short index = 0;
		for (unsigned short i = 0; i < sizeBag->size; i++)
		{

			if (sizeBag->sizesInBag[i] != size)
			{

				newSizesInBag[index] = sizeBag->sizesInBag[i];
				index++;

			}

		}

		// Then delete the old array, and update sizeBag to point to the new array
		SDL_free(sizeBag->sizesInBag);
		sizeBag->sizesInBag = newSizesInBag;
		
		// Decrease size of bag by 1
		sizeBag->size -= 1;

		// Remove size from sizeBag texture
		removeSizeFromBagTexture(sizeBagTexture, size);

	}	// If there is only one size left in the bag
	else if (sizeBag->size == 1)
	{

		// maxSize is MAX_PIECE_SIZE if in MULTRIS mode
		int maxSize = MODE;
		if (MODE == 0)
			maxSize = MAX_PIECE_SIZE;

		// Reset sizeBag to have all sizes up to maxSize
		unsigned short* newSizesInBag = SDL_calloc(maxSize, sizeof(unsigned short));
		for (unsigned short i = 0; i < maxSize; i++)
			newSizesInBag[i] = i + 1;

		// Then delete the old array, and update sizeBag to point to the new array
		SDL_free(sizeBag->sizesInBag);
		sizeBag->sizesInBag = newSizesInBag;

		// And reset the size of the bag to MAX_PIECE_SIZE
		sizeBag->size = maxSize;

		// Reset the sizeBag texture
		resetSizeBagTexture(sizeBagTexture);

	}

}

// Function for placing an X over a size in the sizeBag texture
void removeSizeFromBagTexture(SDL_Texture* sizeBagTexture, unsigned short size)
{

	// Get the top and bottom strings
	char* stringOne = createSizeBagStringOne();
	char* stringTwo = createSizeBagStringTwo();
	
	// Convert the size to a string
	char* sizeAsString = SDL_calloc(getIntLength(size) + 1, sizeof(char));;
	SDL_itoa(size, sizeAsString, 10);

	// Look for sizeAsString in stringOne
	if (SDL_strstr(stringOne, sizeAsString) != NULL)
	{

		// Get position of sizeAsString
		int pos = SDL_strstr(stringOne, sizeAsString) - stringOne;

		// Print red X's covering up sizeAsString
		if (size < 10)
			printToTexture("X", sizeBagTexture, pos * (FONT_WIDTH + STRING_GAP), 0, 1.0, RED);
		else
			printToTexture("XX", sizeBagTexture, pos * (FONT_WIDTH + STRING_GAP), 0, 1.0, RED);

	}	// Look for sizeAsString in string Two
	else if (SDL_strstr(stringTwo, sizeAsString) != NULL)
	{

		// Get position of sizeAsString
		int pos = SDL_strstr(stringTwo, sizeAsString) - stringTwo;

		// Print red X's covering up sizeAsString
		if (size < 10)
			printToTexture("X", sizeBagTexture, pos * (FONT_WIDTH + STRING_GAP), FONT_WIDTH + STRING_GAP, 1.0, RED);
		else
			printToTexture("XX", sizeBagTexture, pos * (FONT_WIDTH + STRING_GAP), FONT_WIDTH + STRING_GAP, 1.0, RED);

	}

	// Free strings
	SDL_free(stringOne);
	SDL_free(stringTwo);
	SDL_free(sizeAsString);

}

// Function for resetting the texture of the sizeBag
void resetSizeBagTexture(SDL_Texture* sizeBagTexture)
{

	// Get top and bottoms strings
	char* stringOne = createSizeBagStringOne();
	char* stringTwo =  createSizeBagStringTwo();

	// Re-print strings to sizeBagTexture
	clearTexture(sizeBagTexture);
	printToTexture(stringOne, sizeBagTexture, 0, 0, 1.0, WHITE);
	printToTexture(stringTwo, sizeBagTexture, 0, FONT_HEIGHT + STRING_GAP, 1.0, WHITE);

	// Free strings
	SDL_free(stringOne);
	SDL_free(stringTwo);

}

// Speed is calculated as if the game is running at ~60 FPS
double calcSpeed(unsigned short level)
{

	unsigned short framesPerCell = 48;

	// Speed table taken from NES Tetris
	if (level == 0)
		framesPerCell = 48;
	else if (level == 1)
		framesPerCell = 43;
	else if (level == 2)
		framesPerCell = 38;
	else if (level == 3)
		framesPerCell = 33;
	else if (level == 4)
		framesPerCell = 28;
	else if (level == 5)
		framesPerCell = 23;
	else if (level == 6)
		framesPerCell = 18;
	else if (level == 7)
		framesPerCell = 13;
	else if (level == 8)
		framesPerCell = 8;
	else if (level == 9)
		framesPerCell = 6;
	else if (level <= 12)
		framesPerCell = 5;
	else if (level <= 15)
		framesPerCell = 4;
	else if (level <= 18)
		framesPerCell = 3;
	else if (level <= 28)
		framesPerCell = 2;
	else
		framesPerCell = 1;

	return (double)60.0988 / (double)framesPerCell;

}

// Calculate how many more lines must be completed until the player reaches the next level
unsigned short calcLinesUntilLevelup(unsigned short linesAtCurrentLevel, unsigned short currentLevel)
{

	return LEVELUP_INCREASE * (currentLevel + 1) - linesAtCurrentLevel;

}

//Function for calculating the Y coord to display the "ghost" of the current piece at
unsigned short calcGhostY(piece* Piece, int X, int startY, int* mapData, int mapWidth, int mapHeight)
{

	double Y = startY;

	while (isColliding(*Piece, X, &Y, DOWN, mapData, mapWidth, mapHeight) == false)
		Y = Y + 1;

	return (unsigned short)Y;

}

//Function for playing a single frame of the overAnimation
bool playOverAnimation(SDL_Texture* foreground, unsigned short mapWidth, unsigned short mapHeight)
{

	//Return if the animation is done playing or not
	bool animationOver = false;

	//These are used to time the animation
	static Uint32* time_start = NULL;
	static Uint32* time_now = NULL;
	if (time_start == NULL)
	{

		//Allocate memory for storing time values
		time_start = SDL_malloc(sizeof(*time_start));
		time_now = SDL_malloc(sizeof(*time_now));
		if (time_start != NULL)
			*time_start = SDL_GetTicks();

	}
	else
	{

		if (time_now != NULL)
		{

			//Get the current time
			*time_now = SDL_GetTicks();

			//When enough time has lapsed since the last frame in the animation, play 
			//another frame
			if ((*time_now - *time_start) > (Uint32)(getAudioLengthInMS(globalInstance->sounds[OVER_SOUND]) * 0.75 / MAP_HEIGHT))
			{

				//row stores the y value of the current row that is being animated
				static unsigned short* row;
				if (row == NULL)
				{

					row = SDL_malloc(sizeof(*row));
					if (row != NULL)
						*row = 0;

				}
				
				//With every frame of animation, we draw a row of randomly colored BLOCKS to 
				//the screen We fill the playable area with these blocks one row at a time
				if (row != NULL)
				{

					//Fill the current row with randomly colored BLOCKSs
					for (unsigned short i = 0; i < mapWidth; i++)
						drawToTexture(BLOCK_SPRITE_ID, foreground, SPRITE_WIDTH * i, SPRITE_HEIGHT * *row, 1, (rand() % (RED - YELLOW + 1)) + YELLOW);

					//Check if we are on the last row
					if (*row < mapHeight)
						*row += 1;
					else
					{

						//If we are on the last row, the animation is over
						animationOver = true;
						
						//Free memory used to store the row 
						SDL_free(row);
						row = NULL;

					}

				}

				//Free time variables
				SDL_free(time_start);
				time_start = NULL;
				SDL_free(time_now);
				time_now = NULL;

			}

		}

	}

	//Return the status of whether the animation is finished playing or not
	return animationOver;

}

//Function for updating the texture that displays the number of lines needed to get to the next level
void updateLines(unsigned short lines, SDL_Texture** linesTexture)
{

	//Erase linesTexture
		//We need to delete it because its size may change with the new lines value
	clearTexture(*linesTexture);
	SDL_DestroyTexture(*linesTexture);

	//Recreate linesTexture
	*linesTexture = createTexture(getIntStringLength(lines, 1.0), FONT_HEIGHT);

	//Draw new lines to linesTexture
	intToTexture(lines, *linesTexture, 0, 0, 1, ORANGE);

}

//Function for updating the texture that displays the current level
void updateLevel(unsigned short level, SDL_Texture* levelTexture)
{

	//Clear level texture
	clearTexture(levelTexture);

	//Draw new level to levelTexture
	intToTexture(level, levelTexture, 0, 0, 1, RED);

}

//Function for updating the texture that displays the score
void updateScore(unsigned int score, SDL_Texture* scoreTexture)
{

	//Get the number of digits in the score
	unsigned short length = (unsigned short)(SDL_floor(SDL_log10(score)) + 1);

	//Clear area of scoreTexture that will be updated
	SDL_Rect rect;
	SDL_QueryTexture(scoreTexture, NULL, NULL, &rect.w, &rect.h);
	rect.x = rect.w - length * (FONT_WIDTH + STRING_GAP);
	rect.w = length * (FONT_WIDTH + STRING_GAP);
	rect.h = FONT_HEIGHT;
	rect.y = 0;
	SDL_SetRenderTarget(globalInstance->renderer, scoreTexture);
	SDL_SetRenderDrawColor(globalInstance->renderer, 0, 0, 0, 0);
	SDL_RenderFillRect(globalInstance->renderer, &rect);

	//Draw new score to scoreTexture
	intToTexture(score, scoreTexture, rect.x, rect.y, 1, WHITE);

	//Restore RenderTarget
	SDL_SetRenderTarget(globalInstance->renderer, NULL);

}

//Function for playing one frame of the line clearing animation
bool playLineAnimation(SDL_Texture* foreground, unsigned short row, bool *clearingLine, int *mapData, unsigned short* numCompleted)
{

	//This is the value that gets returned from the function and is used to tell if a sound should be played for the animation this frame
	bool playSound = false;

	//Allocate memory for storing some time values
	static Uint32* time_start = NULL;
	static Uint32* time_now = NULL;
	if (time_start == NULL)
	{

		time_start = SDL_malloc(sizeof(*time_start));
		time_now = SDL_malloc(sizeof(*time_now));
		if (time_start != NULL)
			*time_start = SDL_GetTicks();

	}
	else
	{

		if (time_now != NULL)
		{

			//Keep track of the current time for this framess
			*time_now = SDL_GetTicks();

			//If a certain amount of time has passed since the last time the animation was updated
			if ((*time_now - *time_start) > CLEARING_TIME)
			{

				//With each frame of the animation, remove two blocks from the line being erased
					//Here, frame refers to when the animation actually changes
				SDL_SetRenderTarget(globalInstance->renderer, foreground);

				//'column' stores the x value of the current column that is being erased
				static unsigned short* column;
				if (column == NULL)
				{

					column = SDL_malloc(sizeof(*column));
					*column = 0;

					//When 'column' == NULL, that means we are on a new line, so play a sound
					playSound = true;

				}
				else if (column != NULL)
				{

					//Because we remove a block on each side of the chasm
						//The blocks getting removed are +-column blocks away from the center
					drawToTexture(BLOCK_SPRITE_ID, foreground, (MAP_WIDTH / 2 - *column) * SPRITE_WIDTH, row * SPRITE_HEIGHT, 1, BLACK);
					drawToTexture(BLOCK_SPRITE_ID, foreground, (MAP_WIDTH / 2 + *column) * SPRITE_WIDTH, row * SPRITE_HEIGHT, 1, BLACK);

					//By increasing *column, we're basically increaing the radius of the 
					//chasm of blocks we are removing
					*column = *column + 1;

					//Once we have removed all the blocks in this row
					if (*column > MAP_WIDTH / 2)
					{

						SDL_free(column);
						column = NULL;

						*numCompleted = *numCompleted - 1;
						if (*numCompleted == 0)
							*clearingLine = false;

						removeLine(row, mapData, foreground, MAP_WIDTH);

					}

				}

				SDL_free(time_start);
				time_start = NULL;
				SDL_free(time_now);
				time_now = NULL;

				SDL_SetRenderTarget(globalInstance->renderer, NULL);

			}

		}

	}

	return playSound;

}

//Function for removing a line from the mapData and moving the lines above it down
void removeLine(unsigned short row, int* mapData, SDL_Texture* foreground, unsigned short mapWidth)
{

	//All blocks in memory for the clearedLine are set to empty
	for (unsigned short j = 0; j < mapWidth; j++)
		*(mapData + row * mapWidth + j) = 0;

	SDL_Rect srcRect = { .x = 0, .y = 0, .w = mapWidth * SPRITE_WIDTH, .h = row * SPRITE_HEIGHT };
	
	//Copy portion of the foreground above the clearedLine
	SDL_Texture* aboveText;
	aboveText = createTexture(srcRect.w, srcRect.h);
	SDL_SetRenderTarget(globalInstance->renderer, aboveText);
	SDL_RenderCopy(globalInstance->renderer, foreground, &srcRect, NULL);

	//Clear area of foreground above the clearedLine
	SDL_SetRenderTarget(globalInstance->renderer, foreground);
	SDL_SetRenderDrawColor(globalInstance->renderer, 0, 0, 0, 0);
	SDL_RenderFillRect(globalInstance->renderer, &srcRect);

	SDL_Rect dstRect = { .x = 0, .y = SPRITE_HEIGHT, .w = srcRect.w, .h = srcRect.h };

	//Copy the texture from above the clearedLine down by 1 row
	SDL_RenderCopy(globalInstance->renderer, aboveText, NULL, &dstRect);
	SDL_SetRenderTarget(globalInstance->renderer, NULL);

	//Shift all mapData above clearedLine down by 1
	for (unsigned short i = row; i > 0; i--)
		for (unsigned short j = 0; j < mapWidth; j++)
			*(mapData + i * mapWidth + j) = *(mapData + (i - 1) * mapWidth + j);

}

//Functiom for checking if a given row is completely filled
bool lineIsComplete(int* mapData, unsigned short row, unsigned short mapWidth)
{

	//Just go through every block in the given row. If any of them are not filled, return false
	for (unsigned short j = 0; j < mapWidth; j++)
		if (*(mapData + row * mapWidth + j) == 0)
			return false;

	//Otherwise, the line must be totally filled and thus complete, so return true
	return true;

}

//This function gets both the number of lines that have just been completed, as well as 
//the row numbers of each one that was completed
unsigned short completedLine(int* mapData, int Y, piece Piece, int** returnRows, int mapWidth, int mapHeight)
{

	//Duct-tape fix for a rare glitch that can sometimes happen when holding near the top
	//of the play area
		//Basically, if Y is out-of-bounds, just move it back in bounds.
	if (Y > mapHeight)
		Y = mapHeight;

	//Keep track of the number of lines that were completed by the last placement
	unsigned short numCompleted = 0;

	//Count the number of lines completed
	for (unsigned short i = 0; i < Piece.height; i++)
		if (lineIsComplete(mapData, Y + i, mapWidth) == true)
			numCompleted = numCompleted + 1;

	if (numCompleted > 0)
	{

		SDL_free(*returnRows);
		*returnRows = SDL_malloc(numCompleted * sizeof(**returnRows));

		//Get the row numbers of each row completed
		unsigned short currentReturnRow = 0;
		for (unsigned short i = 0; i < Piece.height; i++)
		{

			if (lineIsComplete(mapData, Y + i, mapWidth) == true)
			{

				*(*returnRows + currentReturnRow) = Y + i;

				currentReturnRow = currentReturnRow + 1;

			}

		}

	}

	return numCompleted;

}

//This prevents pieces from spawning partially in the wall
void adjustNewPiece(piece* Piece, signed short* X, unsigned short mapWidth)
{

	if (*X + Piece->width > mapWidth)
		*X = *X - ((*X + Piece->width) - mapWidth);

}

//Check if a piece is colliding with another piece in various circumstances
bool isColliding(piece Piece, int X, double* Y, int direction, int* mapData, int mapWidth, int mapHeight)
{

	//A piece is considered colliding if it sticks outside the bounds of the playable area
	if (X < 0 || X + Piece.width > mapWidth)
		return true;
	else if (*Y < 0)
		return true;

	if (direction == DOWN)
	{

		//Here, the piece is trying to move DOWN.
			//We allow it to move down, and then check if any blocks of it are already occupied in
			//map Data
		for (unsigned short i = 0; i < Piece.numOfBlocks; i++)
			if (*(mapData + ((int)*Y + (Piece.blocks[i].Y - Piece.minY))
				* mapWidth + (X + (Piece.blocks[i].X - Piece.minX))) != 0 ||
				(int)*Y + Piece.height > mapHeight)
			{

				//If one of the block is already occupied in mapData, move the piece back up
				*Y = *Y - 1;
				return true;

			}

	}
	else if (direction == LEFT)
	{

		//Check any blocks in mapData exist to the LEFT of any blocks in the currentPiece
		for (unsigned short i = 0; i < Piece.numOfBlocks; i++)
			if (*(mapData + ((int)*Y + (Piece.blocks[i].Y - Piece.minY))
				* mapWidth + (X + (Piece.blocks[i].X - Piece.minX) - 1)) != 0)
				return true;

	}
	else if (direction == RIGHT)
	{

		//Check any blocks in mapData exist to the RIGHT of any blocks in the currentPiece
		for (unsigned short i = 0; i < Piece.numOfBlocks; i++)
			if (*(mapData + ((int)*Y + (Piece.blocks[i].Y - Piece.minY))
				* mapWidth + (X + (Piece.blocks[i].X - Piece.minX) + 1)) != 0)
				return true;

	}
	else if (direction == NONE)
	{

		//Check any blocks in mapData exist exactly where currentPiece is.
			//currentPiece is not trying to move in this instance
		for (unsigned short i = 0; i < Piece.numOfBlocks; i++)
			if (*(mapData + ((int)*Y + (Piece.blocks[i].Y - Piece.minY))
				* mapWidth + (X + (Piece.blocks[i].X - Piece.minX))) != 0)
				return true;

	}

	return false;

}

//Function for moving a piece left and right
void move(char keyPress, signed short *X, piece Piece, unsigned short mapWidth)
{
	
	//Only allow movement within bounds
	if (keyPress == LEFT)
	{

		if (*X > 0)
			*X = *X - 1;

	}
	else if (keyPress == RIGHT)
	{

		if (*X + Piece.width < mapWidth)
			*X = *X + 1;

	}

}