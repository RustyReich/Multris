#include <stdbool.h>
#include <math.h>

#include "HEADERS/MGF.h"

//generate.c
piece* generateGamePiece(unsigned short size);
void delPiece(piece** Piece);
void copyPiece(piece* piece1, piece* piece2);

//draw.c
void drawPiece(piece Piece, SDL_Texture* dstTexture, unsigned short X, 
	unsigned short Y);
void drawTexture(SDL_Texture* texture, int X, int Y, float multiplier);
void intToTexture(int num, SDL_Texture* dstTexture, int X, int Y, 
	float multiplier, unsigned short color);
void printToTexture(char* string, SDL_Texture* dstTexture, int X, int Y, 
	float multiplier, unsigned short color);
unsigned short getIntLength(int num);
SDL_Texture* createTexture(int width, int height);
void drawToTexture(unsigned int SpriteID, SDL_Texture* dstTexture, int X, int Y, 
	float multiplier, Uint8 color);
void clearTexture(SDL_Texture* texture);
SDL_Texture* createPieceTexture(piece Piece);
int getStringLength(char* str, float multiplier);
int getIntStringLength(int num, float multiplier);

//rotate.c
void rotatePiece(piece *Piece, unsigned short direction);
void mirrorPieceOverY(piece* Piece);

//audio.c
void playSound(sound* Sound);

//file.c
void saveTop(unsigned int score);
unsigned int loadTop();
unsigned short getOption(unsigned short line);

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

piece* getFirstPiece(piece*);
void move(char keyPress, unsigned short *X, piece Piece, unsigned short mapWidth);
bool isColliding(piece Piece, unsigned short X, double* Y, unsigned short direction, 
					bool* mapData, unsigned short mapWidth, unsigned short mapHeight);
void adjustNewPiece(piece* Piece, unsigned short* X, unsigned short mapWidth);
unsigned short completedLine(bool* mapData, unsigned short Y, piece Piece, 
								unsigned short** returnRows, unsigned short mapWidth,
								unsigned short mapHeight);
bool lineIsComplete(bool* mapData, unsigned short row, unsigned short mapWidth);
void removeLine(unsigned short row, bool* mapData, SDL_Texture* foreground,
				unsigned short mapWidth);
bool playLineAnimation(SDL_Texture* foreground, unsigned short row, 
	bool *clearingLine, bool *mapData, unsigned short* numCompleted, unsigned short mapWidth,
	unsigned short mapHeight);
void updateScore(unsigned int, SDL_Texture*);
void updateLevel(unsigned short, SDL_Texture*);
void updateLines(unsigned short lines, SDL_Texture** linesTexture);
bool playOverAnimation(SDL_Texture* foreground, unsigned int score, unsigned short mapWidth,
						unsigned short mapHeight);
unsigned short calcGhostY(piece* Piece, unsigned short X, unsigned short startY, 
							bool* mapData, unsigned short mapWidth, unsigned short mapHeight);

