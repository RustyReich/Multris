#include "HEADERS/MGF.h"

//generate.c
piece* generateGamePiece(unsigned short size);
void delPiece(piece** Piece);
void copyPiece(piece* piece1, piece* piece2);

//draw.c
void drawPiece(piece Piece, SDL_Texture* dstTexture, unsigned short X, unsigned short Y);
void drawTexture(SDL_Texture* texture, int X, int Y, float multiplier);
void intToTexture(int num, SDL_Texture* dstTexture, int X, int Y, float multiplier, unsigned short color);
void printToTexture(char* string,SDL_Texture* dstTexture,int X,int Y,float multiplier,unsigned short color);
SDL_Texture* createTexture(int width, int height);
void drawToTexture(int SpriteID,SDL_Texture* dstTexture,int X,int Y,float multiplier,Uint8 color);
void clearTexture(SDL_Texture* texture);
SDL_Texture* createPieceTexture(piece Piece, bool drawCenterDot);
int getStringLength(char* str, float multiplier);
int getIntStringLength(int num, float multiplier);
void drawSimpleRect(SDL_Texture* dstTexture, int x, int y, int width, int height, int color);

//rotate.c
void rotatePiece(piece *Piece, unsigned short direction);
void mirrorPieceOverY(piece* Piece);

//audio.c
void _playSound(int id);
int getAudioLengthInMS(sound* Sound);

//file.c
void saveTop(unsigned int score, unsigned short size, bool inCustomMode);
unsigned int loadTop(unsigned short size, bool inCustomMode);
unsigned int loadProgress();
void saveProgress();

//memory.c
void freeVars();
void declare_unsigned_short(void** ptr, unsigned short value);
void declare_double(void** ptr, double value);
void declare_int(void** ptr, int value);
void declare_char(void** ptr, char value);
void declare_bool(void** ptr, bool value);
void declare_unsigned_int(void** ptr, unsigned int value);
void declare_short(void** ptr, short value);
void declare_Piece(piece** ptr, piece* Piece);
void declare_Piece_Text(SDL_Texture** ptr, piece* Piece, bool drawCenterDot);
void declare_HUD_Text(SDL_Texture** ptr, int type);
void declare_map_matrix(bool** ptr);
void declare_size_bag(SizeBag** ptr, unsigned short mode, bool customMode);

//layout.c
int getScoreDrawX(unsigned short size);
int getScoreDrawY(unsigned short size);
int getForegroundX(unsigned short size);
int getForegroundY(unsigned short size);
int getLevelX(unsigned short size, unsigned short level);
int getLevelY(unsigned short size);
int getLinesX(unsigned short size, unsigned short lines);
int getLinesY(unsigned short size);
int getNextX(unsigned short size, int width);
int getNextY(unsigned short size, int height);
int getHoldX(unsigned short size, int width);
int getHoldY(unsigned short size, int height);
int getPausedX(unsigned short size, float multi);
int getPausedY(unsigned short size, float multi);

void move(char keyPress, signed short *X, piece Piece, unsigned short mapWidth);
bool isColliding(piece Piece, int X, double* Y, int direction, bool* mapData, int mapWidth, int mapHeight);
void adjustNewPiece(piece* Piece, signed short* X, unsigned short mapWidth);
unsigned short completedLine(bool* mapData, int Y, piece Piece, int** returnRows, int mapWidth, int mapHeight);
bool lineIsComplete(bool* mapData, unsigned short row, unsigned short mapWidth);
void removeLine(unsigned short row, bool* mapData, SDL_Texture* foreground, unsigned short mapWidth);
bool playLineAnimation(SDL_Texture* foreground, unsigned short row, bool *clearingLine, bool *mapData, unsigned short* numCompleted);
void updateScore(unsigned int, SDL_Texture*);
void updateLevel(unsigned short, SDL_Texture*);
void updateLines(unsigned short lines, SDL_Texture** linesTexture);
bool playOverAnimation(SDL_Texture* foreground, unsigned short mapWidth, unsigned short mapHeight);
unsigned short calcGhostY(piece* Piece, int X, int startY, bool* mapData, int mapWidth, int mapHeight);
unsigned short calcLinesUntilLevelup(unsigned short linesAtCurrentLevel, unsigned short currentLevel);
double calcSpeed(unsigned short level);
void removeSizeFromBag(SizeBag* sizeBag, unsigned short size, unsigned short mode, bool customMode);

