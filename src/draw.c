#include <ctype.h>
#include <stdbool.h>
#include <math.h>

#include "HEADERS/MGF.h"

//Return the "Red" value for a given color
unsigned short getColor_R(unsigned short color)
{

	if (color == GRAY)
		return 198;
	else if (color == WHITE)
		return 250;
	else if (color == YELLOW)
		return 253;
	else if (color == GREEN)
		return 119;
	else if (color == CYAN)
		return 220;
	else if (color == MAGENTA)
		return 178;
	else if (color == ORANGE)
		return 255;
	else if (color == BLUE)
		return 119;
	else if (color == RED)
		return 194;
	else if (color == BLACK)
		return 0;
	else
		return -1;

}

//Return the "Green" value for a given color
unsigned short getColor_G(unsigned short color)
{

	if (color == GRAY)
		return 203;
	else if (color == WHITE)
		return 250;
	else if (color == YELLOW)
		return 253;
	else if (color == GREEN)
		return 221;
	else if (color == CYAN)
		return 255;
	else if (color == MAGENTA)
		return 60;
	else if (color == ORANGE)
		return 179;
	else if (color == BLUE)
		return 157;
	else if (color == RED)
		return 59;
	else if (color == BLACK)
		return 0;
	else
		return -1;

}

//Return the "Blue" value for a given color
unsigned short getColor_B(unsigned short color)
{

	if (color == GRAY)
		return 206;
	else if (color == WHITE)
		return 250;
	else if (color == YELLOW)
		return 150;
	else if (color == GREEN)
		return 119;
	else if (color == CYAN)
		return 251;
	else if (color == MAGENTA)
		return 121;
	else if (color == ORANGE)
		return 71;
	else if (color == BLUE)
		return 202;
	else if (color == RED)
		return 34;
	else if (color == BLACK)
		return 0;
	else
		return -1;

}

//Get the number of digits in an integer
unsigned short getIntLength(int num)
{

	if (num == 0)
		return 1;
	else
		return (unsigned short)(floor(log10(abs(num))) + 1);

}

//Print string to a texture
void printToTexture(char* string, SDL_Texture* dstTexture, int X, int Y, 
	float multiplier, unsigned short color)
{

	//Save current rendering target
	SDL_Texture* currentTarget = SDL_GetRenderTarget(globalInstance->renderer);

	//Set rendering target
	SDL_SetRenderTarget(globalInstance->renderer, dstTexture);

	//Rectangles used for copying
	SDL_Rect srcR;
	SDL_Rect dstR;

	//Change color of string_SS
	SDL_SetTextureColorMod(globalInstance->stringSS, getColor_R(color),
		getColor_G(color), getColor_B(color));

	for (unsigned short i = 0; i < SDL_strlen(string); i++)
	{

		srcR.w = FONT_WIDTH; 
		srcR.h = FONT_HEIGHT;
		
		//Copy the corresponding character to the dstTexture
			//Drawable characters
		if (string[i] >= ' ' && string[i] <= '~')
		{

			//Source rectangle
			srcR.x = (string[i] - ' ') % 
				(globalInstance->stringSS_W / FONT_WIDTH) * FONT_WIDTH; 
			srcR.y = (string[i] - ' ') / 
				(globalInstance->stringSS_H / FONT_HEIGHT) * FONT_HEIGHT;
	
		}
		else	//Undrawable character will draw the "missing ?" character
		{

			srcR.x = ('~' - ' ' + 1) % 
				(globalInstance->stringSS_W / FONT_WIDTH) * FONT_WIDTH; 
			srcR.y = ('~' - ' ' + 1) / 
				(globalInstance->stringSS_H / FONT_HEIGHT) * FONT_HEIGHT;

		}

		//Destination rectangle
			//Calculate correct positioning and sizing
		dstR.x = X + i * (STRING_GAP + FONT_WIDTH) * multiplier; 
		dstR.y = Y;
		dstR.w = FONT_WIDTH * multiplier;
		dstR.h = FONT_HEIGHT * multiplier;

		//Copy the letter from stringSS to dstTexture
		SDL_RenderCopy(globalInstance->renderer, globalInstance->stringSS, 
			&srcR, &dstR);

	}

	//Reset string_SS color to white
	SDL_SetTextureColorMod(globalInstance->stringSS, 255, 255, 255);

	//Reset rendering target
	SDL_SetRenderTarget(globalInstance->renderer, currentTarget);

}