unsigned short playMode(piece* firstPiece, unsigned short mode)
{

	//Calc mapWidth and mapHeight
	static unsigned short* mapWidth;
	static unsigned short* mapHeight;
	if (mode == 0)
	{

		declare(mapWidth, (int)round(BASE_PLAYFIELD_WIDTH * MAX_PIECE_SIZE));
		declare(mapHeight, (int)round(BASE_PLAYFIELD_HEIGHT * MAX_PIECE_SIZE));

	}
	else
	{

		declare(mapWidth, (int)round(BASE_PLAYFIELD_WIDTH * mode));
		declare(mapHeight, (int)round(BASE_PLAYFIELD_HEIGHT * mode));

	}

	//Game pieces
	static piece* currentPiece; declare_Piece(&currentPiece, firstPiece);
	static piece* holdPiece; declare_Piece(&holdPiece, NULL);
	static piece* nextPiece; declare_Piece(&nextPiece, NULL);
	
	//Variables
	static unsigned short* X; declare(X, 0);
	static double *Y; declare(Y, 0.0);
	static double *speed; declare(speed, INITIAL_SPEED);
	static bool *softDrop; declare(softDrop, false);
	static bool* gameOver; declare(gameOver, false);
	static bool* clearingLine; declare(clearingLine, false);
	static bool* paused; declare(paused, false);
	static bool* overAnimation; declare(overAnimation, false);
	static bool* justHeld; declare(justHeld, false);
	static unsigned short* numCompleted; declare(numCompleted, 0);
	static unsigned int* Score; declare(Score, 0);
	static unsigned short* Level; declare(Level, 0);
	static unsigned short* linesAtCurrentLevel; declare(linesAtCurrentLevel, 0);
	static bool* ghostEnabled; declare(ghostEnabled, GHOST_MODE_ENABLED);
	static unsigned short* ghostY; declare(ghostY, *mapHeight - currentPiece->height);
	static Uint32* moveStart; declare(moveStart, 0);
	static bool* moveStartBool; declare(moveStartBool, false);
	static int* nextText_Width; declare(nextText_Width, 0);
	static int* nextText_Height; declare(nextText_Height, 0);
	static int* holdText_Width; declare(holdText_Width, 0);
	static int* holdText_Height; declare(holdText_Height, 0);

		//Texutures
	static SDL_Texture* Texture_Current; declare_Piece_Text(&Texture_Current, currentPiece);
	static SDL_Texture* Texture_Next; declare_Piece_Text(&Texture_Next, nextPiece);
	static SDL_Texture* Texture_Hold; declare_Piece_Text(&Texture_Hold, holdPiece);
	static SDL_Texture* Texture_Score;
	static SDL_Texture* Texture_Level;
	static SDL_Texture* Texture_Lines;
	static SDL_Texture* Texture_Paused;
	static SDL_Texture* foreground;
	if (Texture_Score == NULL)
	{

		Texture_Score = createTexture(6 * (FONT_WIDTH + STRING_GAP), 
			FONT_HEIGHT);

		printToTexture("000000", Texture_Score, 0, 0, 1, WHITE);

	}
	if (Texture_Level == NULL)
	{

		Texture_Level = createTexture(2 * (FONT_WIDTH + STRING_GAP), 
			FONT_HEIGHT);

		printToTexture("0", Texture_Level, 0, 0, 1, RED);

	}
	if (Texture_Lines == NULL)
	{

		Texture_Lines = createTexture(FONT_WIDTH, FONT_HEIGHT);
		printToTexture("5", Texture_Lines, 0, 0, 1, ORANGE);

	}
	if (Texture_Paused == NULL)
	{

		Texture_Paused = createTexture(6 * (FONT_WIDTH + STRING_GAP), 
			FONT_HEIGHT);
		printToTexture("PAUSED", Texture_Paused, 0, 0, 1, WHITE);

	}
	if (foreground == NULL)
	{
		
		foreground = createTexture(FONT_WIDTH * *mapWidth, FONT_HEIGHT * *mapHeight);

	}

	//Arrays
	static unsigned short* completedRows;
	//map data matrix
	static bool* mapData;
	if (mapData == NULL)
	{

		mapData = 
			(bool*)malloc(*mapHeight * *mapWidth * sizeof(*mapData));
		if (mapData != NULL)
			for (unsigned short i = 0; i < *mapHeight; i++)
				for (unsigned short j = 0; j < *mapWidth; j++)
					*(mapData + i * *mapWidth + j) = false;

	}

	//CONTROLS ---------------------------------------------------------------

	//These controls only work if it is not a game over and the game is not paused
	if (!*gameOver && !*paused)
	{

		//Left and right movement
		if (globalInstance->controls[LEFT_BUTTON_ID].onHold || 
			globalInstance->controls[RIGHT_BUTTON_ID].onHold)
		{

			Uint32 currTicks = SDL_GetTicks();

			//Start the counter for holding the movement keys
			if (*moveStart == 0)
			{

				*moveStart = currTicks;
				*moveStartBool = true;

			}

			if (*moveStartBool || (currTicks - *moveStart) >= 
				(MOVEMENT_WAIT + MOVEMENT_TIME))
			{

				if (globalInstance->controls[LEFT_BUTTON_ID].onHold && 
					!isColliding(*currentPiece, *X, Y, LEFT, mapData, *mapWidth, *mapHeight))
				{

					//Only play sound if can actually move
					if (*X != 0)
						playSound(globalInstance->sounds[MOVE_SOUND_ID]);
					move(LEFT, X, *currentPiece, *mapWidth);

					//Recalculate ghostY
					*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, 
											mapData, *mapWidth, *mapHeight);

				}
				else if (globalInstance->controls[RIGHT_BUTTON_ID].onHold && 
							!isColliding(*currentPiece, *X, Y, RIGHT, mapData, *mapWidth,
											*mapHeight))
				{

					if (*X + currentPiece->width < *mapWidth)
						playSound(globalInstance->sounds[MOVE_SOUND_ID]);
					move(RIGHT, X, *currentPiece, *mapWidth);

					//Recalculate ghostY
					*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, 
											mapData, *mapWidth, *mapHeight);

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

		//Rotate CCW
		if (globalInstance->controls[ROTATE_CCW_BUTTON_ID].onPress && 
			*X + currentPiece->height <= *mapWidth &&
			*Y + currentPiece->width <= *mapHeight)
		{

			rotatePiece(currentPiece, CCW);
			//If rotation puts the piece inside another piece, just rotate back the 
			//opposite way
			if (isColliding(*currentPiece, *X, Y, NONE, mapData, *mapWidth, *mapHeight))
				rotatePiece(currentPiece, CW);
			else
			{

				//Only play sound if it actually rotated
				playSound(globalInstance->sounds[ROTATE_SOUND_ID]);
				SDL_DestroyTexture(Texture_Current);
				Texture_Current = NULL;

				//Recalculate ghostY
				*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData, *mapWidth,
										*mapHeight);

			}

		}

		//Rotate CW
		if (globalInstance->controls[ROTATE_CW_BUTTON_ID].onPress && 
			*X + currentPiece->height <= *mapWidth && *Y + currentPiece->width <= *mapHeight)
		{

			rotatePiece(currentPiece, CW);
			if (isColliding(*currentPiece, *X, Y, NONE, mapData, *mapWidth, *mapHeight))
				rotatePiece(currentPiece, CCW);
			else
			{

				playSound(globalInstance->sounds[ROTATE_SOUND_ID]);
				SDL_DestroyTexture(Texture_Current);
				Texture_Current = NULL;

				//Recalculate ghostY
				*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData, *mapWidth,
										*mapHeight);

			}

		}

		//Mirroring
		if (globalInstance->controls[MIRROR_BUTTON_ID].onPress)
		{

			mirrorPieceOverY(currentPiece);
			//If mirroring puts the piece inside another piece, just mirror it back
			if (isColliding(*currentPiece, *X, Y, NONE, mapData, *mapWidth, *mapHeight))
				mirrorPieceOverY(currentPiece);
			else
			{

				//Only play sound if actually mirrored
				playSound(globalInstance->sounds[ROTATE_SOUND_ID]);
				SDL_DestroyTexture(Texture_Current);
				Texture_Current = NULL;

				*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData, *mapWidth,
										*mapHeight);

			}

		}

		//Holding
		if (globalInstance->controls[HOLD_BUTTON_ID].onPress && !*justHeld)
		{

			playSound(globalInstance->sounds[HOLD_SOUND_ID]);

			if (holdPiece == NULL)
			{

				//Create holdPiece by copying currentPiece
				holdPiece = malloc(sizeof(*holdPiece));
				holdPiece->blocks = 
					malloc(currentPiece->numOfBlocks * sizeof(*holdPiece->blocks));
				copyPiece(currentPiece, holdPiece);
				Texture_Hold = createPieceTexture(*holdPiece);
				SDL_QueryTexture(Texture_Hold, NULL, NULL, holdText_Width, holdText_Height);

				//Delete currentPiece
				delPiece(&currentPiece);
				SDL_DestroyTexture(Texture_Current);
				Texture_Current = NULL;

				//Move nextPiece to currentPiece
				currentPiece = nextPiece;
				adjustNewPiece(currentPiece, X, *mapWidth);

				//NextPiece no longer exists
				nextPiece = NULL;
				//Destory nextPiece
				SDL_DestroyTexture(Texture_Next);
				Texture_Next = NULL;

				//Reset Y
				*Y = 0;

				//Recalculate ghostY
				*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData, *mapWidth,
										*mapHeight);

			}
			else //Swap holdPiece and currentPiece
			{

				//Store current holdPiece in tempPeiece
				piece* tempPiece;
				tempPiece = malloc(sizeof(*tempPiece));
				tempPiece->blocks = 
					malloc(holdPiece->numOfBlocks * sizeof(*tempPiece->blocks));
				copyPiece(holdPiece, tempPiece);

				//Recreate holdPiece
				delPiece(&holdPiece);
				SDL_DestroyTexture(Texture_Hold);
				holdPiece = malloc(sizeof(*holdPiece));
				holdPiece->blocks = 
					malloc(currentPiece->numOfBlocks * sizeof(*holdPiece->blocks));
				copyPiece(currentPiece, holdPiece);
				Texture_Hold = createPieceTexture(*holdPiece);
				SDL_QueryTexture(Texture_Hold, NULL, NULL, holdText_Width, holdText_Height);

				//Delete currentPiece
				delPiece(&currentPiece);
				SDL_DestroyTexture(Texture_Current);
				Texture_Current = NULL;

				//Copy tempPiece to currentPiece
				currentPiece = malloc(sizeof(*currentPiece));
				currentPiece->blocks = 
					malloc(tempPiece->numOfBlocks * sizeof(*tempPiece->blocks));
				copyPiece(tempPiece, currentPiece);
				adjustNewPiece(currentPiece, X, *mapWidth);

				//Delete tempPeice
				delPiece(&tempPiece);

				//Reset Y
				*Y = 0;

				//Recalculate ghostY
				*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData, *mapWidth,
										*mapHeight);

			}

			*justHeld = true;

		}

		//Hard drop
		if (globalInstance->controls[HARD_DROP_BUTTON_ID].onPress)
			*Y = *ghostY + 1;
			
		//Soft drop
		if (globalInstance->controls[SOFT_DROP_BUTTON_ID].onHold)
			*softDrop = true;
		else
			*softDrop = false;

	}
	
	if (!*gameOver)	//These controls work even when paused, as long as the game is
	{				//not over

		//Pausing
		if (globalInstance->controls[SELECT_BUTTON_ID].onPress)
		{

			playSound(globalInstance->sounds[PAUSE_SOUND_ID]);
			*paused = !*paused;

		}

	}

	//-----------------------------------------------------------------------

	// RENDERING -------------------------------------------------------------

	//Draw various textures that don't move
	drawTexture(Texture_Next, 
				318 - (*nextText_Width / 2),
				282 - (*nextText_Height / 2), 
				1.0);
	drawTexture(Texture_Hold,
				318 - (*holdText_Width * HOLD_TEXTURE_MULTI / 2), 
				403 - (*holdText_Height * HOLD_TEXTURE_MULTI / 2), 
				HOLD_TEXTURE_MULTI);
	drawTexture(Texture_Score, 
				SPRITE_WIDTH * (round(BASE_PLAYFIELD_WIDTH * MAX_PIECE_SIZE) + 1) +
				FONT_WIDTH + 0.5 * (SPRITE_WIDTH * 9 - getStringLength("000000", 1.0)), 
				FONT_HEIGHT * 2 + (FONT_WIDTH + STRING_GAP) * 4 - 10, 1.0);
	drawTexture(Texture_Level, 
				SPRITE_WIDTH * (round(BASE_PLAYFIELD_WIDTH * MAX_PIECE_SIZE) + 1) +
				SPRITE_WIDTH + 0.5 * (SPRITE_WIDTH * 9 - getStringLength("0", 1.0)), 
				FONT_HEIGHT * 9 + (SPRITE_HEIGHT + STRING_GAP) - 7, 1.0);
	drawTexture(Texture_Lines, 
				318 - getIntStringLength(*linesAtCurrentLevel, 1.0) / 2, 
				189, 
				1.0);

	//Draw the foreground
	drawTexture(foreground, FONT_WIDTH, FONT_HEIGHT, 1.0);

	//Draw ghost if enabled
	if (*ghostEnabled)
	{

		//Make Texture_Current opaque
		SDL_SetTextureAlphaMod(Texture_Current, 255 / 3);
		//Draw Texture_Current at ghostY
		drawTexture(Texture_Current, FONT_WIDTH * (*X + 1), 
			FONT_HEIGHT * ((int)*ghostY + 1), 1.0);
		//Reset Texture_Current opacity
		SDL_SetTextureAlphaMod(Texture_Current, 255);

	}

	//Draw current piece
	drawTexture(Texture_Current, FONT_WIDTH * (*X + 1), 
		FONT_HEIGHT * ((int)*Y + 1), 1.0);

	//Draw PAUSED if game is paused
		//Center the text
	if (*paused)
		drawTexture(Texture_Paused, 
			FONT_WIDTH + FONT_WIDTH * *mapWidth / 2 - 3 * 
			(FONT_WIDTH + STRING_GAP), FONT_HEIGHT * (23 - 0.5), 1.0);

	//------------------------------------------------------------------------------

	//Animations -----------------------------------------------------------------

	//Line-clearing animation -------------------
	if (*clearingLine == true && numCompleted != NULL)
	{
		
		unsigned short prevNumCompleted = *numCompleted;
		
		if (playLineAnimation(foreground, *completedRows, clearingLine,
			mapData, numCompleted, *mapWidth, *mapHeight) == true)
			playSound(globalInstance->sounds[COMPLETE_SOUND_ID]);

		//Remove first element in completedRows array
			//Also resize completedRows array
		if (prevNumCompleted != *numCompleted && *numCompleted > 0)
		{

			unsigned short* tempRows;
			tempRows = malloc(*numCompleted * sizeof(*tempRows));
			for (unsigned short i = 0; i < *numCompleted; i++)
				if (tempRows + i != NULL)
					*(tempRows + i) = *(completedRows + i + 1);

			free(completedRows);
			completedRows = malloc(*numCompleted * sizeof(*completedRows));
			for (unsigned short i = 0; i < *numCompleted; i++)
				if (completedRows + i != NULL && tempRows + i != NULL)
					*(completedRows + i) = *(tempRows + i);

			free(tempRows);

		}

		//Recalculate ghostY
		*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData, *mapWidth,
								*mapHeight);
		
	}

	//If game is over
	if (*gameOver == true)
	{

		if (*overAnimation == false)
		{

			*overAnimation = playOverAnimation(foreground, *Score, *mapWidth, *mapHeight);

			//Only save the top score if the player is playing in MULTRIS mode
				//Save score once overAnimation is finished playing
			if (*overAnimation == true)
				if (*Score > loadTop() && mode == 0)
					saveTop(*Score);

		}
		else
		{

			if (globalInstance->controls[SELECT_BUTTON_ID].onPress)
			{

				//Free all memory taken by PLAYMODE -----------------------------------

				//Free variables
				freeVars();

				SDL_DestroyTexture(Texture_Score);
				Texture_Score = NULL;
				SDL_DestroyTexture(Texture_Level);
				Texture_Level = NULL;
				SDL_DestroyTexture(Texture_Lines);
				Texture_Lines = NULL;
				SDL_DestroyTexture(Texture_Paused);
				Texture_Paused = NULL;
				SDL_DestroyTexture(foreground);
				foreground = NULL;

				//Free arrays
				free(completedRows);
				completedRows = NULL;

				free(mapData);
				mapData = NULL;

				//--------------------------------------------------------

				return RESET;

			}

		}

	}

	//----------------------------------------------------------------------------------

	//LOGIC ------------------------------------------------------------------------------

	//If nextPiece does not exist, generate a new one
	if (nextPiece == NULL)
	{

		if (mode == 0)
			nextPiece = generateGamePiece(rand() % MAX_PIECE_SIZE + 1);
		else
			nextPiece = generateGamePiece(mode);

		//And create a new texture for it as well
		if (Texture_Next == NULL)
		{
			
			Texture_Next = createPieceTexture(*nextPiece);
			SDL_QueryTexture(Texture_Next, NULL, NULL, nextText_Width, 
				nextText_Height);

		}

	}

	//Gravity
	if (currentPiece != NULL && !*gameOver && !*paused)
	{
		
		//Piece is falling
		if (!isColliding(*currentPiece, *X, Y, DOWN, mapData, *mapWidth, *mapHeight))
		{

			//Don't drop piece while the clearingLine animation is in progress
			if (!*clearingLine)
			{

				if (*softDrop == false)
					*Y = *Y + *speed * globalInstance->frame_time;
				else
					*Y = *Y + 20 * *speed * globalInstance->frame_time;

			}

		}
		else     //Piece is placed
		{

			playSound(globalInstance->sounds[LAND_SOUND_ID]);

			drawPiece(*currentPiece, foreground, *X * SPRITE_WIDTH, 
				(int)*Y * SPRITE_HEIGHT);

			//Store piece in mapData
			if (mapData != NULL)
				for (unsigned short i = 0; i < currentPiece->numOfBlocks; i++)
					*(mapData + ((int)*Y + (currentPiece->blocks[i].Y - currentPiece->minY))
					* *mapWidth + (*X + (currentPiece->blocks[i].X - 
					currentPiece->minX))) = true;

			//Check if the player just completed a line
			if (numCompleted != NULL)
				*numCompleted = completedLine(mapData, (unsigned short)*Y, *currentPiece, 
												&completedRows, *mapWidth, *mapHeight);
			if (*numCompleted > 0)
			{

				//This enables the clearing animation to play
				*clearingLine = true;

				//Scoring formula was derived by plotting the scoring table from the NES 
				//version of tetris and then finding a best-fit line
				*Score += (unsigned int)((93.333 * pow(*numCompleted, 3) - 490 * 
					pow(*numCompleted, 2) + 876.67 * *numCompleted - 440) * (*Level + 1));
				updateScore(*Score, Texture_Score);

				//Keep track of the number of lines that have been cleared on the current 
				//level
				*linesAtCurrentLevel += *numCompleted;
				if (*linesAtCurrentLevel >= 5 * (*Level + 1))
				{
					
					//Increase level count and update Texture_Level
					*Level += 1;
					*linesAtCurrentLevel = 0;
					updateLevel(*Level, Texture_Level);

					//Increase speed
					*speed = (double)(60.0988 / (48 - 5 * *Level));

				}
				updateLines(5 * (*Level + 1) - *linesAtCurrentLevel, 
					&Texture_Lines);

			}

			delPiece(&currentPiece);
			SDL_DestroyTexture(Texture_Current);
			Texture_Current = NULL;

			currentPiece = nextPiece;
			adjustNewPiece(currentPiece, X, *mapWidth);

			//NextPiece no longer exists
			nextPiece = NULL;

			SDL_DestroyTexture(Texture_Next);
			Texture_Next = NULL;

			//Reset Y
			*Y = 0;

			//Player can hold again
			if (*justHeld)
				*justHeld = false;

			//Recalculate ghostY
			*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData, *mapWidth,
									*mapHeight);

			//Player lost
			if (isColliding(*currentPiece, *X, Y, NONE, mapData, *mapWidth, *mapHeight))
			{

				playSound(globalInstance->sounds[OVER_SOUND_ID]);
				*gameOver = true;

			}

		}

	}

	return PLAY_SCREEN;

}