unsigned short playMode(piece* firstPiece)
{

	//Game pieces
	static piece* currentPiece; declare_Piece(&currentPiece, firstPiece);
	static piece* holdPiece; declare_Piece(&holdPiece, NULL);
	static piece* nextPiece; declare_Piece(&nextPiece, NULL);
	
	//Variables
	DECLARE_VARIABLE(signed short, X, 0);
	DECLARE_VARIABLE(double, Y, 0.0);
	DECLARE_VARIABLE(double, speed, INITIAL_SPEED);
	DECLARE_VARIABLE(bool, softDrop, false);
	DECLARE_VARIABLE(bool, gameOver, false);
	DECLARE_VARIABLE(bool, clearingLine, false);
	DECLARE_VARIABLE(bool, paused, false);
	DECLARE_VARIABLE(bool, overAnimation, false);
	DECLARE_VARIABLE(bool, justHeld, false);
	DECLARE_VARIABLE(unsigned short, numCompleted, 0);
	DECLARE_VARIABLE(unsigned int, Score, 0);
	DECLARE_VARIABLE(short, Level, 0);
	DECLARE_VARIABLE(short, linesAtCurrentLevel, 0);
	DECLARE_VARIABLE(short, ghostY, MAP_HEIGHT - currentPiece->height);
	DECLARE_VARIABLE(unsigned int, moveStart, 0);
	DECLARE_VARIABLE(bool, moveStartBool, false);
	DECLARE_VARIABLE(int, nextText_Width, 0);
	DECLARE_VARIABLE(int, nextText_Height, 0);
	DECLARE_VARIABLE(int, holdText_Width, 0);
	DECLARE_VARIABLE(int, holdText_Height, 0);
	DECLARE_VARIABLE(bool, firstLoop, true);
	DECLARE_VARIABLE(int, foregroundX, 0);
	DECLARE_VARIABLE(int, foregroundY, 0);
	DECLARE_VARIABLE(double, pausedMulti, 1.0);
	DECLARE_VARIABLE(bool, playing_progress_sound, false);
	DECLARE_VARIABLE(unsigned int, progress_sound_start, 0);
	DECLARE_VARIABLE(int, length_of_progress_sound, 0);

	//Texutures
	static SDL_Texture* Texture_Current; declare_Piece_Text(&Texture_Current, currentPiece, true);
	static SDL_Texture* Texture_Ghost; declare_Piece_Text(&Texture_Ghost, currentPiece, false);
	static SDL_Texture* Texture_Next; declare_Piece_Text(&Texture_Next, nextPiece, false);
	static SDL_Texture* Texture_Hold; declare_Piece_Text(&Texture_Hold, holdPiece, false);
	static SDL_Texture* Texture_Score; declare_HUD_Text(&Texture_Score, SCORE_TEXT);
	static SDL_Texture* Texture_Level; declare_HUD_Text(&Texture_Level, LEVEL_TEXT);
	static SDL_Texture* Texture_Lines; declare_HUD_Text(&Texture_Lines, LINES_TEXT);
	static SDL_Texture* Texture_Paused; declare_HUD_Text(&Texture_Paused, PAUSED_TEXT);
	static SDL_Texture* foreground; declare_HUD_Text(&foreground, FOREGROUND_TEXT);

	//Arrays
		//Initialize completedRows to only include a row that is offscreen
	static int* completedRows; declareStart(completedRows, MAP_HEIGHT + 1);
	static bool* mapData; declare_map_matrix(&mapData);

	// Declare the bag holding the possible sizes for bag shuffling of sizes
	static SizeBag* sizeBag; declare_size_bag(&sizeBag, MODE, CUSTOM_MODE);
	
	//First frame
	if (*firstLoop)
	{

		//Get X and Y for foreground
		*foregroundX = getForegroundX(MODE);
		*foregroundY = getForegroundY(MODE);

		//Calculate multiplier for PAUSED text when it would be wider than the playfield
		if (MODE != 0)
			if (getStringLength("PAUSED", 1.0) > SDL_round(BASE_PLAYFIELD_WIDTH * MODE) * SPRITE_WIDTH)
				*pausedMulti = SDL_round(BASE_PLAYFIELD_WIDTH * MODE) * SPRITE_WIDTH / getStringLength("PAUSED", 1.0);

		// Calc speed at start incase want to start at different level (for debugging)
		*speed = calcSpeed(*Level);

		// Remove the size of the firstPiece from the sizeBag
		removeSizeFromBag(sizeBag, firstPiece->numOfBlocks, MODE, CUSTOM_MODE);

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
					else if (*(mapData + ((int)*Y + (blockY - currentPiece->minY)) * MAP_WIDTH + (*X + (blockX - currentPiece->minX))))
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

			playSound(PAUSE_SOUND);
			*paused = !*paused;

		}
		// Also pause if window loses focus
		if (!(SDL_GetWindowFlags(globalInstance->window) & SDL_WINDOW_INPUT_FOCUS) && !*paused)
		{
		
			playSound(PAUSE_SOUND);
			*paused = true;

		}

		// Exit if press ESCAPE_BUTTON while paused
		if (*paused && onPress(EXIT_BUTTON))
		{

			freeVars();
			return RESET;

		}

	}

	//-----------------------------------------------------------------------

	//LOGIC ------------------------------------------------------------------------------

	//If nextPiece does not exist, generate a new one
	if (nextPiece == NULL)
	{

		// Pick random size from the sizeBag to generate next piece
		nextPiece = generateGamePiece(sizeBag->sizesInBag[rand() % sizeBag->size]);

		/*
		if (MODE == 0)
			nextPiece = generateGamePiece(rand() % MAX_PIECE_SIZE + 1);
		else if (CUSTOM_MODE == true)
			nextPiece = generateGamePiece(rand() % MODE + 1);
		else
			nextPiece = generateGamePiece(MODE);
		*/

		//And create a new texture for it as well
		if (Texture_Next == NULL)
		{
			
			Texture_Next = createPieceTexture(*nextPiece, false);
			SDL_QueryTexture(Texture_Next, NULL, NULL, nextText_Width, nextText_Height);

		}

		// Remove size of nextPiece from sizeBag
		removeSizeFromBag(sizeBag, nextPiece->numOfBlocks, MODE, CUSTOM_MODE);

	}

	//Gravity
	if (currentPiece != NULL && !*gameOver && !*paused)
	{
		
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

			}

		}
		else     //Piece is placed
		{

			playSound(LAND_SOUND);

			drawPiece(*currentPiece, foreground, *X * SPRITE_WIDTH, (int)*Y * SPRITE_HEIGHT);

			//Store piece in mapData
			for (unsigned short i = 0; i < currentPiece->numOfBlocks; i++) {

				int row = (int)*Y + (currentPiece->blocks[i].Y - currentPiece->minY);
				int column = *X + (currentPiece->blocks[i].X - currentPiece->minX);

				*(mapData + row * MAP_WIDTH + column) = true;

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

				//Keep track of the number of lines that have been cleared on the current 
				//level
				*linesAtCurrentLevel += *numCompleted;
				if (*linesAtCurrentLevel >= 5 * (*Level + 1))
				{
					
					//If the player completes more lines than is needed to level up, the completed lines will
					//carry-over between levels
					*linesAtCurrentLevel = *linesAtCurrentLevel - 5 * (*Level + 1);

					//Increase level count and update Texture_Level
					*Level += 1;
					updateLevel(*Level, Texture_Level);

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

	//Draw the foreground
	if (CUSTOM_MODE == false || MODE < MAX_PIECE_SIZE)
		drawTexture(foreground, *foregroundX, *foregroundY, 1.0);
	else	// In SIZE > MAX_PIECE_SIZE, the foreground is rendered smaller to fit within the same sized play area as MULTRIS mode.
		drawTexture(foreground, *foregroundX, *foregroundY, (float)MAX_PIECE_SIZE / (float)MODE);

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

		}

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

// Function for removing a size from the sizeBag
	// Also resets the sizeBag if the last size is removed
void removeSizeFromBag(SizeBag* sizeBag, unsigned short size, unsigned short mode, bool customMode)
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

	}
	else if (sizeBag->size == 1)
	{

		if (mode == 0)
		{

			// Reset sizeBag to have all sizes up to MAX_PIECE_SIZE
			unsigned short* newSizesInBag = SDL_calloc(MAX_PIECE_SIZE, sizeof(unsigned short));
			for (unsigned short i = 0; i < MAX_PIECE_SIZE; i++)
				newSizesInBag[i] = i + 1;

			// Then detel the old array, and update sizeBag to point to the new array
			SDL_free(sizeBag->sizesInBag);
			sizeBag->sizesInBag = newSizesInBag;

			// And reset the size of the bag to MAX_PIECE_SIZE
			sizeBag->size = MAX_PIECE_SIZE;

		}
		else if (customMode == true)
		{

			// Reset sizeBag to have all sizes up to MODE
			unsigned short* newSizesInBag = SDL_calloc(mode, sizeof(unsigned short));
			for (unsigned short i = 0; i < mode; i++)
				newSizesInBag[i] = i + 1;

			// Then detel the old array, and update sizeBag to point to the new array
			SDL_free(sizeBag->sizesInBag);
			sizeBag->sizesInBag = newSizesInBag;

			// And reset the size of the bag to MODE
			sizeBag->size = mode;

		}

	}

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

	return 5 * (currentLevel + 1) - linesAtCurrentLevel;

}

