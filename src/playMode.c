#include <stdbool.h>

#include "HEADERS/technicalDefinitions.h"
#include "HEADERS/Structures.h"

//generate.c
piece* generateGamePiece(unsigned short size);
void delPiece(piece** Piece);

//draw.c
void drawPiece(piece, unsigned short, unsigned short, sprite, SDL_Renderer*);
void drawSprite(sprite, unsigned short, unsigned short, unsigned short, unsigned short, SDL_Renderer*);
void print_int(int num, unsigned short X, unsigned short Y, unsigned short multi, unsigned short color,
	SDL_Renderer* renderer, sprite* Sprites);
void print_string(char* string, unsigned short X, unsigned short Y, unsigned short multi, unsigned short color,
	SDL_Renderer* renderer, sprite* Sprites);
unsigned short getIntLength(int num);

//rotate.c
void rotatePiece(piece*, unsigned short);

//audio.c
void playSound(sound*, SDL_AudioDeviceID*, SDL_AudioSpec*);
sound* loadSound(char*, SDL_AudioSpec*);
void delSound(sound** Sound);

//file.c
void saveTop(unsigned int score);
unsigned int loadTop();
unsigned short getOption(unsigned short line);

//texture.c
SDL_Texture* createTexture(SDL_Renderer*, unsigned short, unsigned short);
void drawTexture(SDL_Texture*, unsigned short, unsigned short, SDL_Renderer*);
SDL_Texture* createPieceTexture(piece, sprite, SDL_Renderer*);

piece* getFirstPiece(piece*);
void move(char, unsigned short*, piece);
bool isColliding(piece, unsigned short, double*, unsigned short, bool*);
void adjustNewPiece(piece*, unsigned short*);
bool inputLockPressed(Uint8*);
unsigned short completedLine(bool*, unsigned short, piece, unsigned short**);
bool lineIsComplete(bool*, unsigned short);
void removeLine(unsigned short, bool*, SDL_Texture*, SDL_Renderer*);
bool playLineAnimation(SDL_Renderer*, SDL_Texture*, sprite, unsigned short, bool*, bool*, unsigned short*);
void updateScore(unsigned int, SDL_Texture*, sprite*, SDL_Renderer*);
void updateLevel(unsigned short, SDL_Texture*, sprite*, SDL_Renderer*);
void updateLines(unsigned short, SDL_Texture*, sprite*, SDL_Renderer*);
bool playOverAnimation(SDL_Renderer*, SDL_Texture*, sprite*, unsigned int);
unsigned short calcGhostY(piece*, unsigned short, unsigned short, bool*);

