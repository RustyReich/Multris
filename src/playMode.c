#include <stdbool.h>
#include <math.h>

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
unsigned short getIntLength(int num);
SDL_Texture* createTexture(int width, int height);
void drawToTexture(unsigned int SpriteID,SDL_Texture* dstTexture,int X,int Y,float multiplier,Uint8 color);
void clearTexture(SDL_Texture* texture);
SDL_Texture* createPieceTexture(piece Piece);
int getStringLength(char* str, float multiplier);
int getIntStringLength(int num, float multiplier);
void drawSimpleRect(SDL_Texture* dstTexture, int x, int y, int width, int height, int color);

//rotate.c
void rotatePiece(piece *Piece, unsigned short direction);
void mirrorPieceOverY(piece* Piece);

//audio.c
void _playSound(int id);

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
void declare_HUD_Text(SDL_Texture** ptr, int type);
void declare_map_matrix(bool** ptr);

//layout.c
int getScoreDrawX(unsigned short size);
int getScoreDrawY(unsigned short size);
int getForegroundX(unsigned short size);
int getForegroundY(unsigned short size);
int getLevelX(unsigned short size, unsigned short level);
int getLevelY(unsigned short size, unsigned short level);
int getLinesX(unsigned short size, unsigned short lines);
int getLinesY(unsigned short size, unsigned short lines);
int getNextX(unsigned short size, int width);
int getNextY(unsigned short size, int height);
int getHoldX(unsigned short size, int width);
int getHoldY(unsigned short size, int height);
int getPausedX(unsigned short size, float multi);
int getPausedY(unsigned short size, float multi);

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

