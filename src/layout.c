#include <stdbool.h>
#include <math.h>

#include "HEADERS/MGF.h"

//draw.c
void drawRectangle(int spriteID, SDL_Texture* dstTexture, unsigned short X, 
	unsigned short Y, unsigned short Width, unsigned short Height, 
	unsigned short color, bool filled);
void printToTexture(char* string, SDL_Texture* dstTexture, int X, int Y, 
	float multiplier, unsigned short color);
void intToTexture(int num, SDL_Texture* dstTexture, int X, int Y, 
	float multiplier, unsigned short color);
unsigned short getIntLength(int num);
int getStringLength(char* str, float multiplier);
SDL_Texture* createTexture(int width, int height);
void drawTexture(SDL_Texture* texture, int X, int Y, float multiplier);

//file.c
unsigned int loadTop();

//Draw the playfield
void drawPlayField(SDL_Texture* background, unsigned short size)
{

    drawRectangle(WALL_SPRITE_ID, background, 0, 0, round(BASE_PLAYFIELD_WIDTH * size) + 2, 
                    round(BASE_PLAYFIELD_HEIGHT * size) + 2, GRAY, false);
		
}

void drawScoreBox(SDL_Texture* background, unsigned short size)
{

	//Save current rendering target
	SDL_Texture* currentTarget = SDL_GetRenderTarget(globalInstance->renderer);

	//Create 'content' texture
	int contentWidth = getStringLength("000000", 1.0);
	int contentHeight = getStringLength("00000", 1.0);
	SDL_Texture* content = createTexture(contentWidth, contentHeight);

    //Load the top score and create a string for it
    int topScore = loadTop();
	char* top;
	unsigned short zeroLength = 6 - getIntLength(topScore);
	top = malloc(zeroLength * sizeof(*top));
	*(top + zeroLength) = '\0';
	for (unsigned short i = 0; i < zeroLength; i++)
		*(top + i) = '0';

	//Print all content to 'content' texture
	printToTexture("TOP   ", content, 
					0.5 * (contentWidth - getStringLength("TOP   ", 1.0)), 
					0, 1, WHITE);
	printToTexture(top, content,
					0.5 * (contentWidth - getStringLength("000000", 1.0)),
					(FONT_HEIGHT + STRING_GAP), 1, WHITE);
	intToTexture(topScore, content, 
					0.5 * (contentWidth - 
					getStringLength("000000", 1.0)) + zeroLength * (FONT_WIDTH + STRING_GAP), 
					(FONT_HEIGHT + STRING_GAP), 1, WHITE);
    printToTexture("SCORE", content, 
					0.5 * (contentWidth - getStringLength("SCORE ", 1.0)), 
					(FONT_WIDTH + STRING_GAP) * 3, 1, WHITE);

    int X = 0;
    int Y = 0;

	int width_in_sprites = 9;
	int height_in_sprites = 6;
	int width_in_pixels = SPRITE_WIDTH * width_in_sprites;
	int height_in_pixels = SPRITE_HEIGHT * height_in_sprites;

    if (size == 0 || size == MAX_PIECE_SIZE)
    {

		//Set X any Y
        X = SPRITE_WIDTH * (round(BASE_PLAYFIELD_WIDTH * size) + 1);
        Y = 0;

    }

	//Draw box
    drawRectangle(WALL_SPRITE_ID, background, X, Y, width_in_sprites + 2, 
					height_in_sprites + 2, GRAY, false);

	//Draw the 'content' texture
	SDL_SetRenderTarget(globalInstance->renderer, background);
	drawTexture(content, X + SPRITE_WIDTH + 0.5 * (width_in_pixels - contentWidth), 
				Y + SPRITE_HEIGHT + 0.5 * (height_in_pixels - contentHeight), 1.0);

	//Free memory
    free(top);
	SDL_DestroyTexture(content);

	//Reset rendering target
	SDL_SetRenderTarget(globalInstance->renderer, currentTarget);

}