unsigned short playMode(sprite* Sprites, double frame_time, SDL_Renderer* renderer, piece* firstPiece, 
	SDL_AudioDeviceID* audioDevice, SDL_AudioSpec* wavSpec, unsigned short size)
{
	
	//Get current keyboard state
	static Uint8* keys = NULL;
	if (keys == NULL)
		keys = (Uint8*)SDL_GetKeyboardState(NULL);
	
	//Game pieces
	static piece* nextPiece;
	static piece* currentPiece;
	if (currentPiece == NULL)
		currentPiece = getFirstPiece(firstPiece);
	if (nextPiece == NULL)
	{

		if (size == 0)
			nextPiece = generateGamePiece(rand() % MAX_PIECE_SIZE + 1);
		else
			nextPiece = generateGamePiece(size);

	}

	//Texutures
	static SDL_Texture* Texture_Current;
	static SDL_Texture* Texture_Next;
		static int* nextText_Width;
		if (nextText_Width == NULL)
			nextText_Width = malloc(sizeof(*nextText_Width));
		static int* nextText_Height;
		if (nextText_Height == NULL)
			nextText_Height = malloc(sizeof(*nextText_Height));
	static SDL_Texture* Texture_Score;
	static SDL_Texture* Texture_Level;
	static SDL_Texture* Texture_Lines;
	static SDL_Texture* foreground;
	if (Texture_Current == NULL)
		Texture_Current = createPieceTexture(*currentPiece, Sprites[BLOCK_CHAR], renderer);
	if (Texture_Next == NULL)
	{
		
		Texture_Next = createPieceTexture(*nextPiece, Sprites[BLOCK_CHAR], renderer);
		SDL_QueryTexture(Texture_Next, NULL, NULL, nextText_Width, nextText_Height);

	}
	if (Texture_Score == NULL)
	{

		Texture_Score = createTexture(renderer, 6 * (CHAR_DIMENSION + LETTER_GAP), CHAR_DIMENSION);

		SDL_SetRenderTarget(renderer, Texture_Score);
		print_string("000000", 0, 0, 1, WHITE, renderer, Sprites);
		SDL_SetRenderTarget(renderer, NULL);

	}
	if (Texture_Level == NULL)
	{

		Texture_Level = createTexture(renderer, 2 * (CHAR_DIMENSION + LETTER_GAP), CHAR_DIMENSION);

		SDL_SetRenderTarget(renderer, Texture_Level);
		print_string("0", 0, 0, 1, RED, renderer, Sprites);
		SDL_SetRenderTarget(renderer, NULL);

	}
	if (Texture_Lines == NULL)
	{

		Texture_Lines = createTexture(renderer, 3 * CHAR_DIMENSION + 2 * LETTER_GAP, CHAR_DIMENSION);
		SDL_SetRenderTarget(renderer, Texture_Lines);
		print_string("5", CHAR_DIMENSION, 0, 1, ORANGE, renderer, Sprites);
		SDL_SetRenderTarget(renderer, NULL);

	}
	if (foreground == NULL)
	{
		
		foreground = createTexture(renderer, CHAR_DIMENSION * WIDTH_OF_PLAYSPACE,
			CHAR_DIMENSION * (HEIGHT_IN_CHARS - 2));

	}

	//Sounds
	static sound* Sound_Move;
	static sound* Sound_Land;
	static sound* Sound_Rotate;
	static sound* Sound_Complete;
	static sound* Sound_Over;
	if (Sound_Move == NULL)
		Sound_Move = loadSound("AUDIO/move.wav", wavSpec);
	if (Sound_Land == NULL)
		Sound_Land = loadSound("AUDIO/land.wav", wavSpec);
	if (Sound_Rotate == NULL)
		Sound_Rotate = loadSound("AUDIO/rotate.wav", wavSpec);
	if (Sound_Complete == NULL)
		Sound_Complete = loadSound("AUDIO/complete.wav", wavSpec);
	if (Sound_Over == NULL)
		Sound_Over = loadSound("AUDIO/over.wav", wavSpec);

	//Variables
	static unsigned short* X;
	if (X == NULL)
	{

		X = malloc(sizeof(*X));
		*X = 0;

	}
	static double *Y;
	if (Y == NULL)
	{

		Y = malloc(sizeof(*Y));
		*Y = 0;

	}
	static double *speed;
	if (speed == NULL)
	{

		speed = malloc(sizeof(*speed));
		*speed = (double)(60.0988 / 48);	//In cells / second

	}
	static bool *softDrop;
	if (softDrop == NULL)
	{

		softDrop = malloc(sizeof(*softDrop));
		*softDrop = false;

	}
	static bool* gameOver;
	if (gameOver == NULL)
	{

		gameOver = malloc(sizeof(*gameOver));
		*gameOver = false;

	}
	static bool *clearingLine;
	if (clearingLine == NULL)
	{

		clearingLine = malloc(sizeof(*clearingLine));
		*clearingLine = false;

	}
	static bool* overAnimation;
	if (overAnimation == NULL)
	{

		overAnimation = malloc(sizeof(*overAnimation));
		*overAnimation = false;

	}
	static unsigned short* completedRows;
	static unsigned short* numCompleted;
	if (numCompleted == NULL)
	{

		numCompleted = malloc(sizeof(*numCompleted));
		*numCompleted = 0;

	}
	static unsigned int* Score;
	if (Score == NULL)
	{

		Score = malloc(sizeof(*Score));
		*Score = 0;

	}
	static unsigned short* Level;
	if (Level == NULL)
	{

		Level = malloc(sizeof(*Level));
		*Level = 0;

	}
	static unsigned short* linesAtCurrentLevel;
	if (linesAtCurrentLevel == NULL)
	{

		linesAtCurrentLevel = malloc(sizeof(*linesAtCurrentLevel));
		*linesAtCurrentLevel = 0;

	}
	static bool* ghostEnabled;
	if (ghostEnabled == NULL)
	{

		ghostEnabled = malloc(sizeof(*ghostEnabled));
		*ghostEnabled = getOption(1);

	}
	static unsigned short* ghostY;
	if (ghostY == NULL && *ghostEnabled)
	{

		ghostY = malloc(sizeof(*ghostY));
		*ghostY = HEIGHT_IN_CHARS - 2 - currentPiece->height;

	}

	//map data matrix
	static bool* mapData;
	if (mapData == NULL)
	{

		mapData = (bool*)malloc((HEIGHT_IN_CHARS - 2) * WIDTH_OF_PLAYSPACE * sizeof(*mapData));
		if (mapData != NULL)
			for (unsigned short i = 0; i < HEIGHT_IN_CHARS - 2; i++)
				for (unsigned short j = 0; j < WIDTH_OF_PLAYSPACE; j++)
					*(mapData + i * WIDTH_OF_PLAYSPACE + j) = false;

	}

	//CONTROLS ---------------

	//inputLocked keys
	static char* inputLock;		//This prevents the user from holding down certain buttons
	if (inputLock == NULL)
	{

		inputLock = malloc(sizeof(*inputLock));
		if (inputLock != NULL)
			*inputLock = true;

	}

	if (*inputLock == false && inputLockPressed(keys) && !*gameOver)
	{

		//Movement
		if (currentPiece != NULL)
		{

			if (keys[SDL_SCANCODE_LEFT] && !isColliding(*currentPiece, *X, Y, LEFT, mapData))
			{

				//Only play sound if can actually move
				if (*X != 0)
					playSound(Sound_Move, audioDevice, wavSpec);
				move(LEFT, X, *currentPiece);

				//Recalculate ghostY
				if (*ghostEnabled)
					*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData);

			}
			else if (keys[SDL_SCANCODE_RIGHT] && !isColliding(*currentPiece, *X, Y, RIGHT, mapData))
			{

				if (*X + currentPiece->width < WIDTH_OF_PLAYSPACE)
					playSound(Sound_Move, audioDevice, wavSpec);
				move(RIGHT, X, *currentPiece);

				//Recalculate ghostY
				if (*ghostEnabled)
					*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData);

			}
			else if (keys[SDL_SCANCODE_Z] && *X + currentPiece->height <= WIDTH_OF_PLAYSPACE &&
				*Y + currentPiece->width <= HEIGHT_IN_CHARS - 2)
			{

				rotatePiece(currentPiece, CCW);
				//If rotation puts the piece inside another piece, just rotate back the opposite way
				if (isColliding(*currentPiece, *X, Y, NONE, mapData))
					rotatePiece(currentPiece, CW);
				else
				{

					//Only play sound if it actually rotated
					playSound(Sound_Rotate, audioDevice, wavSpec);
					SDL_DestroyTexture(Texture_Current);
					Texture_Current = NULL;

					//Recalculate ghostY
					if (*ghostEnabled)
						*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData);

				}

			}
			else if (keys[SDL_SCANCODE_X] && *X + currentPiece->height <= WIDTH_OF_PLAYSPACE &&
				*Y + currentPiece->width <= HEIGHT_IN_CHARS - 2)
			{

				rotatePiece(currentPiece, CW);
				if (isColliding(*currentPiece, *X, Y, NONE, mapData))
					rotatePiece(currentPiece, CCW);
				else
				{

					playSound(Sound_Rotate, audioDevice, wavSpec);
					SDL_DestroyTexture(Texture_Current);
					Texture_Current = NULL;

					//Recalculate ghostY
					if (*ghostEnabled)
						*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData);

				}

			}
			
		}

		*inputLock = true;

	}
	else if (!inputLockPressed(keys))
		*inputLock = false;

	//Non-inputlocked keys
	if (keys[SDL_SCANCODE_DOWN])
		*softDrop = true;
	else
		*softDrop = false;

	// RENDERING -------------------------------

	drawTexture(Texture_Current, CHAR_DIMENSION * (*X + 1), CHAR_DIMENSION * ((int)*Y + 1), renderer);
	drawTexture(Texture_Next, (int)(CHAR_DIMENSION * 42.5) - *nextText_Width / 2, 
		(int)(CHAR_DIMENSION * 29.5) - *nextText_Height / 2, renderer);
	drawTexture(Texture_Score, CHAR_DIMENSION * 39, CHAR_DIMENSION * 6 + LETTER_GAP, renderer);
	drawTexture(Texture_Level, CHAR_DIMENSION * 46, CHAR_DIMENSION * 10, renderer);
	drawTexture(Texture_Lines, CHAR_DIMENSION * 41, CHAR_DIMENSION * 18, renderer);

	//Draw ghost if enabled
	if (*ghostEnabled)
	{

		//Make Texture_Current opaque
		SDL_SetTextureAlphaMod(Texture_Current, 255 / 3);
		//Draw Texture_Current at ghostY
		drawTexture(Texture_Current, CHAR_DIMENSION * (*X + 1), CHAR_DIMENSION * ((int)*ghostY + 1), renderer);
		//Reset Texture_Current opacity
		SDL_SetTextureAlphaMod(Texture_Current, 255);

	}

	drawTexture(foreground, CHAR_DIMENSION, CHAR_DIMENSION, renderer);

	//--------------------------------------

	//Line-clearing animation -------------------

	if (*clearingLine == true && numCompleted != NULL)
	{
		
		unsigned short prevNumCompleted = *numCompleted;
		
		if (playLineAnimation(renderer, foreground, Sprites[BLOCK_CHAR], *completedRows, clearingLine,
			mapData, numCompleted) == true)
			playSound(Sound_Complete, audioDevice, wavSpec);

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
		if (*ghostEnabled)
			*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData);
		
	}

	//---------------------------------------------

	//Game Over -----------------------------------

	if (*gameOver == true)
	{

		if (*overAnimation == false)
		{

			*overAnimation = playOverAnimation(renderer, foreground, Sprites, *Score);

			//Only save the top score if the player is playing in MULTRIS mode
				//Save score once overAnimation is finished playing
			if (*overAnimation == true)
				if (*Score > loadTop() && size == 0)
					saveTop(*Score);

		}
		else
		{

			if (keys[SDL_SCANCODE_RETURN])
			{

				//Free all memory taken by PLAYMODE ------------------

				delPiece(&nextPiece);
				SDL_DestroyTexture(Texture_Next);
				Texture_Next = NULL;
				delPiece(&currentPiece);
				SDL_DestroyTexture(Texture_Current);
				Texture_Current = NULL;

				free(nextText_Width);
				nextText_Width = NULL;
				free(nextText_Height);
				nextText_Height = NULL;

				SDL_DestroyTexture(Texture_Score);
				Texture_Score = NULL;
				SDL_DestroyTexture(Texture_Level);
				Texture_Level = NULL;
				SDL_DestroyTexture(Texture_Lines);
				Texture_Lines = NULL;
				SDL_DestroyTexture(foreground);
				foreground = NULL;

				delSound(&Sound_Move);
				delSound(&Sound_Land);
				delSound(&Sound_Rotate);
				delSound(&Sound_Complete);
				delSound(&Sound_Over);

				keys = NULL;

				free(X);
				X = NULL;
				free(Y);
				Y = NULL;
				free(speed);
				speed = NULL;
				free(softDrop);
				softDrop = NULL;
				free(gameOver);
				gameOver = NULL;
				free(clearingLine);
				clearingLine = NULL;
				free(overAnimation);
				overAnimation = NULL;
				free(completedRows);
				completedRows = NULL;
				free(numCompleted);
				numCompleted = NULL;
				free(Score);
				Score = NULL;
				free(Level);
				Level = NULL;
				free(linesAtCurrentLevel);
				linesAtCurrentLevel = NULL;
				free(ghostEnabled);
				ghostEnabled = NULL;
				free(ghostY);
				ghostY = NULL;

				free(inputLock);
				inputLock = NULL;

				free(mapData);
				mapData = NULL;

				//--------------------------------------------------------

				return RESET;

			}

		}

	}

	//---------------------------------------------

	//Gravity
	if (currentPiece != NULL && !*gameOver)
	{
		
		//Piece is falling
		if (!isColliding(*currentPiece, *X, Y, DOWN, mapData))
		{

			//Don't drop piece while the clearingLine animation is in progress
			if (!*clearingLine)
			{

				if (*softDrop == false)
					*Y = *Y + *speed * frame_time;
				else
					*Y = *Y + 20 * *speed * frame_time;

			}

		}
		else     //Piece is placed
		{

			playSound(Sound_Land, audioDevice, wavSpec);

			SDL_SetRenderTarget(renderer, foreground);
			drawPiece(*currentPiece, *X * CHAR_DIMENSION, (int)*Y * CHAR_DIMENSION, Sprites[BLOCK_CHAR], 
				renderer);
			SDL_SetRenderTarget(renderer, NULL);

			//Store piece in mapData
			if (mapData != NULL)
				for (unsigned short i = 0; i < currentPiece->numOfBlocks; i++)
					*(mapData + ((int)*Y + (currentPiece->blocks[i].Y - currentPiece->minY))
						* WIDTH_OF_PLAYSPACE + (*X + (currentPiece->blocks[i].X - currentPiece->minX))) = true;

			//Check if the player just completed a line
			if (numCompleted != NULL)
				*numCompleted = completedLine(mapData, (unsigned short)*Y, *currentPiece, &completedRows);
			if (*numCompleted > 0)
			{

				//This enables the clearing animation to play
				*clearingLine = true;

				//Scoring formula was derived by plotting the scoring table from the NES version of tetris and then finding
				//a best-fit line
				*Score += (unsigned int)((93.333 * pow(*numCompleted, 3) - 490 * pow(*numCompleted, 2) + 
					876.67 * *numCompleted - 440) * (*Level + 1));
				updateScore(*Score, Texture_Score, Sprites, renderer);

				//Keep track of the number of lines that have been cleared on the current level
				*linesAtCurrentLevel += *numCompleted;
				if (*linesAtCurrentLevel >= 5 * (*Level + 1))
				{
					
					//Increase level count and update Texture_Level
					*Level += 1;
					*linesAtCurrentLevel = 0;
					updateLevel(*Level, Texture_Level, Sprites, renderer);

					//Increase speed
					*speed = (double)(60.0988 / (48 - 5 * *Level));

				}
				updateLines(5 * (*Level + 1) - *linesAtCurrentLevel, Texture_Lines, Sprites, renderer);

			}

			delPiece(&currentPiece);
			SDL_DestroyTexture(Texture_Current);
			Texture_Current = NULL;

			currentPiece = nextPiece;
			adjustNewPiece(currentPiece, X);

			SDL_DestroyTexture(Texture_Next);
			Texture_Next = NULL;

			//Generate new piece
			if (size == 0)
				nextPiece = generateGamePiece(rand() % MAX_PIECE_SIZE + 1);
			else
				nextPiece = generateGamePiece(size);

			//Reset Y
			*Y = 0;

			//Recalculate ghostY
			if (*ghostEnabled)
				*ghostY = calcGhostY(currentPiece, *X, (unsigned short)*Y, mapData);

			//Player lost
			if (isColliding(*currentPiece, *X, Y, NONE, mapData))
			{

				playSound(Sound_Over, audioDevice, wavSpec);
				*gameOver = true;

			}

		}

	}

	return PLAY_SCREEN;

}

