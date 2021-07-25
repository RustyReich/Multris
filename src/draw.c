#include <ctype.h>
#include <stdbool.h>

#include "HEADERS/technicalDefinitions.h"
#include "HEADERS/Structures.h"

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

void drawSprite(sprite Sprite, unsigned short X, unsigned short Y, unsigned short multi, unsigned short color, 
	SDL_Renderer* renderer)
{
	
	SDL_SetRenderDrawColor(renderer, (Uint8)getColor_R(color), (Uint8)getColor_G(color), (Uint8)getColor_B(color), 
		255);

	SDL_Rect currentRect;

	//Draw each rectangle that the sprite is comprised of
	for (unsigned short i = 0; i < Sprite.numOfRect; i++)
	{

		currentRect.x = X + Sprite.rectangles[i].X * multi;
		currentRect.y = Y + Sprite.rectangles[i].Y * multi;
		currentRect.w = Sprite.rectangles[i].Width * multi;
		currentRect.h = Sprite.rectangles[i].Height * multi;

		SDL_RenderFillRect(renderer, &currentRect);

	}

}

void print_string(char* string, unsigned short X, unsigned short Y, unsigned short multi, unsigned short color,
	SDL_Renderer* renderer, sprite* Sprites)
{
	
	for (unsigned short i = 0; i < strlen(string); i++)
	{
		
		//Only upper case characters exist
		char currentChar = toupper(string[i]);

		//Draw the sprite that corresponds to the currentChar
		if (currentChar >= 'A' && currentChar <= 'Z')
			drawSprite(Sprites[currentChar - 'A'], X + i * (LETTER_GAP + CHAR_DIMENSION) * multi, Y,
				multi, color, renderer);
		else if (currentChar == ' ')
			drawSprite(Sprites[SPACE_CHAR], X + i * (LETTER_GAP + CHAR_DIMENSION) * multi, Y, multi, color, 
				renderer);
		else if (currentChar >= '0' && currentChar <= '9')
			drawSprite(Sprites[ZERO_CHAR + (currentChar - '0')],
				X + i * (LETTER_GAP + CHAR_DIMENSION) * multi, Y, multi, color, renderer);
		else if (currentChar == ':')
			drawSprite(Sprites[COLON_CHAR],
				X + i * (LETTER_GAP + CHAR_DIMENSION) * multi, Y, multi, color, renderer);
		else if (currentChar == '<')
			drawSprite(Sprites[LEFT_CHAR],
				X + i * (LETTER_GAP + CHAR_DIMENSION) * multi, Y, multi, color, renderer);
		else if (currentChar == '>')
			drawSprite(Sprites[RIGHT_CHAR],
				X + i * (LETTER_GAP + CHAR_DIMENSION) * multi, Y, multi, color, renderer);
		else if (currentChar == '^')
			drawSprite(Sprites[UP_CHAR],
				X + i * (LETTER_GAP + CHAR_DIMENSION) * multi, Y, multi, color, renderer);

	}

}

unsigned short getIntLength(int num)
{

	if (num == 0)
		return 1;
	else
		return (unsigned short)(floor(log10(abs(num))) + 1);

}

void print_int(int num, unsigned short X, unsigned short Y, unsigned short multi, unsigned short color, 
	SDL_Renderer* renderer, sprite* Sprites)
{

	//Stores the number of digits in the integer
	unsigned short length;

	if (num == 0)
	{

		//Printing 0 sometimes goes wrong with my algorithm, so this is just a simple way to fix that
		print_string("0", X, Y, multi, color, renderer, Sprites);
		return;

	}
	//Get the number of digits in the integer
	else
		length = (unsigned short)(floor(log10(abs(num))) + 1);
	
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
	print_string(string, X, Y, multi, color, renderer, Sprites);

	//Delete string from memory
	free(string);

}

//Draw a rectangle, filled or unfilled, with the passed sprite
void drawRectangle(sprite Sprite, unsigned short X, unsigned short Y, unsigned short Width, unsigned short Height,
	unsigned short color, bool filled, SDL_Renderer* renderer)
{

	for (unsigned short i = 0; i < Width; i++)
	{

		for (unsigned short j = 0; j < Height; j++)
		{

			if (filled)
				drawSprite(Sprite, X + i * CHAR_DIMENSION, Y + j * CHAR_DIMENSION, 1, color, renderer);
			else
				if ((i == 0 || i == Width - 1) || (j == 0 || j == Height - 1))
					drawSprite(Sprite, X + i * CHAR_DIMENSION, Y + j * CHAR_DIMENSION, 1, color, renderer);

		}

	}

}

//Draw a piece to the screen
	//Piece will be drawn with the top left of the Piece corresponding to X,Y
void drawPiece(piece Piece, unsigned short X, unsigned short Y, sprite Sprite, SDL_Renderer* renderer)
{

	for (unsigned short i = 0; i < Piece.numOfBlocks; i++)
		drawSprite(Sprite, X + CHAR_DIMENSION * (Piece.blocks[i].X - Piece.minX), 
			Y + CHAR_DIMENSION * (Piece.blocks[i].Y - Piece.minY), 1, Piece.color, renderer);

}