void drawLevelBox(SDL_Texture* background, unsigned short size)
{

	//Save the current rendering target
	SDL_Texture* currentTarget = SDL_GetRenderTarget(globalInstance->renderer);

	//Create 'content' texture
	int contentWidth = getStringLength("LEVEL", 1.0);
	int contentHeight = getStringLength("00", 1.0);
	SDL_Texture* content = createTexture(contentWidth, contentHeight);

	//Print all content to 'content' texture
	printToTexture("LEVEL", content, 0, 0, 1, WHITE);

	int X = 0;
	int Y = 0;

	int width_in_sprites = 9;
	int height_in_sprites = 3;
	int width_in_pixels = SPRITE_WIDTH * width_in_sprites;
	int height_in_pixels = SPRITE_HEIGHT * height_in_sprites;

	if (size == 0 || size == MAX_PIECE_SIZE)
	{

		//Set X and Y
		X = SPRITE_WIDTH * (round(BASE_PLAYFIELD_WIDTH * size) + 1);
		Y = FONT_HEIGHT * 7;

	}

	//Draw box
	drawRectangle(WALL_SPRITE_ID, background, X, Y, width_in_sprites + 2, 
					height_in_sprites + 2, GRAY, false);

	//Draw the 'content' texture
	SDL_SetRenderTarget(globalInstance->renderer, background);
	drawTexture(content, X + SPRITE_WIDTH + 0.5 * (width_in_pixels - contentWidth), 
				Y + SPRITE_HEIGHT + 0.5 * (height_in_pixels - contentHeight), 1.0);

	//Free 'content' memory
	SDL_DestroyTexture(content);

	//Reset rendering target
	SDL_SetRenderTarget(globalInstance->renderer, currentTarget);

}

void drawUntilBox(SDL_Texture* background, unsigned short size)
{

	//Save the current rendering target
	SDL_Texture* currentTarget = SDL_GetRenderTarget(globalInstance->renderer);

	//Create 'content' texture
	int contentWidth = getStringLength("LEVELUP", 1.0);
	int contentHeight = getStringLength("0000", 1.0);
	SDL_Texture* content = createTexture(contentWidth, contentHeight);

	//Print all content to 'content' texture
	printToTexture("LINES", content, 
					0.5 * (contentWidth - getStringLength("LINES", 1.0)), 
					0, 1, WHITE);
	printToTexture("UNTIL", content,
					0.5 * (contentWidth - getStringLength("UNTIL", 1.0)),
					(FONT_HEIGHT + STRING_GAP), 1, WHITE);
	printToTexture("LEVELUP", content,
					0.5 * (contentWidth - getStringLength("LEVELUP", 1.0)),
					(FONT_HEIGHT + STRING_GAP) * 2, 1, WHITE);


	int X = 0;
	int Y = 0;

	int width_in_sprites = 9;
	int height_in_sprites = 5;
	int width_in_pixels = SPRITE_WIDTH * width_in_sprites;
	int height_in_pixels = SPRITE_HEIGHT * height_in_sprites;

	if (size == 0 || size == MAX_PIECE_SIZE)
	{

		//Set X and Y
		X = SPRITE_WIDTH * (round(BASE_PLAYFIELD_WIDTH * size) + 1);
		Y = FONT_HEIGHT * 11;

	}

	//Draw box
	drawRectangle(WALL_SPRITE_ID, background, X, Y, width_in_sprites + 2, 
					height_in_sprites + 2, GRAY, false);
	
	//Draw the 'content' texture
	SDL_SetRenderTarget(globalInstance->renderer, background);
	drawTexture(content, X + SPRITE_WIDTH + 0.5 * (width_in_pixels - contentWidth), 
				Y + SPRITE_HEIGHT + 0.5 * (height_in_pixels - contentHeight), 1.0);

	//Free 'content' memory
	SDL_DestroyTexture(content);

	//Reset rendering target
	SDL_SetRenderTarget(globalInstance->renderer, currentTarget);

}