unsigned short calcGhostY(piece* Piece, unsigned short X, unsigned short startY, bool* mapData)
{

	double Y = startY;

	while (isColliding(*Piece, X, &Y, DOWN, mapData) == false)
		Y = Y + 1;

	return (unsigned short)Y;

}

bool playOverAnimation(SDL_Renderer* renderer, SDL_Texture* foreground, sprite* Sprites, unsigned int score)
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

			//When enough time has lapsed since the last frame in the animation, play another frame
			if ((*time_now - *time_start) > OVER_TIME)
			{

				SDL_SetRenderTarget(renderer, foreground);

				static unsigned short* row;
				if (row == NULL)
				{

					row = malloc(sizeof(*row));
					if (row != NULL)
						*row = 0;

				}
				
				//With every frame of animation, we draw a row of randomly colored BLOCKS to the screen
					//We fill the playable area with these blocks one row at a time
				if (row != NULL)
				{

					//Fill the current row with randomly colored BLOCKSs
					for (unsigned short i = 0; i < WIDTH_OF_PLAYSPACE; i++)
						drawSprite(Sprites[BLOCK_CHAR], CHAR_DIMENSION * i, CHAR_DIMENSION * *row, 1,
							(rand() % (RED - YELLOW + 1)) + YELLOW, renderer);

					//Check if we are on the last row
					if (*row < HEIGHT_IN_CHARS - 2)
						*row += 1;
					else
					{

						//If we are on the last row, the animation is over
						animationOver = true;
						
						free(row);
						row = NULL;

						//Print "GAME OVER"
						print_string("GAME", (unsigned short)(CHAR_DIMENSION * 3.5), 
							(unsigned short)(CHAR_DIMENSION * 10), 6, BLACK, renderer, Sprites);
						print_string("OVER", (unsigned short)(CHAR_DIMENSION * 3.5),
							(unsigned short)(CHAR_DIMENSION * 17), 6, BLACK, renderer, Sprites);

						//Print the score
						print_string("Score:", (unsigned short)(CHAR_DIMENSION * (10.5 - getIntLength(score))),
							(unsigned short)(CHAR_DIMENSION * 24), 2, BLACK, renderer, Sprites);
						print_int(score, (unsigned short)(CHAR_DIMENSION * (23.5 - getIntLength(score))),
							(unsigned short)(CHAR_DIMENSION * 24), 2, BLACK, renderer, Sprites);

					}

				}

				//Free time variables
				free(time_start);
				time_start = NULL;
				free(time_now);
				time_now = NULL;

				SDL_SetRenderTarget(renderer, NULL);

			}

		}

	}

	return animationOver;

}