unsigned short calcGhostY(piece* Piece, unsigned short X, unsigned short startY, 
							bool* mapData, unsigned short mapWidth, unsigned short mapHeight)
{

	double Y = startY;

	while (isColliding(*Piece, X, &Y, DOWN, mapData, mapWidth, mapHeight) == false)
		Y = Y + 1;

	return (unsigned short)Y;

}

bool playOverAnimation(SDL_Texture* foreground, unsigned int score, unsigned short mapWidth,
						unsigned short mapHeight)
{

	//Return if the animation is done playing or not
	bool animationOver = false;

	//These are used to time the animation
	static Uint32* time_start = NULL;
	static Uint32* time_now = NULL;
	if (time_start == NULL)
	{

		time_start = malloc(sizeof(*time_start));
		time_now = malloc(sizeof(*time_now));
		if (time_start != NULL)
			*time_start = SDL_GetTicks();

	}
	else
	{

		if (time_now != NULL)
		{

			*time_now = SDL_GetTicks();

			//When enough time has lapsed since the last frame in the animation, play 
			//another frame
			if ((*time_now - *time_start) > OVER_TIME)
			{

				SDL_SetRenderTarget(globalInstance->renderer, foreground);

				static unsigned short* row;
				if (row == NULL)
				{

					row = malloc(sizeof(*row));
					if (row != NULL)
						*row = 0;

				}
				
				//With every frame of animation, we draw a row of randomly colored BLOCKS to 
				//the screen We fill the playable area with these blocks one row at a time
				if (row != NULL)
				{

					//Fill the current row with randomly colored BLOCKSs
					for (unsigned short i = 0; i < mapWidth; i++)
						drawToTexture(BLOCK_SPRITE_ID, foreground, 
							SPRITE_WIDTH * i, SPRITE_HEIGHT * *row, 1,
							(rand() % (RED - YELLOW + 1)) + YELLOW);

					//Check if we are on the last row
					if (*row < mapHeight)
						*row += 1;
					else
					{

						//If we are on the last row, the animation is over
						animationOver = true;
						
						free(row);
						row = NULL;

						//Print "GAME OVER"
						printToTexture("GAME", foreground, 
							(unsigned short)(FONT_WIDTH * 3.5), 
							(unsigned short)(FONT_HEIGHT * 10), 6, BLACK);
						printToTexture("OVER", foreground, 
							(unsigned short)(FONT_WIDTH * 3.5),
							(unsigned short)(FONT_HEIGHT * 17), 6, BLACK);

						//Print the score
						printToTexture("Score:", foreground,
							(unsigned short)(FONT_WIDTH * (10.5 - 
							getIntLength(score))),
							(unsigned short)(FONT_HEIGHT * 24), 2, BLACK);
						intToTexture(score, foreground,
							(unsigned short)(FONT_WIDTH * (23.5 - 
							getIntLength(score))),
							(unsigned short)(FONT_HEIGHT * 24), 2, BLACK);

					}

				}

				//Free time variables
				free(time_start);
				time_start = NULL;
				free(time_now);
				time_now = NULL;

				SDL_SetRenderTarget(globalInstance->renderer, NULL);

			}

		}

	}

	return animationOver;

}