//Print integer to a texture
void intToTexture(int num, SDL_Texture* dstTexture, int X, int Y, 
	float multiplier, unsigned short color)
{

	//Stores the number of digits in the integer
	unsigned short length;

	if (num == 0)
	{

		//Printing 0 sometimes goes wrong with my algorithm, so this is just a 
		//simple way to fix that
		printToTexture("0", dstTexture, X, Y, multiplier, color);
		return;

	}
	else //Get the number of digits in the integet
		length = getIntLength(num);

	//Convert integer to a string
	char* string = malloc((length + 1) * sizeof(*string));
	if (string != NULL)
	{

		//Store each digit from num into string in reverse order
		unsigned short count = 0;
		while (num)
		{

			string[length - count - 1] = '0' + (num % 10);

			count = count + 1;
			num /= 10;

		}

		//End the string with a end-character
		string[length] = '\0';

	}

	//Now print that string
	printToTexture(string, dstTexture, X, Y, multiplier, color);

	//Delete string from memory
	free(string);

}

//Draw a sprite to a texture
void drawToTexture(unsigned int SpriteID, SDL_Texture* dstTexture, int X, int Y, 
	float multiplier, Uint8 color)
{

	//Save current rendering target
	SDL_Texture* currentTarget = SDL_GetRenderTarget(globalInstance->renderer);

	//Set rendering target
	SDL_SetRenderTarget(globalInstance->renderer, dstTexture);

	//Rectangles used for copying
	SDL_Rect srcR;
	SDL_Rect dstR;

	//Change color of game_SS
	SDL_SetTextureColorMod(globalInstance->gameSS, getColor_R(color),
		getColor_G(color), getColor_B(color));

	srcR.w = SPRITE_WIDTH; 
	srcR.h = SPRITE_HEIGHT;
		
	//Copy the corresponding sprite to the dstTexture
		//Make sure SpriteID is within bounds of the number of sprites
	if (SpriteID < globalInstance->gameSS_W / SPRITE_WIDTH * 
		globalInstance->gameSS_H / SPRITE_HEIGHT)
	{

		//Source rectangle
		srcR.x = SpriteID % (globalInstance->gameSS_W / SPRITE_WIDTH) * 
			SPRITE_WIDTH; 
		srcR.y = SpriteID / (globalInstance->gameSS_H / SPRITE_HEIGHT) * 
			SPRITE_HEIGHT;

		//Destination rectangle
			//Calculate correct positioning and sizing
		dstR.x = X; 
		dstR.y = Y;
		dstR.w = SPRITE_WIDTH * multiplier;
		dstR.h = SPRITE_HEIGHT * multiplier;

		//Copy the letter from gameSS to dstTexture
		SDL_RenderCopy(globalInstance->renderer, globalInstance->gameSS, 
			&srcR, &dstR);
	
	}
	else	//SpriteID's outside valid bounds will draw the "missing ?" character
	{

		char str[2];
		str[0] = '~' + 1;
		str[1] = '\0';

		printToTexture(str, dstTexture, X, Y, multiplier, color);

	}

	//Reset game_SS color to white
	SDL_SetTextureColorMod(globalInstance->gameSS, 255, 255, 255);

	//Reset rendering target
	SDL_SetRenderTarget(globalInstance->renderer, currentTarget);

}

//Draw a rectangle, filled or unfilled, with the passed sprite to a texture
void drawRectangle(int spriteID, SDL_Texture* dstTexture, unsigned short X, 
	unsigned short Y, unsigned short Width, unsigned short Height, 
	unsigned short color, bool filled)
{

	for (unsigned short i = 0; i < Width; i++)
	{

		for (unsigned short j = 0; j < Height; j++)
		{

			if (filled)
				drawToTexture(spriteID, dstTexture, X + i * FONT_WIDTH, 
					Y + j * FONT_HEIGHT, 1.0, color);
			else
				if ((i == 0 || i == Width - 1) || (j == 0 || j == Height - 1))
					drawToTexture(spriteID, dstTexture, X + i * FONT_WIDTH, 
						Y + j * FONT_HEIGHT, 1.0, color);

		}

	}

}