void updateLines(unsigned short lines, SDL_Texture* linesTexture, sprite* Sprites, SDL_Renderer* renderer)
{

	//Get the number of digits in the lines number
	unsigned short length = getIntLength(lines);

	//Clear lines texture
	SDL_SetRenderTarget(renderer, linesTexture);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	//Draw new level to levelTexture
	print_int(lines, (int)(CHAR_DIMENSION * (1.5 - (float)length / 2)), 0, 1, ORANGE, renderer, Sprites);


	//Restore RenderTarget
	SDL_SetRenderTarget(renderer, NULL);

}

void updateLevel(unsigned short level, SDL_Texture* levelTexture, sprite* Sprites, SDL_Renderer* renderer)
{

	//Clear level texture
	SDL_SetRenderTarget(renderer, levelTexture);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	//Draw new level to levelTexture
	print_int(level, 0, 0, 1, RED, renderer, Sprites);

	//Restore RenderTarget
	SDL_SetRenderTarget(renderer, NULL);

}

void updateScore(unsigned int score, SDL_Texture* scoreTexture, sprite* Sprites, SDL_Renderer* renderer)
{

	//Get the number of digits in the score
	unsigned short length = (unsigned short)(floor(log10(abs(score))) + 1);

	//Clear area of scoreTexture that will be updated
	SDL_Rect rect;
	SDL_QueryTexture(scoreTexture, NULL, NULL, &rect.w, &rect.h);
	rect.x = rect.w - length * (CHAR_DIMENSION + LETTER_GAP);
	rect.w = length * (CHAR_DIMENSION + LETTER_GAP);
	rect.h = CHAR_DIMENSION;
	rect.y = 0;
	SDL_SetRenderTarget(renderer, scoreTexture);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderFillRect(renderer, &rect);

	//Draw new score to scoreTexture
	print_int(score, rect.x, rect.y, 1, WHITE, renderer, Sprites);

	//Restore RenderTarget
	SDL_SetRenderTarget(renderer, NULL);

}