unsigned short playMode(piece* firstPiece)
{

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
	static unsigned short* ghostY; declare(ghostY, MAP_HEIGHT - currentPiece->height);
	static Uint32* moveStart; declare(moveStart, 0);
	static bool* moveStartBool; declare(moveStartBool, false);
	static int* nextText_Width; declare(nextText_Width, 0);
	static int* nextText_Height; declare(nextText_Height, 0);
	static int* holdText_Width; declare(holdText_Width, 0);
	static int* holdText_Height; declare(holdText_Height, 0);
	static bool* firstLoop; declare(firstLoop, true);
	static int* foregroundX; declare(foregroundX, 0);
	static int* foregroundY; declare(foregroundY, 0);
	static double* pausedMulti; declare(pausedMulti, 1.0);

	//Texutures
	static SDL_Texture* Texture_Current; declare_Piece_Text(&Texture_Current, currentPiece);
	static SDL_Texture* Texture_Next; declare_Piece_Text(&Texture_Next, nextPiece);
	static SDL_Texture* Texture_Hold; declare_Piece_Text(&Texture_Hold, holdPiece);
	static SDL_Texture* Texture_Score; declare_HUD_Text(&Texture_Score, SCORE_TEXT);
	static SDL_Texture* Texture_Level; declare_HUD_Text(&Texture_Level, LEVEL_TEXT);
	static SDL_Texture* Texture_Lines; declare_HUD_Text(&Texture_Lines, LINES_TEXT);
	static SDL_Texture* Texture_Paused; declare_HUD_Text(&Texture_Paused, PAUSED_TEXT);
	static SDL_Texture* foreground; declare_HUD_Text(&foreground, FOREGROUND_TEXT);

	//Arrays
		//Initialize completedRows to only include a row that is offscreen
	static unsigned short* completedRows; declare(completedRows, MAP_HEIGHT + 1);
	static bool* mapData; declare_map_matrix(&mapData);

	if (*firstLoop)
	{

		*foregroundX = getForegroundX(MODE);
		*foregroundY = getForegroundY(MODE);

		//Calculate multiplier for PAUSED text when it would be wider than the playfield
		if (MODE != 0)
			if (getStringLength("PAUSED", 1.0) > round(BASE_PLAYFIELD_WIDTH * MODE) * SPRITE_WIDTH)
				*pausedMulti = round(BASE_PLAYFIELD_WIDTH*MODE)*SPRITE_WIDTH/getStringLength("PAUSED",1.0);

		*firstLoop = false;

	}

	//CONTROLS ---------------------------------------------------------------

	//These controls only work if it is not a game over and the game is not paused and the clearing animation is not
	//playing
	if (!*gameOver && !*paused && !*clearingLine)
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

			if (*moveStartBool || (currTicks - *moveStart) >= 
				(MOVEMENT_WAIT + MOVEMENT_TIME))
			{

				if (onHold(LEFT_BUTTON) && 
					!isColliding(*currentPiece, *X, Y, LEFT, mapData, MAP_WIDTH, MAP_HEIGHT))
				{

					//Only play sound if can actually move
					if (*X != 0)
						playSound(MOVE_SOUND);
					move(LEFT, X, *currentPiece, MAP_WIDTH);

					//Recalculate ghostY
					*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, 
											mapData, MAP_WIDTH, MAP_HEIGHT);

				}
				else if (onHold(RIGHT_BUTTON) && 
							!isColliding(*currentPiece, *X, Y, RIGHT, mapData, MAP_WIDTH,
											MAP_HEIGHT))
				{

					if (*X + currentPiece->width < MAP_WIDTH)
						playSound(MOVE_SOUND);
					move(RIGHT, X, *currentPiece, MAP_WIDTH);

					//Recalculate ghostY
					*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, 
											mapData, MAP_WIDTH, MAP_HEIGHT);

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
		if (onPress(ROTATE_CCW_BUTTON) && *X + currentPiece->height <= MAP_WIDTH &&
			*Y + currentPiece->width <= MAP_HEIGHT)
		{

			rotatePiece(currentPiece, CCW);
			//If rotation puts the piece inside another piece, just rotate back the 
			//opposite way
			if (isColliding(*currentPiece, *X, Y, NONE, mapData, MAP_WIDTH, MAP_HEIGHT))
				rotatePiece(currentPiece, CW);
			else
			{

				//Only play sound if it actually rotated
				playSound(ROTATE_SOUND);
				SDL_DestroyTexture(Texture_Current);
				Texture_Current = NULL;

				//Recalculate ghostY
				*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData, MAP_WIDTH,
										MAP_HEIGHT);

			}

		}

		//Rotate CW
		if (onPress(ROTATE_CW_BUTTON) && *X + currentPiece->height <= MAP_WIDTH &&
			*Y + currentPiece->width <= MAP_HEIGHT)
		{

			rotatePiece(currentPiece, CW);
			if (isColliding(*currentPiece, *X, Y, NONE, mapData, MAP_WIDTH, MAP_HEIGHT))
				rotatePiece(currentPiece, CCW);
			else
			{

				playSound(ROTATE_SOUND);
				SDL_DestroyTexture(Texture_Current);
				Texture_Current = NULL;

				//Recalculate ghostY
				*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData, MAP_WIDTH,
										MAP_HEIGHT);

			}

		}

		//Mirroring
		if (onPress(MIRROR_BUTTON))
		{

			mirrorPieceOverY(currentPiece);
			//If mirroring puts the piece inside another piece, just mirror it back
			if (isColliding(*currentPiece, *X, Y, NONE, mapData, MAP_WIDTH, MAP_HEIGHT))
				mirrorPieceOverY(currentPiece);
			else
			{

				//Only play sound if actually mirrored
				playSound(ROTATE_SOUND);
				SDL_DestroyTexture(Texture_Current);
				Texture_Current = NULL;

				*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData, MAP_WIDTH,
										MAP_HEIGHT);

			}

		}

		//Holding
		if (onPress(HOLD_BUTTON) && !*justHeld)
		{

			playSound(HOLD_SOUND);

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
				adjustNewPiece(currentPiece, X, MAP_WIDTH);

				//NextPiece no longer exists
				nextPiece = NULL;
				//Destory nextPiece
				SDL_DestroyTexture(Texture_Next);
				Texture_Next = NULL;

				//Reset Y
				*Y = 0;

				//Recalculate ghostY
				*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData, MAP_WIDTH,
										MAP_HEIGHT);

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
				adjustNewPiece(currentPiece, X, MAP_WIDTH);

				//Delete tempPeice
				delPiece(&tempPiece);

				//Reset Y
				*Y = 0;

				//Recalculate ghostY
				*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData, MAP_WIDTH,
										MAP_HEIGHT);

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

	}

	//-----------------------------------------------------------------------

	//LOGIC ------------------------------------------------------------------------------

	//If nextPiece does not exist, generate a new one
	if (nextPiece == NULL)
	{

		if (MODE == 0)
			nextPiece = generateGamePiece(rand() % MAX_PIECE_SIZE + 1);
		else
			nextPiece = generateGamePiece(MODE);

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
		if (!isColliding(*currentPiece, *X, Y, DOWN, mapData, MAP_WIDTH, MAP_HEIGHT))
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

			playSound(LAND_SOUND);

			drawPiece(*currentPiece, foreground, *X * SPRITE_WIDTH, 
				(int)*Y * SPRITE_HEIGHT);

			//Store piece in mapData
			if (mapData != NULL)
				for (unsigned short i = 0; i < currentPiece->numOfBlocks; i++)
					*(mapData + ((int)*Y + (currentPiece->blocks[i].Y - currentPiece->minY))
					* MAP_WIDTH + (*X + (currentPiece->blocks[i].X - 
					currentPiece->minX))) = true;

			//Check if the player just completed a line
			if (numCompleted != NULL)
				*numCompleted = completedLine(mapData, (unsigned short)*Y, *currentPiece, 
												&completedRows, MAP_WIDTH, MAP_HEIGHT);
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
					
					//If the player completes more lines than is needed to level up, the completed lines will
					//carry-over between levels
					*linesAtCurrentLevel = *linesAtCurrentLevel - 5 * (*Level + 1);

					//Increase level count and update Texture_Level
					*Level += 1;
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
			adjustNewPiece(currentPiece, X, MAP_WIDTH);

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
			*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData, MAP_WIDTH,
									MAP_HEIGHT);

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
	drawTexture(Texture_Next, getNextX(MODE, *nextText_Width), getNextY(MODE, *nextText_Height), 1.0);
	drawTexture(Texture_Hold,getHoldX(MODE,*holdText_Width),getHoldY(MODE,*holdText_Height),HOLD_TEXTURE_MULTI);
	drawTexture(Texture_Score, getScoreDrawX(MODE), getScoreDrawY(MODE), 1.0);
	drawTexture(Texture_Level, getLevelX(MODE, *Level), getLevelY(MODE, *Level), 1.0);
	drawTexture(Texture_Lines, getLinesX(MODE, *linesAtCurrentLevel), getLinesY(MODE, *linesAtCurrentLevel), 1.0);

	//Draw the foreground
	drawTexture(foreground, *foregroundX, *foregroundY, 1.0);

	//Make Texture_Current opaque
	SDL_SetTextureAlphaMod(Texture_Current, 255 / 3);
	//Draw Texture_Current at ghostY
	drawTexture(Texture_Current, FONT_WIDTH * *X + *foregroundX, FONT_HEIGHT * (int)*ghostY + *foregroundY, 1.0);
	//Reset Texture_Current opacity
	SDL_SetTextureAlphaMod(Texture_Current, 255);

	//Draw current piece if game isnt over
	if (*gameOver == false)
		drawTexture(Texture_Current,FONT_WIDTH*(*X)+*foregroundX,FONT_HEIGHT*((int)*Y)+*foregroundY,1.0);

	//Draw PAUSED if game is paused
		//Center the text
	if (*paused)
		drawTexture(Texture_Paused,getPausedX(MODE,*pausedMulti),getPausedY(MODE,*pausedMulti),*pausedMulti);

	//Animations -----------------------------------------------------------------

	//Line-clearing animation -------------------
	if (*clearingLine == true && numCompleted != NULL)
	{
		
		unsigned short prevNumCompleted = *numCompleted;
		
		if (playLineAnimation(foreground, *completedRows, clearingLine,
			mapData, numCompleted, MAP_WIDTH, MAP_HEIGHT) == true)
			playSound(COMPLETE_SOUND);

		//Remove first element in completedRows array
			//Also resize completedRows array
		if (prevNumCompleted != *numCompleted && *numCompleted > 0)
		{

			unsigned short* tempRows;
			tempRows = malloc(*numCompleted * sizeof(*tempRows));
			for (unsigned short i = 0; i < *numCompleted; i++)
				*(tempRows + i) = *(completedRows + i + 1);

			free(completedRows);
			completedRows = malloc(*numCompleted * sizeof(*completedRows));
			for (unsigned short i = 0; i < *numCompleted; i++)
				*(completedRows + i) = *(tempRows + i);

			free(tempRows);

		}

		//Recalculate ghostY
		*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData, MAP_WIDTH, MAP_HEIGHT);
		
	}

	//If game is over
	if (*gameOver == true)
	{

		if (*overAnimation == false)
		{

			*overAnimation = playOverAnimation(foreground, *Score, MAP_WIDTH, MAP_HEIGHT);

			//Once animation is over, print GAME OVER to screen
			if (*overAnimation == true)
			{

				//Calculate multiplier, width, and height for "GAME" and "OVER"
				float multi = (float)(MAP_WIDTH * SPRITE_WIDTH) / (float)(getStringLength("GAME", 1)) * 0.9;
				int width = getStringLength("GAME", multi);
				int height = multi * FONT_HEIGHT;

				//Calculate X and Y value to print "GAME" at
				int x = 0.5 * (MAP_WIDTH * SPRITE_WIDTH - getStringLength("GAME", multi));
				int y = (0.5 * (MAP_HEIGHT * SPRITE_HEIGHT - (height * 2 + round(multi * STRING_GAP))));

				//Print "GAME"
					//Draw black rectangle behind it 
				drawSimpleRect(foreground, x, y, width, height, BLACK);
				printToTexture("GAME", foreground, x, y, multi, WHITE);
				
				//Draw black rectangle between GAME and OVER
				y = y + height;
				drawSimpleRect(foreground, x, y, width, round(multi * STRING_GAP), BLACK);

				//Calculate Y value for "OVER" and print it
				y = y + round(multi * STRING_GAP);
				drawSimpleRect(foreground, x, y, width, height, BLACK);
				printToTexture("OVER", foreground, x, y, multi, WHITE);

				//Only save the top score if the player is playing in MULTRIS mode
					//Save score once overAnimation is finished playing
				if (*Score > loadTop() && MODE == 0)
					saveTop(*Score);

			}

		}
		else
		{

			if (onPress(SELECT_BUTTON))
			{

				//Free all memory taken by PLAYMODE -----------------------------------

				freeVars();

				//--------------------------------------------------------

				return RESET;

			}

		}

	}

	//----------------------------------------------------------------------------------

	//------------------------------------------------------------------------------

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
			if ((*time_now - *time_start) > OVER_SOUND_LENGTH / MAP_HEIGHT)
			{

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
						drawToTexture(BLOCK_SPRITE_ID, foreground, SPRITE_WIDTH * i, SPRITE_HEIGHT * *row, 1,
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

					}

				}

				//Free time variables
				free(time_start);
				time_start = NULL;
				free(time_now);
				time_now = NULL;

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