//Return the length, in pixels, of a string
int getStringLength(char* str, float multiplier)
{

	int length = SDL_strlen(str);

	return multiplier * (length * FONT_WIDTH + (length - 1) * STRING_GAP);

}

//Clear a texture with the current renderer's draw color
void clearTexture(SDL_Texture* texture)
{

	//Save current rendering target
	SDL_Texture* currentTarget = SDL_GetRenderTarget(globalInstance->renderer);
	//Save current rendering draw color
	Uint8 currentR;
	Uint8 currentG;
	Uint8 currentB;
	Uint8 currentA;
	SDL_GetRenderDrawColor(globalInstance->renderer, &currentR, &currentG,
		&currentB, &currentA);

	//Clear texture with "clear" color
	SDL_SetRenderDrawColor(globalInstance->renderer, 0, 0, 0, 0);
	SDL_SetRenderTarget(globalInstance->renderer, texture);
	SDL_RenderClear(globalInstance->renderer);

	//Reset rendering draw color
	SDL_SetRenderDrawColor(globalInstance->renderer, currentR, currentG,
		currentB, currentA);
	//Reset rendering target
	SDL_SetRenderTarget(globalInstance->renderer, currentTarget);

}

//Create a texture of the given width and height
SDL_Texture* createTexture(int width, int height)
{

	//Save current rendering target
	SDL_Texture* currentTarget = SDL_GetRenderTarget(globalInstance->renderer);

	SDL_Rect dstR = { .x = 0, .y = 0, .w = width, .h = height };

	SDL_Texture* texture;
	texture = SDL_CreateTexture(globalInstance->renderer, 
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, dstR.w, dstR.h);

	//For some reason, you have to clear the texture
	clearTexture(texture);

	//Reset rendering target
	SDL_SetRenderTarget(globalInstance->renderer, currentTarget);

	//Make it so stuff can render on top of it
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	return texture;	

}

//Draw texture to current rendering target
void drawTexture(SDL_Texture* texture, int X, int Y, float multiplier)
{

	//Rect used to draw the texture
	SDL_Rect dstR = { .x = X, .y = Y, .w = 0, .h = 0 };
	//Get the width and height of the texture
	SDL_QueryTexture(texture, NULL, NULL, &dstR.w, &dstR.h);

	//Resize texture appropriately
	dstR.w *= multiplier;
	dstR.h *= multiplier;

	//Copy the texture to the current rendering target
	SDL_RenderCopy(globalInstance->renderer, texture, NULL, &dstR);

}

//Draw a piece to the screen
	//Piece will be drawn with the top left of the Piece corresponding to X,Y
void drawPiece(piece Piece, SDL_Texture* dstTexture, unsigned short X, 
	unsigned short Y)
{

	for (unsigned short i = 0; i < Piece.numOfBlocks; i++)
		drawToTexture(BLOCK_SPRITE_ID, dstTexture, 
			X + SPRITE_WIDTH * (Piece.blocks[i].X - Piece.minX), 
			Y + SPRITE_HEIGHT * (Piece.blocks[i].Y - Piece.minY), 1.0, 
			Piece.color);

}

//Create a texture from the given piece
SDL_Texture* createPieceTexture(piece Piece)
{

	SDL_Rect DestR;
	DestR.w = SPRITE_WIDTH * Piece.width;
	DestR.h = SPRITE_HEIGHT * Piece.height;

	SDL_Texture* texture;
	texture = createTexture(DestR.w, DestR.h);

	//For some reason, you have to clear the texture
	clearTexture(texture);

	//Pretty simple, we just draw the piece to the texture
	drawPiece(Piece, texture, 0, 0);

	return texture;

}