bool playLineAnimation(SDL_Renderer* renderer, SDL_Texture* foreground, sprite Sprite, 
	unsigned short row, bool *clearingLine, bool *mapData, unsigned short* numCompleted)
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
				SDL_SetRenderTarget(renderer, foreground);

				static unsigned short* column;
				if (column == NULL)
				{

					column = malloc(sizeof(*column));
					if (column != NULL)
					{

						//Except on the first frame, we only erase one block
						*column = 0;
						drawSprite(Sprite, (WIDTH_OF_PLAYSPACE / 2) * CHAR_DIMENSION,
							row * CHAR_DIMENSION, 1, BLACK, renderer);

						playSound = true;

					}

				}
				else if (column != NULL)
				{

					//By increasing *column, we're basically increaing the radius of the chasm of blocks we are
					//removing
					*column = *column + 1;
					//Because we remove a block on each side of the chasm
						//The blocks getting removed are +-*column blocks away from the center
					drawSprite(Sprite,
						(WIDTH_OF_PLAYSPACE / 2 - *column) * CHAR_DIMENSION, row * CHAR_DIMENSION, 1, 
							BLACK, renderer);
					drawSprite(Sprite,
						(WIDTH_OF_PLAYSPACE / 2 + *column) * CHAR_DIMENSION, row * CHAR_DIMENSION, 1, 
							BLACK, renderer);

					if (*column > WIDTH_OF_PLAYSPACE / 2)
					{

						free(column);
						column = NULL;

						*numCompleted = *numCompleted - 1;
						if (*numCompleted == 0)
							*clearingLine = false;

						removeLine(row, mapData, foreground, renderer);

					}

				}

				free(time_start);
				time_start = NULL;
				free(time_now);
				time_now = NULL;

				SDL_SetRenderTarget(renderer, NULL);

			}

		}

	}

	return playSound;

}