void drawNextBox(SDL_Texture* background, unsigned short size)
{

	//Save the current rendering target
	SDL_Texture* currentTarget = SDL_GetRenderTarget(globalInstance->renderer);

	//Create 'content' texture
	int contentWidth = getStringLength("NEXT", 1.0);
	int contentHeight = SPRITE_HEIGHT;
	SDL_Texture* content = createTexture(contentWidth, contentHeight);

	//Print all content to 'content' texture
	printToTexture("NEXT", content,
						0.5 * (contentWidth - getStringLength("NEXT", 1.0)),
						0, 1, WHITE);

	int X = 0;
	int Y = 0;

	int width_in_sprites = size + 1;
	int height_in_sprites = size + 2;
	int width_in_pixels = SPRITE_WIDTH * width_in_sprites;

	if (size == 0 || size == MAX_PIECE_SIZE)
	{

		//Set X and Y
		X = SPRITE_WIDTH * (round(BASE_PLAYFIELD_WIDTH * size) + 1);
		Y = FONT_HEIGHT * 17;

	}

	//Draw box
	drawRectangle(WALL_SPRITE_ID, background, X, Y, width_in_sprites + 2,
					height_in_sprites + 2, GRAY, false);
	
	//Draw the 'content' texture
	SDL_SetRenderTarget(globalInstance->renderer, background);
	drawTexture(content, X + SPRITE_WIDTH + 0.5 * (width_in_pixels - contentWidth),
				Y + SPRITE_HEIGHT + STRING_GAP, 1.0);

	//Free 'content' memory
	SDL_DestroyTexture(content);

	//Reset rendering target
	SDL_SetRenderTarget(globalInstance->renderer, currentTarget);

}

void drawHoldBox(SDL_Texture* background, unsigned short size)
{

	//Save the current rendering target
	SDL_Texture* currentTarget = SDL_GetRenderTarget(globalInstance->renderer);

	//Create 'content' texture
	int contentWidth = getStringLength("HOLD", 1.0);
	int contentHeight = SPRITE_HEIGHT;
	SDL_Texture* content = createTexture(contentWidth, contentHeight);

	//Print all content to 'content' texture
	printToTexture("HOLD", content,
					0.5 * (contentWidth - getStringLength("HOLD", 1.0)), 0, 1, WHITE);

	int X = 0;
	int Y = 0;

	int width_in_sprites = size + 1;
	int height_in_sprites = size + 2;
	int width_in_pixels = SPRITE_WIDTH * width_in_sprites;

	if (size == 0 || size == MAX_PIECE_SIZE)
	{

		//Set X and Y
		X = SPRITE_WIDTH * (round(BASE_PLAYFIELD_WIDTH * size) + 1);
		Y = FONT_HEIGHT * 28;

	}

	//Draw filled box
	drawRectangle(WALL_SPRITE_ID, background, X, Y, width_in_sprites + 2, 
					height_in_sprites, GRAY, true);

	//Cut out a box in the middle that is the correct size and in the correct space
	int width_difference = size + 1 - HOLD_TEXTURE_MULTI * width_in_sprites;
	drawRectangle(WALL_SPRITE_ID, background, 
					X + SPRITE_WIDTH * width_difference, 
					Y + SPRITE_HEIGHT,
					ceil(HOLD_TEXTURE_MULTI * width_in_sprites),
					ceil(HOLD_TEXTURE_MULTI * height_in_sprites), BLACK, true); 

	//Draw the 'content' texture
	SDL_SetRenderTarget(globalInstance->renderer, background);
	drawTexture(content, X + SPRITE_WIDTH + 0.5 * (width_in_pixels - contentWidth),
				Y + SPRITE_HEIGHT + STRING_GAP, 1.0);

	//Free 'content' memory
	SDL_DestroyTexture(content);

	//Reset rendering target
	SDL_SetRenderTarget(globalInstance->renderer, currentTarget);

}