//Function for calculating the Y coord to display the "ghost" of the current piece at
unsigned short calcGhostY(piece* Piece, int X, int startY, bool* mapData, int mapWidth, int mapHeight)
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
			if ((*time_now - *time_start) > (Uint32)(OVER_SOUND_LENGTH / MAP_HEIGHT))
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
bool playLineAnimation(SDL_Texture* foreground, unsigned short row, bool *clearingLine, bool *mapData, unsigned short* numCompleted)
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
void removeLine(unsigned short row, bool* mapData, SDL_Texture* foreground, unsigned short mapWidth)
{

	//All blocks in memory for the clearedLine are set to empty
	for (unsigned short j = 0; j < mapWidth; j++)
		*(mapData + row * mapWidth + j) = false;

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
bool lineIsComplete(bool* mapData, unsigned short row, unsigned short mapWidth)
{

	//Just go through every block in the given row. If any of them are not filled, return false
	for (unsigned short j = 0; j < mapWidth; j++)
		if (*(mapData + row * mapWidth + j) == false)
			return false;

	//Otherwise, the line must be totally filled and thus complete, so return true
	return true;

}

//This function gets both the number of lines that have just been completed, as well as 
//the row numbers of each one that was completed
unsigned short completedLine(bool* mapData, int Y, piece Piece, int** returnRows, int mapWidth, int mapHeight)
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
bool isColliding(piece Piece, int X, double* Y, int direction, bool* mapData, int mapWidth, int mapHeight)
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
				* mapWidth + (X + (Piece.blocks[i].X - Piece.minX))) == true ||
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
				* mapWidth + (X + (Piece.blocks[i].X - Piece.minX) - 1)) == true)
				return true;

	}
	else if (direction == RIGHT)
	{

		//Check any blocks in mapData exist to the RIGHT of any blocks in the currentPiece
		for (unsigned short i = 0; i < Piece.numOfBlocks; i++)
			if (*(mapData + ((int)*Y + (Piece.blocks[i].Y - Piece.minY))
				* mapWidth + (X + (Piece.blocks[i].X - Piece.minX) + 1)) == true)
				return true;

	}
	else if (direction == NONE)
	{

		//Check any blocks in mapData exist exactly where currentPiece is.
			//currentPiece is not trying to move in this instance
		for (unsigned short i = 0; i < Piece.numOfBlocks; i++)
			if (*(mapData + ((int)*Y + (Piece.blocks[i].Y - Piece.minY))
				* mapWidth + (X + (Piece.blocks[i].X - Piece.minX))) == true)
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