void removeLine(unsigned short row, bool* mapData, SDL_Texture* foreground, SDL_Renderer* renderer)
{

	//All blocks in memory for the clearedLine are set to empty
	for (unsigned short j = 0; j < WIDTH_OF_PLAYSPACE; j++)
		*(mapData + row * WIDTH_OF_PLAYSPACE + j) = false;

	SDL_Rect srcRect
		= { .x = 0, .y = 0, .w = WIDTH_OF_PLAYSPACE * CHAR_DIMENSION, .h = row * CHAR_DIMENSION };
	
	//Copy portion of the foreground above the clearedLine
	SDL_Texture* aboveText;
	aboveText = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 
		srcRect.w, srcRect.h);
	SDL_SetRenderTarget(renderer, aboveText);
	SDL_RenderCopy(renderer, foreground, &srcRect, NULL);

	//Clear area of foreground above the clearedLine
	SDL_SetRenderTarget(renderer, foreground);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderFillRect(renderer, &srcRect);

	SDL_Rect dstRect
		= { .x = 0, .y = CHAR_DIMENSION, .w = srcRect.w, .h = srcRect.h };

	//Copy the texture from above the clearedLine down by 1 row
	SDL_RenderCopy(renderer, aboveText, NULL, &dstRect);
	SDL_SetRenderTarget(renderer, NULL);

	//Shift all mapData above clearedLine down by 1
	for (unsigned short i = row; i > 0; i--)
		for (unsigned short j = 0; j < WIDTH_OF_PLAYSPACE; j++)
			*(mapData + i * WIDTH_OF_PLAYSPACE + j) = *(mapData + (i - 1) * WIDTH_OF_PLAYSPACE + j);

}

