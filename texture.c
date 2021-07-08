#include <SDL.h>

#include "Structures.h"
#include "technicalDefinitions.h"

//draw.c
void drawPiece(piece, unsigned short, unsigned short, sprite, SDL_Renderer*);

SDL_Texture* createPieceTexture(piece Piece, sprite Sprite, SDL_Renderer* renderer)
{

	SDL_Rect DestR;
	DestR.w = CHAR_DIMENSION * Piece.width;
	DestR.h = CHAR_DIMENSION * Piece.height;

	SDL_Texture* texture;
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, DestR.w, DestR.h);

	SDL_SetRenderTarget(renderer, texture);

	//For some reason, you have to clear the texture
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	//Pretty simple, we just draw the piece to the texture
	drawPiece(Piece, 0, 0, Sprite, renderer);
	SDL_SetRenderTarget(renderer, NULL);

	return texture;

}

SDL_Texture* createTexture(SDL_Renderer* renderer, unsigned short width, unsigned short height)
{

	SDL_Rect DestR;
	DestR.w = width;
	DestR.h = height;

	SDL_Texture* foreground;
	foreground = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, DestR.w,
		DestR.h);

	//For some reason, you have to clear the texture
	SDL_SetRenderTarget(renderer, foreground);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_SetRenderTarget(renderer, NULL);

	//Make it so stuff can render on top of it
	SDL_SetTextureBlendMode(foreground, SDL_BLENDMODE_BLEND);

	return foreground;

}

void drawTexture(SDL_Texture* texture, unsigned short X, unsigned short Y, SDL_Renderer* renderer)
{

	//Rect used to draw the texture
	SDL_Rect DestR;
	//Draw texture at X, Y
	DestR.x = X;
	DestR.y = Y;
	DestR.w;
	DestR.h;
	//Get the width and height of the texture
	SDL_QueryTexture(texture, NULL, NULL, &DestR.w, &DestR.h);

	//Copy the texture to the current rendering target
	SDL_RenderCopy(renderer, texture, NULL, &DestR);

}