void drawFPSBox(SDL_Texture* background, unsigned short size)
{

	//Save the current rendering target
	SDL_Texture* currentTarget = SDL_GetRenderTarget(globalInstance->renderer);

	//Create the 'content' texture
	int contentWidth = getStringLength("0000", 1.0);
	int contentHeight = SPRITE_HEIGHT * 3;
	SDL_Texture* content = createTexture(contentWidth, contentHeight);

	//Print all content to 'content' texture
	printToTexture("FPS", content,
					0.5 * (contentWidth - getStringLength("FPS", 1.0)), STRING_GAP * 2, 1, 
					WHITE);

	int X = 0;
	int Y = 0;

	int width_in_sprites = size + 1;
	int height_in_sprites = 3;
	int width_in_pixels = SPRITE_WIDTH * width_in_sprites;
	int height_in_pixels = SPRITE_HEIGHT * height_in_sprites;

	if (size == 0 || size == MAX_PIECE_SIZE)
	{

		//Set X and Y
		X = SPRITE_WIDTH * (round(BASE_PLAYFIELD_WIDTH * size) + 1);
		Y = FONT_HEIGHT * 37;

	}

	//Draw box
	drawRectangle(WALL_SPRITE_ID, background, X, Y, width_in_sprites + 2, 
					height_in_sprites + 2, GRAY, false);

	//Draw the 'content' texture
	SDL_SetRenderTarget(globalInstance->renderer, background);
	drawTexture(content, X + SPRITE_WIDTH + 0.5 * (width_in_pixels - contentWidth), 
				Y + SPRITE_HEIGHT + 0.5 * (height_in_pixels - contentHeight), 1.0);

	//Free 'content' memory
	SDL_DestroyTexture(content);

	//Reset rendering target
	SDL_SetRenderTarget(globalInstance->renderer, currentTarget);

}

//Calculate the current mapWidth
int calcMapWidth()
{

	if (MODE == 0)
		return (int)round(BASE_PLAYFIELD_WIDTH * MAX_PIECE_SIZE);
	else
		return (int)round(BASE_PLAYFIELD_WIDTH * MODE);

}

//Calculate the current mapHeight
int calcMapHeight()
{

	if (MODE == 0)
		return (int)round(BASE_PLAYFIELD_HEIGHT * MAX_PIECE_SIZE);
	else
		return (int)round(BASE_PLAYFIELD_HEIGHT * MODE);

}

SDL_Texture* create_Score_Text()
{

	SDL_Texture* texture;

	texture = createTexture(6 * (FONT_WIDTH + STRING_GAP), 
		FONT_HEIGHT);

	printToTexture("000000", texture, 0, 0, 1, WHITE);

	return texture;

}

SDL_Texture* create_Level_Text()
{

	SDL_Texture* texture;

	texture = createTexture(2 * (FONT_WIDTH + STRING_GAP), 
		FONT_HEIGHT);

	printToTexture("0", texture, 0, 0, 1, RED);

	return texture;

}

SDL_Texture* create_Lines_Text()
{

	SDL_Texture* texture;

	
	texture = createTexture(FONT_WIDTH, FONT_HEIGHT);
	printToTexture("5", texture, 0, 0, 1, ORANGE);

	return texture;

}

SDL_Texture* create_Pause_Text()
{

	SDL_Texture* texture;

	texture = createTexture(6 * (FONT_WIDTH + STRING_GAP), 
		FONT_HEIGHT);
	printToTexture("PAUSED", texture, 0, 0, 1, WHITE);

	return texture;

}

SDL_Texture* create_Foreground_Text()
{


	SDL_Texture* texture;

	texture = createTexture(FONT_WIDTH * MAP_WIDTH, FONT_HEIGHT * MAP_HEIGHT);

	return texture;

}