bool lineIsComplete(bool* mapData, unsigned short row)
{

	//Just go through every block in the given row. If any of them are not filled, return false
	for (unsigned short j = 0; j < WIDTH_OF_PLAYSPACE; j++)
		if (*(mapData + row * WIDTH_OF_PLAYSPACE + j) == false)
			return false;

	//Otherwise, the line must be totally filled and thus complete, so return true
	return true;

}

//This function gets both the number of lines that have just been completed, as well as the row numbers
//of each one that was completed
unsigned short completedLine(bool* mapData, unsigned short Y, piece Piece, unsigned short** returnRows)
{

	unsigned short numCompleted = 0;

	//Count the number of lines completed
	for (unsigned short i = 0; i < Piece.height; i++)
		if (lineIsComplete(mapData, Y + i) == true)
			numCompleted = numCompleted + 1;

	if (numCompleted > 0)
	{

		free(*returnRows);
		*returnRows = malloc(numCompleted * sizeof(**returnRows));

		//Get the row numbers of each row completed
		unsigned short currentReturnRow = 0;
		for (unsigned short i = 0; i < Piece.height; i++)
		{

			if (lineIsComplete(mapData, Y + i) == true)
			{

				if (*returnRows + currentReturnRow != NULL)
					*(*returnRows + currentReturnRow) = Y + i;

				currentReturnRow = currentReturnRow + 1;

			}

		}

	}

	return numCompleted;

}