void updateLines(unsigned short lines, SDL_Texture** linesTexture)
{

	//Erase linesTexture
	clearTexture(*linesTexture);
	SDL_DestroyTexture(*linesTexture);

	//Recreate linesTexture
	*linesTexture = createTexture(getIntStringLength(lines, 1.0), FONT_HEIGHT);

	//Draw new level to levelTexture
	intToTexture(lines, *linesTexture, 0, 0, 1, ORANGE);

}

void updateLevel(unsigned short level, SDL_Texture* levelTexture)
{

	//Clear level texture
	clearTexture(levelTexture);

	//Draw new level to levelTexture
	intToTexture(level, levelTexture, 0, 0, 1, RED);

}

void updateScore(unsigned int score, SDL_Texture* scoreTexture)
{

	//Get the number of digits in the score
	unsigned short length = (unsigned short)(floor(log10(abs(score))) + 1);

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

bool playLineAnimation(SDL_Texture* foreground, unsigned short row, 
	bool *clearingLine, bool *mapData, unsigned short* numCompleted, unsigned short mapWidth,
	unsigned short mapHeight)
{

	bool playSound = false;

	static Uint32* time_start = NULL;
	static Uint32* time_now = NULL;
	if (time_start == NULL)
	{

		time_start = malloc(sizeof(*time_start));
		time_now = malloc(sizeof(*time_now));
		if (time_start != NULL)
			*time_start = SDL_GetTicks();

	}
	else
	{

		if (time_now != NULL)
		{

			*time_now = SDL_GetTicks();

			if ((*time_now - *time_start) > CLEARING_TIME)
			{

				//With each frame of the animation, remove two blocks from the line being erased
				SDL_SetRenderTarget(globalInstance->renderer, foreground);

				static unsigned short* column;
				if (column == NULL)
				{

					column = malloc(sizeof(*column));
					*column = 0;

					playSound = true;

				}
				else if (column != NULL)
				{

					//Because we remove a block on each side of the chasm
						//The blocks getting removed are +-*column blocks away from the center
					drawToTexture(BLOCK_SPRITE_ID, foreground,
						(mapWidth / 2 - *column) * SPRITE_WIDTH, 
						row * SPRITE_HEIGHT, 1, BLACK);
					drawToTexture(BLOCK_SPRITE_ID, foreground,
						(mapWidth / 2 + *column) * SPRITE_WIDTH, 
						row * SPRITE_HEIGHT, 1, BLACK);

					//By increasing *column, we're basically increaing the radius of the 
					//chasm of blocks we are removing
					*column = *column + 1;

					if (*column > mapWidth / 2)
					{

						free(column);
						column = NULL;

						*numCompleted = *numCompleted - 1;
						if (*numCompleted == 0)
							*clearingLine = false;

						removeLine(row, mapData, foreground, mapWidth);

					}

				}

				free(time_start);
				time_start = NULL;
				free(time_now);
				time_now = NULL;

				SDL_SetRenderTarget(globalInstance->renderer, NULL);

			}

		}

	}

	return playSound;

}

void removeLine(unsigned short row, bool* mapData, SDL_Texture* foreground,
				unsigned short mapWidth)
{

	//All blocks in memory for the clearedLine are set to empty
	for (unsigned short j = 0; j < mapWidth; j++)
		*(mapData + row * mapWidth + j) = false;

	SDL_Rect srcRect
		= { .x = 0, .y = 0, .w = mapWidth * SPRITE_WIDTH, 
			.h = row * SPRITE_HEIGHT };
	
	//Copy portion of the foreground above the clearedLine
	SDL_Texture* aboveText;
	aboveText = createTexture(srcRect.w, srcRect.h);
	SDL_SetRenderTarget(globalInstance->renderer, aboveText);
	SDL_RenderCopy(globalInstance->renderer, foreground, &srcRect, NULL);

	//Clear area of foreground above the clearedLine
	SDL_SetRenderTarget(globalInstance->renderer, foreground);
	SDL_SetRenderDrawColor(globalInstance->renderer, 0, 0, 0, 0);
	SDL_RenderFillRect(globalInstance->renderer, &srcRect);

	SDL_Rect dstRect
		= { .x = 0, .y = SPRITE_HEIGHT, .w = srcRect.w, .h = srcRect.h };

	//Copy the texture from above the clearedLine down by 1 row
	SDL_RenderCopy(globalInstance->renderer, aboveText, NULL, &dstRect);
	SDL_SetRenderTarget(globalInstance->renderer, NULL);

	//Shift all mapData above clearedLine down by 1
	for (unsigned short i = row; i > 0; i--)
		for (unsigned short j = 0; j < mapWidth; j++)
			*(mapData + i * mapWidth + j) = 
				*(mapData + (i - 1) * mapWidth + j);

}

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
unsigned short completedLine(bool* mapData, unsigned short Y, piece Piece, 
								unsigned short** returnRows, unsigned short mapWidth,
								unsigned short mapHeight)
{

	//Duct-tape fix for a rare glitch that can sometimes happen when holding near the top
	//of the play area
		//Basically, if Y is out-of-bounds, just move it back in bounds.
	if (Y < 0)
		Y = 0;
	else if (Y > mapHeight)
		Y = mapHeight;

	unsigned short numCompleted = 0;

	//Count the number of lines completed
	for (unsigned short i = 0; i < Piece.height; i++)
		if (lineIsComplete(mapData, Y + i, mapWidth) == true)
			numCompleted = numCompleted + 1;

	if (numCompleted > 0)
	{

		free(*returnRows);
		*returnRows = malloc(numCompleted * sizeof(**returnRows));

		//Get the row numbers of each row completed
		unsigned short currentReturnRow = 0;
		for (unsigned short i = 0; i < Piece.height; i++)
		{

			if (lineIsComplete(mapData, Y + i, mapWidth) == true)
			{

				if (*returnRows + currentReturnRow != NULL)
					*(*returnRows + currentReturnRow) = Y + i;

				currentReturnRow = currentReturnRow + 1;

			}

		}

	}

	return numCompleted;

}

//This prevents pieces from spawning partially in the wall
void adjustNewPiece(piece* Piece, unsigned short* X, unsigned short mapWidth)
{

	if (*X + Piece->width > mapWidth)
		*X = *X - ((*X + Piece->width) - mapWidth);

}

//Check if a piece is colliding with another piece in various circumstances
bool isColliding(piece Piece, unsigned short X, double* Y, unsigned short direction, 
					bool* mapData, unsigned short mapWidth, unsigned short mapHeight)
{

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

void move(char keyPress, unsigned short *X, piece Piece, unsigned short mapWidth)
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

piece* getFirstPiece(piece* firstPiece)
{
	
	piece* currentPiece;
	currentPiece = malloc(sizeof(*currentPiece));
	if (currentPiece != NULL)
	{

		currentPiece = malloc(sizeof(*currentPiece));
		if (currentPiece != NULL)
		{

			//Copy over all attributes of the firstPiece
			currentPiece->width = firstPiece->width;
			currentPiece->height = firstPiece->height;
			currentPiece->numOfBlocks = firstPiece->numOfBlocks;
			currentPiece->color = firstPiece->color;
			currentPiece->minX = firstPiece->minX;
			currentPiece->minY = firstPiece->minY;

			//Copy over all blocks from firstPiece into currentPiece
			if (currentPiece->numOfBlocks > 0)
				currentPiece->blocks = malloc(currentPiece->numOfBlocks * 
												sizeof(*currentPiece->blocks));
			for (unsigned short i = 0; i < currentPiece->numOfBlocks; i++)
			{
				if (currentPiece->blocks != NULL)
				{

					currentPiece->blocks[i].X = firstPiece->blocks[i].X;
					currentPiece->blocks[i].Y = firstPiece->blocks[i].Y;

				}

			}

		}

	}

	return currentPiece;

}