//Just check if an inputlocked key is being pressed
bool inputLockPressed(Uint8* keys)
{

	if (keys[SDL_SCANCODE_LEFT])
		return true;
	else if (keys[SDL_SCANCODE_RIGHT])
		return true;
	else if (keys[SDL_SCANCODE_Z])
		return true;
	else if (keys[SDL_SCANCODE_X])
		return true;

	return false;

}

//This prevents pieces from spawning partially in the wall
void adjustNewPiece(piece* Piece, unsigned short* X)
{

	if (*X + Piece->width > WIDTH_OF_PLAYSPACE)
		*X = *X - ((*X + Piece->width) - WIDTH_OF_PLAYSPACE);

}

//Check if a piece is colliding with another piece in various circumstances
bool isColliding(piece Piece, unsigned short X, double* Y, unsigned short direction, bool* mapData)
{

	if (direction == DOWN)
	{

		//Here, the piece is trying to move DOWN.
			//We allow it to move down, and then check if any blocks of it are already occupied in
			//map Data
		for (unsigned short i = 0; i < Piece.numOfBlocks; i++)
			if (*(mapData + ((int)*Y + (Piece.blocks[i].Y - Piece.minY))
				* WIDTH_OF_PLAYSPACE + (X + (Piece.blocks[i].X - Piece.minX))) == true ||
				(int)*Y + Piece.height > HEIGHT_IN_CHARS - 2)
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
				* WIDTH_OF_PLAYSPACE + (X + (Piece.blocks[i].X - Piece.minX) - 1)) == true)
				return true;

	}
	else if (direction == RIGHT)
	{

		//Check any blocks in mapData exist to the RIGHT of any blocks in the currentPiece
		for (unsigned short i = 0; i < Piece.numOfBlocks; i++)
			if (*(mapData + ((int)*Y + (Piece.blocks[i].Y - Piece.minY))
				* WIDTH_OF_PLAYSPACE + (X + (Piece.blocks[i].X - Piece.minX) + 1)) == true)
				return true;

	}
	else if (direction == NONE)
	{

		//Check any blocks in mapData exist exactly where currentPiece is.
			//currentPiece is not trying to move in this instance
		for (unsigned short i = 0; i < Piece.numOfBlocks; i++)
			if (*(mapData + ((int)*Y + (Piece.blocks[i].Y - Piece.minY))
				* WIDTH_OF_PLAYSPACE + (X + (Piece.blocks[i].X - Piece.minX))) == true)
				return true;

	}

	return false;

}

void move(char keyPress, unsigned short *X, piece Piece)
{
	
	//Only allow movement within bounds
	if (keyPress == LEFT)
	{

		if (*X > 0)
			*X = *X - 1;

	}
	else if (keyPress == RIGHT)
	{

		if (*X + Piece.width < WIDTH_OF_PLAYSPACE)
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
				currentPiece->blocks = malloc(currentPiece->numOfBlocks * sizeof(*currentPiece->blocks));
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