#include "MGF.h"

#define TITLE_TEXTURE_HEIGHT	84
#define TITLE_TEXTURE_WIDTH		240
#define NUM_OF_TITLE_PIECES		19

//Draw the playfield
void drawPlayField(SDL_Texture* background, unsigned short size, int XOffset)
{

	//Use MAX_PIECE_SIZE if size == 0
		// Also if size > MAX_PIECE_SIZE (Only for Custom mode) because Custom mode will always have the same sized HUD layout for sizes
		// above MAX_PIECE_SIZE
	if (size == 0 || size > MAX_PIECE_SIZE)
		size = MAX_PIECE_SIZE;

	int X = 0;
	int Y = 0;

	//Set correct X and Y value depending on size
	if (size == 1)
	{

		X = SPRITE_WIDTH * 10;
		Y = 48;

	}
	else if (size == 2)
	{

		X = SPRITE_WIDTH * 10;
		Y = SPRITE_HEIGHT * 3;
		
	}
	else if (size == 3) {

		X = SPRITE_WIDTH * 10;
		Y = 12;

	}
	else if (size == 4 || size == 5 || size == 6 || size == 7) {

		X = SPRITE_WIDTH * 10;
		Y = 0;

	}

	// Apply offset. This is used for drawing a second play field in multiplayer mode
	X += XOffset;

	//Set correct width and height depending on size
	int width = SDL_round(BASE_PLAYFIELD_WIDTH * size) + 2;
	int height = SDL_round(BASE_PLAYFIELD_HEIGHT * size) + 2;

	//Draw the play field rectangle
    drawRectangle(WALL_SPRITE_ID, background, X, Y, width, height, GRAY, false);
		
}

//Function for drawing the score box
void drawScoreBox(SDL_Texture* background, unsigned short size, bool inCustomMode, int XOffset)
{

	if (size == 0 || size > MAX_PIECE_SIZE)
		size = MAX_PIECE_SIZE;

	//Save current rendering target
	SDL_Texture* currentTarget = SDL_GetRenderTarget(globalInstance->renderer);

	//Create 'content' texture
	int contentWidth = getStringLength("0000000", 1.0);
	int contentHeight = getStringLength("00000", 1.0);
	SDL_Texture* content = createTexture(contentWidth, contentHeight);

    //Load the top score and create a string for it
	int topScore = loadTop(MODE, inCustomMode);
	char* top_zeros;
	//Figure out how many zeros should be pre-pended to the score
	unsigned short zeroLength = 7 - getIntLength(topScore);
	
	//top_zeros is the string of zeros that prepend the score
	top_zeros = SDL_malloc(zeroLength * sizeof(*top_zeros));
	*(top_zeros + zeroLength) = '\0';
	for (unsigned short i = 0; i < zeroLength; i++)
		*(top_zeros + i) = '0';

	//Print all content to 'content' texture
	int temp = 0.5 * (contentWidth - getStringLength("TOP    ", 1.0));
	printToTexture("TOP    ", content, temp, 0, 1, WHITE);
	printToTexture(top_zeros, content, temp, (FONT_HEIGHT + STRING_GAP), 1, WHITE);
	printToTexture("SCORE", content, temp, (FONT_WIDTH + STRING_GAP) * 3, 1, WHITE);
	temp = temp + zeroLength * (FONT_WIDTH + STRING_GAP);
	intToTexture(topScore, content, temp, (FONT_HEIGHT + STRING_GAP), 1, WHITE);

    int X = 0;
    int Y = 0;

	//Hardcode the width and height of the score box
		//This is the size of the inside of the box
	int width_in_sprites = 9;
	int height_in_sprites = 6;
	int width_in_pixels = SPRITE_WIDTH * width_in_sprites;
	int height_in_pixels = SPRITE_HEIGHT * height_in_sprites;

	//Set X and Y depending on size
    if (size == 0 || size == MAX_PIECE_SIZE)
    {

		//Set X any Y
        X = SPRITE_WIDTH * (SDL_round(BASE_PLAYFIELD_WIDTH * size) + 1);
        Y = 0;

    }
	else if (size == 1)
	{

		X = SPRITE_WIDTH * 14;
		Y = 0;

	}
	else if (size == 2)
	{

		X = SPRITE_WIDTH * 16;
		Y = 0;

	}
	else if (size == 3) {

		X = SPRITE_WIDTH * 19;
		Y = 0;

	}
	else if (size == 4) {

		X = SPRITE_WIDTH * 21;
		Y = 0;

	}
	else if (size == 5) {

		X = SPRITE_WIDTH * 24;
		Y = 24;

	}
	else if (size == 6) {

		X = SPRITE_WIDTH * 26;
		Y = 36;

	}
	else if (size == 7) {

		X = SPRITE_WIDTH * 29;
		Y = 60;

	}
	
	// Apply offset. This is used for drawing a second score box in multiplayer mode
	X += XOffset;

	//Draw box
    drawRectangle(WALL_SPRITE_ID, background, X, Y, width_in_sprites + 2, height_in_sprites + 2, GRAY, false);

	//Draw the 'content' texture
	SDL_SetRenderTarget(globalInstance->renderer, background);
	int content_X = X + SPRITE_WIDTH + 0.5 * (width_in_pixels - contentWidth);
	int content_Y = Y + SPRITE_HEIGHT + 0.5 * (height_in_pixels - contentHeight);
	drawTexture(content, content_X, content_Y, 1.0);

	//Free memory
    SDL_free(top_zeros);
	SDL_DestroyTexture(content);

	//Reset rendering target
	SDL_SetRenderTarget(globalInstance->renderer, currentTarget);

}

//Function for drawing the level box
void drawLevelBox(SDL_Texture* background, unsigned short size, int XOffset)
{

	if (size == 0 || size > MAX_PIECE_SIZE)
		size = MAX_PIECE_SIZE;

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
		X = SPRITE_WIDTH * (SDL_round(BASE_PLAYFIELD_WIDTH * size) + 1);
		Y = FONT_HEIGHT * 7;

	}
	else if (size == 3)
		Y = 24;
	else if (size == 4)
		Y = 36;
	else if (size == 5)
		Y = 72;
	else if (size == 6)
		Y = 96;
	else if (size == 7)
		Y = 132;

	// Apply offset. This is used for drawing a second level box in multiplayer mode
	X += XOffset;

	//Draw box
	drawRectangle(WALL_SPRITE_ID, background, X, Y, width_in_sprites + 2, height_in_sprites + 2, GRAY, false);

	//Draw the 'content' texture
	SDL_SetRenderTarget(globalInstance->renderer, background);
	int content_X = X + SPRITE_WIDTH + 0.5 * (width_in_pixels - contentWidth);
	int content_Y = Y + SPRITE_HEIGHT + 0.5 * (height_in_pixels - contentHeight);
	drawTexture(content, content_X, content_Y, 1.0);

	//Free 'content' memory
	SDL_DestroyTexture(content);

	//Reset rendering target
	SDL_SetRenderTarget(globalInstance->renderer, currentTarget);

}

//Function for drawing the "lines until levelup" box
void drawUntilBox(SDL_Texture* background, unsigned short size, int XOffset)
{

	if (size == 0 || size > MAX_PIECE_SIZE)
		size = MAX_PIECE_SIZE;

	//Save the current rendering target
	SDL_Texture* currentTarget = SDL_GetRenderTarget(globalInstance->renderer);

	//Create 'content' texture
	int contentWidth = getStringLength("LEVELUP", 1.0);
	int contentHeight = getStringLength("0000", 1.0);
	SDL_Texture* content = createTexture(contentWidth, contentHeight);

	//Print all content to 'content' texture
	int five_letter_X = 0.5 * (contentWidth - getStringLength("00000", 1.0));
	int seven_letter_X = 0.5 * (contentWidth - getStringLength("0000000", 1.0));
	printToTexture("LINES", content, five_letter_X, 0, 1, WHITE);
	printToTexture("UNTIL", content, five_letter_X,	(FONT_HEIGHT + STRING_GAP), 1, WHITE);
	printToTexture("LEVELUP", content, seven_letter_X, (FONT_HEIGHT + STRING_GAP) * 2, 1, WHITE);

	int X = 0;
	int Y = 0;

	int width_in_sprites = 9;
	int height_in_sprites = 5;
	int width_in_pixels = SPRITE_WIDTH * width_in_sprites;
	int height_in_pixels = SPRITE_HEIGHT * height_in_sprites;

	if (size == 0 || size == MAX_PIECE_SIZE)
	{

		//Set X and Y
		X = SPRITE_WIDTH * (SDL_round(BASE_PLAYFIELD_WIDTH * size) + 1);
		Y = FONT_HEIGHT * 11;

	}
	else if (size == 1 || size == 2)
	{

		X = 0;
		Y = 48;

	}
	else if (size == 3) {

		X = 0;
		Y = 72;

	}
	else if (size == 4) {

		X = 0;
		Y = 84;

	}
	else if (size == 5) {

		X = 0;
		Y = 120;

	}
	else if (size == 6) {

		X = 0;
		Y = 144;

	}
	else if (size == 7) {

		X = 0;
		Y = 180;

	}

	// Apply offset. This is used for drawing a second until box in multiplayer mode
	X += XOffset;

	//Draw box
	drawRectangle(WALL_SPRITE_ID, background, X, Y, width_in_sprites + 2, height_in_sprites + 2, GRAY, false);
	
	//Draw the 'content' texture
	SDL_SetRenderTarget(globalInstance->renderer, background);
	int content_X = X + SPRITE_WIDTH + 0.5 * (width_in_pixels - contentWidth);
	int content_Y = Y + SPRITE_HEIGHT + 0.5 * (height_in_pixels - contentHeight);
	drawTexture(content, content_X, content_Y, 1.0);

	//Free 'content' memory
	SDL_DestroyTexture(content);

	//Reset rendering target
	SDL_SetRenderTarget(globalInstance->renderer, currentTarget);

}

//Function for drawing the "next" box
void drawNextBox(SDL_Texture* background, unsigned short size, int XOffset)
{

	if (size == 0 || size > MAX_PIECE_SIZE)
		size = MAX_PIECE_SIZE;

	//Save the current rendering target
	SDL_Texture* currentTarget = SDL_GetRenderTarget(globalInstance->renderer);

	//Create 'content' texture
	int contentWidth = getStringLength("NEXT", 1.0);
	int contentHeight = SPRITE_HEIGHT;
	SDL_Texture* content = createTexture(contentWidth, contentHeight);

	//Print all content to 'content' texture
	printToTexture("NEXT", content, 0.5 * (contentWidth - getStringLength("NEXT", 1.0)), 0, 1, WHITE);

	int X = 0;
	int Y = 0;

	//There is a minimum width for this box because the word "NEXT" needs to fit in it
	int width_in_sprites = SDL_max(size + 1, 5);
	int height_in_sprites = size + 2;
	int width_in_pixels = SPRITE_WIDTH * width_in_sprites;

	if (size == 0 || size == MAX_PIECE_SIZE)
	{

		//Set X and Y
		X = SPRITE_WIDTH * (SDL_round(BASE_PLAYFIELD_WIDTH * size) + 1);
		Y = FONT_HEIGHT * 17;

	}
	else if (size == 1)
	{

		X = 192;
		Y = 84;

	}
	else if (size == 2)
	{

		X = 216;
		Y = 84;

	}
	else if (size == 3) {

		X = 252;
		Y = 84;

	}
	else if (size == 4) {

		X = 276;
		Y = 84;

	}
	else if (size == 5) {

		X = 312;
		Y = 108;

	}
	else if (size == 6) {

		X = 324;
		Y = 120;

	}
	else if (size == 7) {

		X = 348;
		Y = 144;

	}

	// Apply offset. This is used for drawing a second NEXT box in multiplayer mode
	X += XOffset;

	//Draw box
	drawRectangle(WALL_SPRITE_ID, background, X, Y, width_in_sprites + 2, height_in_sprites + 2, GRAY, false);
	
	//Draw the 'content' texture
	SDL_SetRenderTarget(globalInstance->renderer, background);
	int content_X = X + SPRITE_WIDTH + 0.5 * (width_in_pixels - contentWidth);
	int content_Y = Y + SPRITE_HEIGHT + STRING_GAP;
	drawTexture(content, content_X, content_Y, 1.0);

	//Free 'content' memory
	SDL_DestroyTexture(content);

	//Reset rendering target
	SDL_SetRenderTarget(globalInstance->renderer, currentTarget);

}

//Function for drawing the "hold" box
void drawHoldBox(SDL_Texture* background, unsigned short size)
{

	if (size == 0 || size > MAX_PIECE_SIZE)
		size = MAX_PIECE_SIZE;

	//Save the current rendering target
	SDL_Texture* currentTarget = SDL_GetRenderTarget(globalInstance->renderer);

	//Create 'content' texture
	int contentWidth = getStringLength("HOLD", 1.0);
	int contentHeight = SPRITE_HEIGHT;
	SDL_Texture* content = createTexture(contentWidth, contentHeight);

	//Print all content to 'content' texture
	printToTexture("HOLD", content, 0.5 * (contentWidth - getStringLength("HOLD", 1.0)), 0, 1, WHITE);

	int X = 0;
	int Y = 0;

	int	width_in_sprites = size + 1;
	int	height_in_sprites = size + 2;
	int	width_in_pixels = SPRITE_WIDTH * width_in_sprites;

	if (size == 0 || size == MAX_PIECE_SIZE)
	{

		//Set X and Y
		X = SPRITE_WIDTH * (SDL_round(BASE_PLAYFIELD_WIDTH * size) + 1);
		Y = FONT_HEIGHT * 28;

		width_in_sprites = size + 1;
		height_in_sprites = size + 2;
		width_in_pixels = SPRITE_WIDTH * width_in_sprites;

		//Draw filled box
		drawRectangle(WALL_SPRITE_ID, background, X, Y, width_in_sprites + 2, height_in_sprites, GRAY, true);

		//Cut out a box in the middle that is the correct size and in the correct space
		int width_difference = size + 1 - HOLD_TEXTURE_MULTI * width_in_sprites;
		int rect_X = X + SPRITE_WIDTH * width_difference;
		int rect_Y = Y + SPRITE_HEIGHT;
		int rect_W = ceil(HOLD_TEXTURE_MULTI * width_in_sprites);
		int rect_H = ceil(HOLD_TEXTURE_MULTI * height_in_sprites);
		drawRectangle(WALL_SPRITE_ID, background, rect_X, rect_Y, rect_W, rect_H, BLACK, true); 

	}
	else if (size == 1)
	{

		X = 192;
		Y = 132;

		width_in_sprites = 5;
		height_in_sprites = size + 2;
		width_in_pixels = SPRITE_WIDTH * width_in_sprites;

		//Draw box
		drawRectangle(WALL_SPRITE_ID,background,X,Y,width_in_sprites+2,height_in_sprites+2,GRAY,false);

	}
	else if (size == 2)
	{

		X = 216;
		Y = 144;

		width_in_sprites = 5;
		height_in_sprites = size + 2;
		width_in_pixels = SPRITE_WIDTH * width_in_sprites;

		//Draw box
		drawRectangle(WALL_SPRITE_ID,background,X,Y,width_in_sprites+2,height_in_sprites+2,GRAY,false);

	}
	else if (size == 3) {

		X = 252;
		Y = 156;

		width_in_sprites = 5;
		height_in_sprites = size + 1;
		width_in_pixels = SPRITE_WIDTH * width_in_sprites;

		//Draw box
		drawRectangle(WALL_SPRITE_ID,background,X,Y,width_in_sprites+2,height_in_sprites+2,GRAY,false);

	}
	else if (size == 4) {

		X = 276;
		Y = 168;

		width_in_sprites = 5;
		height_in_sprites = size + 1;
		width_in_pixels = SPRITE_WIDTH * width_in_sprites;

		//Draw box
		drawRectangle(WALL_SPRITE_ID,background,X,Y,width_in_sprites+2,height_in_sprites+2,GRAY,false);

	}
	else if (size == 5) {

		X = 312;
		Y = 204;

		width_in_sprites = 5;
		height_in_sprites = size + 1;
		width_in_pixels = SPRITE_WIDTH * width_in_sprites;

		//Draw box
		drawRectangle(WALL_SPRITE_ID,background,X,Y,width_in_sprites+2,height_in_sprites+2,GRAY,false);

	}
	else if (size == 6) {

		X = 336;
		Y = 228;

		width_in_sprites = 5;
		height_in_sprites = size + 1;
		width_in_pixels = SPRITE_WIDTH * width_in_sprites;

		//Draw box
		drawRectangle(WALL_SPRITE_ID,background,X,Y,width_in_sprites+2,height_in_sprites+2,GRAY,false);

	}
	else if (size == 7) {

		X = 360;
		Y = 264;

		width_in_sprites = 7;
		height_in_sprites = size + 1;
		width_in_pixels = SPRITE_WIDTH * width_in_sprites;

		//Draw box
		drawRectangle(WALL_SPRITE_ID,background,X,Y,width_in_sprites+2,height_in_sprites+2,GRAY,false);

	}

	//Draw the 'content' texture
	SDL_SetRenderTarget(globalInstance->renderer, background);
	int content_X = X + SPRITE_WIDTH + 0.5 * (width_in_pixels - contentWidth);
	int content_Y = Y + SPRITE_HEIGHT + STRING_GAP;
	drawTexture(content, content_X, content_Y, 1.0);

	//Free 'content' memory
	SDL_DestroyTexture(content);

	//Reset rendering target
	SDL_SetRenderTarget(globalInstance->renderer, currentTarget);

}

//Function for drawing the sizeBag box
void drawSizeBagBox(SDL_Texture* background, unsigned short size)
{

	if (size == 0 || size > MAX_PIECE_SIZE)
		size = MAX_PIECE_SIZE;

	//Save the current rendering target
	SDL_Texture* currentTarget = SDL_GetRenderTarget(globalInstance->renderer);

	//Create the 'content' texture
	int contentWidth = getStringLength("SIZES", 1.0);
	int contentHeight = SPRITE_HEIGHT * 3;
	SDL_Texture* content = createTexture(contentWidth, contentHeight);

	//Print all content to 'content' texture
	printToTexture("SIZES", content, 0.5 * (contentWidth - getStringLength("SIZES", 1.0)), STRING_GAP, 1, WHITE);

	int X = 0;
	int Y = 0;

	int width_in_sprites = 9;
	int height_in_sprites = 3;

	if (size == 0 || size == MAX_PIECE_SIZE)
	{

		//Set X and Y
		X = SPRITE_WIDTH * (SDL_round(BASE_PLAYFIELD_WIDTH * size) + 1);
		Y = FONT_HEIGHT * 37;

	}
	else if (size == 1 || size == 2)
		Y = 120;
	else if (size == 3)
		Y = 144;
	else if (size == 4)
		Y = 156;
	else if (size == 5)
		Y = 192;
	else if (size == 6)
		Y = 216;
	else if (size == 7)
		Y = 252;	

	int width_in_pixels = SPRITE_WIDTH * width_in_sprites;
	int height_in_pixels = SPRITE_HEIGHT * height_in_sprites;

	//Draw box
	drawRectangle(WALL_SPRITE_ID, background, X, Y, width_in_sprites + 2, height_in_sprites + 2, GRAY, false);

	//Draw the 'content' texture
	SDL_SetRenderTarget(globalInstance->renderer, background);
	int content_X = X + SPRITE_WIDTH + 0.5 * (width_in_pixels - contentWidth);
	int content_Y = Y + SPRITE_HEIGHT + 0.5 * (height_in_pixels - contentHeight);
	drawTexture(content, content_X, content_Y, 1.0);

	//Free 'content' memory
	SDL_DestroyTexture(content);

	//Reset rendering target
	SDL_SetRenderTarget(globalInstance->renderer, currentTarget);

}

//Calculate the current mapWidth
int calcMapWidth()
{

	if (MODE == 0)
		return (int)SDL_round(BASE_PLAYFIELD_WIDTH * MAX_PIECE_SIZE);
	else
		return (int)SDL_round(BASE_PLAYFIELD_WIDTH * MODE);

}

//Calculate the current mapHeight
int calcMapHeight()
{

	if (MODE == 0)
		return (int)SDL_round(BASE_PLAYFIELD_HEIGHT * MAX_PIECE_SIZE);
	else
		return (int)SDL_round(BASE_PLAYFIELD_HEIGHT * MODE);

}

//Function for creating the texture for the score
SDL_Texture* create_Score_Text()
{

	SDL_Texture* texture;

	texture = createTexture(7 * (FONT_WIDTH + STRING_GAP), FONT_HEIGHT);

	printToTexture("0000000", texture, 0, 0, 1, WHITE);

	return texture;

}

//Function for creating the texture for the level
SDL_Texture* create_Level_Text()
{

	SDL_Texture* texture;

	texture = createTexture(2 * (FONT_WIDTH + STRING_GAP), FONT_HEIGHT);

	printToTexture("0", texture, 0, 0, 1, RED);

	return texture;

}

//Function for creating the texture for the "lines until levelup" number
SDL_Texture* create_Lines_Text()
{

	SDL_Texture* texture;
	
	texture = createTexture(FONT_WIDTH, FONT_HEIGHT);
	intToTexture(LEVELUP_INCREASE, texture, 0, 0, 1, ORANGE);

	return texture;

}

// Function for creating the texture for the ConnectionMessage in the multiplayer lobby
SDL_Texture* create_ConnectionMessage_Text()
{

	SDL_Texture* texture;

	texture = createTexture(1, 1);

	return texture;

}

//Function for creating the texture for the PAUSED text
SDL_Texture* create_Pause_Text()
{

	SDL_Texture* texture;

	texture = createTexture(6 * (FONT_WIDTH + STRING_GAP), FONT_HEIGHT);
	printToTexture("PAUSED", texture, 0, 0, 1, WHITE);

	return texture;

}

//Function for creating the texture for the foreground
SDL_Texture* create_Foreground_Text()
{

	SDL_Texture* texture;

	texture = createTexture(FONT_WIDTH * MAP_WIDTH, FONT_HEIGHT * MAP_HEIGHT);

	//Make entire foreground black to begin with.
		// This is to deal with rounding errors with CUSTOM_MODE foregrounds
	drawSimpleRect(texture, 0, 0, FONT_WIDTH * MAP_WIDTH, FONT_HEIGHT * MAP_HEIGHT, BLACK);	

	return texture;

}

//Function for creating the texture for the cursor
SDL_Texture* create_Cursor_Text()
{

	SDL_Texture* texture;
	
	texture = createTexture(FONT_WIDTH, FONT_HEIGHT);
	printToTexture(">", texture, 0, 0, 1.0, WHITE);

	return texture;

}

//Create the texture for the two cursors that show the player that they can use the LEFT
//and RIGHT buttons to "slide" the volume
SDL_Texture* create_volSlide_Text()
{

	SDL_Texture* texture;

	int width = 6 * FONT_WIDTH + 5 * STRING_GAP;
	int height = FONT_HEIGHT;
	texture = createTexture(width, height);

	printToTexture("<", texture, 0, 0, 1.0, YELLOW);
	printToTexture(">", texture, width - FONT_WIDTH, 0, 1.0, YELLOW);

	return texture;

}

// Update the ConnectionMessge texture with the passed message
void updateConnectionMessageText(SDL_Texture** texture, char* message)
{

	SDL_DestroyTexture(*texture);

	int width = getStringLength(message, 1.0);
	int height = FONT_HEIGHT;
	*texture = createTexture(width, height);

	printToTexture(message, *texture, 0, 0, 1.0, WHITE);
		
}

// Update the PAUSED texture between saying "PAUSED" and "EXIT?"
void updatePausedText(SDL_Texture* texture, bool exit)
{

	clearTexture(texture);

	if (exit == true)
		printToTexture("EXIT?", texture, 10, 0, 1, WHITE);
	else if (exit == false)
		printToTexture("PAUSED", texture, 0, 0, 1, WHITE);

}

// Function for redrawing the values on the multiplayer lobby screen
void updateConnectionValuesText(SDL_Texture* texture, char* ip, char* port)
{

	clearTexture(texture);

	// Print ":" after each modifiable options
	printToTexture(":", texture, 0, 0, 1.0, WHITE);
	printToTexture(":", texture, 0, 14, 1.0, WHITE);

	// Print the values of the options
	printToTexture(ip, texture, 14, 0, 1.0, WHITE);
	printToTexture(port, texture, 14, 14, 1.0, WHITE);

}

//Function for redrawing the values texture
void updateValuesText(SDL_Texture* texture)
{

	clearTexture(texture);

	//Print ":" after each modifiable option
	printToTexture(":", texture, 0, 0, 1.0, WHITE);
	printToTexture(":", texture, 0, 14, 1.0, WHITE);
	printToTexture(":", texture, 0, 28, 1.0, WHITE);
	printToTexture(":", texture, 0, 42, 1.0, WHITE);
	printToTexture(":", texture, 0, 56, 1.0, WHITE);
	printToTexture(":", texture, 0, 70, 1.0, WHITE);

	//Print FULLSCREEN value
	if (FULLSCREEN_MODE == 0)
		printToTexture("OFF", texture, 14, 0, 1.0, RED);
	else
		printToTexture("ON", texture, 14, 0, 1.0, GREEN);

	//Print VOLUME value
	intToTexture(VOLUME, texture, 14, 14, 1.0, CYAN);

	//Print LIMIT_FPS value
	if (LIMIT_FPS == 0)
		printToTexture("OFF", texture, 14, 28, 1.0, RED);
	else
		printToTexture("ON", texture, 14, 28, 1.0, GREEN);

	//Print SHOW_FPS value
	if (SHOW_FPS == 0)
		printToTexture("OFF", texture, 14, 42, 1.0, RED);
	else
		printToTexture("ON", texture, 14, 42, 1.0, GREEN);

	//Print CENTER_DOT value
	if (CENTER_DOT == 0)
		printToTexture("OFF", texture, 14, 56, 1.0, RED);
	else
		printToTexture("ON", texture, 14, 56, 1.0, GREEN);

	//Print GRAPHICS value
		// OLD == 0. NEW == 1.
	if (GRAPHICS == 0)
		printToTexture("OLD", texture, 14, 70, 1.0, RED);
	else
		printToTexture("NEW", texture, 14, 70, 1.0, GREEN);

}

// Create the texture that displays on the multiplayer lobby screen to connect to a server
SDL_Texture* create_ConnectionValues_Text()
{

	SDL_Texture* texture;

	// There are two options on this screen, "IP" and "PORT"
	int num_options = 2;
	int height = num_options * FONT_HEIGHT + (num_options - 1) * STRING_GAP;
	int widht = 16 * FONT_WIDTH + 15 * STRING_GAP;

	texture = createTexture(widht, height);

	// Initial values for IP and PORT are empty strings
	updateConnectionValuesText(texture, "\0", "\0");

	return texture;

}

//Create the texture that displays the values of the options
SDL_Texture* create_Values_Text()
{

	SDL_Texture* texture;

	//Calculate height and width for value texture
	int num_options = getLineCount("SAVES/options.cfg");
	int height = num_options * FONT_HEIGHT + (num_options - 1) * STRING_GAP;
	int width = 4 * FONT_WIDTH + 3 * STRING_GAP;

	//Create texture
	texture = createTexture(width, height);

	//Print all values to texture
	updateValuesText(texture);

	return texture;

}

//Function for redrawing the controls texture
void updateControlsText(SDL_Texture* texture, int selected_control, bool editing)
{

	clearTexture(texture);

	//Iterate throught every control (+1 for the "RESET CONTROLS" option)
	for (unsigned short i = 0; i < NUM_OF_CONTROLS + 1; i++)
	{

		//Calculate position for current control
		int x = 18;
		int y = i * 3 * (FONT_HEIGHT + STRING_GAP);
		char name[16];

		//Get the name of the control
		if (i == 0)
			SDL_strlcpy(name, "LEFT", 16);
		else if (i == 1)
			SDL_strlcpy(name, "RIGHT", 16);
		else if (i == 2)
			SDL_strlcpy(name, "SOFT DROP", 16);
		else if (i == 3)
			SDL_strlcpy(name, "HARD DROP", 16);
		else if (i == 4)
			SDL_strlcpy(name, "HOLD", 16);
		else if (i == 5)
			SDL_strlcpy(name, "ROTATE CCW", 16);
		else if (i == 6)
			SDL_strlcpy(name, "ROTATE CW", 16);
		else if (i == 7)
			SDL_strlcpy(name, "MIRROR", 16);
		else if (i == 8)
			SDL_strlcpy(name, "SELECT", 16);
		else if (i == 9)
			SDL_strlcpy(name, "EXIT", 16);
		else if (i == 10)
			SDL_strlcpy(name, "DOWN", 16);
		else if (i == 11)
			SDL_strlcpy(name, "UP", 16);
		else if (i == 12)
			SDL_strlcpy(name, "RESET CONTROLS", 16);

		name[SDL_strlen(name)] = '\0';

		//Print the name
		printToTexture(name, texture, x, y, 1.0, WHITE);

		//Underline the name
		char* underlines = SDL_calloc(SDL_strlen(name) + 1, sizeof(char));
		underlines[SDL_strlen(name)] = '\0';
		for (unsigned short j = 0; j < SDL_strlen(name); j++)
			underlines[j] = '_';

		printToTexture(underlines, texture, x, y + STRING_GAP + 2, 1.0, WHITE);
		
		SDL_free(underlines);

		// Don't print a button for the "RESET CONTROLS" option
		if (i != 12)
		{

			//Get the button for the control
			char* button = (char*)SDL_GetKeyName(SDL_GetKeyFromScancode(globalInstance->controls[i].button));
			int len = SDL_strlen(button) + 2;
			char* button_string = SDL_calloc(len + 1, sizeof(char));
			//Surround the button name with brackets
			SDL_strlcpy(button_string, "[", len + 1);
			SDL_strlcat(button_string, button, len + 1);
			SDL_strlcat(button_string, "]", len + 1);
			button_string[len] = '\0';

			//Print the button for the control
				//It's yellow if it's not currently being edited. It's red if it is currently being edited.
			if (editing == true && selected_control == i)
				printToTexture(button_string, texture, x, y + FONT_HEIGHT + 2 + 2 * STRING_GAP, 1.0, RED);
			else
				printToTexture(button_string, texture, x, y + FONT_HEIGHT + 2 + 2 * STRING_GAP, 1.0, YELLOW);

			SDL_free(button_string);

		}

	}

}

// Function for redrawing the custom texture
void updateCustomText(SDL_Texture* texture, unsigned short value)
{

	clearTexture(texture);

	// Print the value to the texture
	intToTexture(value, texture, 0, 0, 1.0, WHITE);

}

// Create top string of sizeBagTexture
char* createSizeBagStringOne()
{

	if (MODE == 0 || CUSTOM_MODE == true)
	{
		
		// maxSize is MAX_PIECE_SIZE if in MULTRIS mode
		int maxSize = MODE;
		if (MODE == 0)
			maxSize = MAX_PIECE_SIZE;

		// Create string for the top line of sizes
		switch (maxSize)
		{

		case 1:
			return SDL_strdup("1");
			break;
		case 2:
			return SDL_strdup("1");
			break;
		case 3:
			return SDL_strdup("1 2");
			break;
		case 4:
			return SDL_strdup("1 2");
			break;
		case 5:
			return SDL_strdup("1 2 3");
			break;
		case 6:
			return SDL_strdup("1 2 3");
			break;
		case 7:
			return SDL_strdup("1 2 3 4");
			break;
		case 8:
			return SDL_strdup("1 2 3 4");
			break;
		case 9:
			return SDL_strdup("1 2 3 4 5");
			break;
		case 10:
			return SDL_strdup("1 2 3 4 5");
			break;
		case 11:
			return SDL_strdup("1 2 3 4 5 6");
			break;
		case 12:
			return SDL_strdup("1 2 3 4 5 6 7");
			break;
		case 13:
			return SDL_strdup("1 2 3 4 5 6 7 8");
			break;
		case 14:
			return SDL_strdup("1 2 3 4 5 6 7 8");
			break;
		case 15:
			return SDL_strdup("1 2 3 4 5 6 7 8 9");
			break;
		default:
			break;

		}

	}

	return NULL;

}

// Create bottom string of sizeBagTexture
char* createSizeBagStringTwo()
{

	// For MULTRIS and CUSTOM mode
	if (MODE == 0 || CUSTOM_MODE == true)
	{

		// maxSize is MAX_PIECE_SIZE if in MULTRIS mode
		int maxSize = MODE;
		if (MODE == 0)
			maxSize = MAX_PIECE_SIZE;

		// Create string for the bottom line of sizes
		switch (maxSize)
		{

		case 1:
			return SDL_strdup("");
			break;
		case 2:
			return SDL_strdup("2");
			break;
		case 3:
			return SDL_strdup("3");
			break;
		case 4:
			return SDL_strdup("3 4");
			break;
		case 5:
			return SDL_strdup("4 5");
			break;
		case 6:
			return SDL_strdup("4 5 6");
			break;
		case 7:
			return SDL_strdup("5 6 7");
			break;
		case 8:
			return SDL_strdup("5 6 7 8");
			break;
		case 9:
			return SDL_strdup("6 7 8 9");
			break;
		case 10:
			return SDL_strdup("6 7 8 9 10");
			break;
		case 11:
			return SDL_strdup("7 8 9 10 11");
			break;
		case 12:
			return SDL_strdup("8 9 10 11 12");
			break;
		case 13:
			return SDL_strdup("9 10 11 12 13");
			break;
		case 14:
			return SDL_strdup("9 10 11 12 13 14");
			break;
		case 15:
			return SDL_strdup("10 11 12 13 14 15");
			break;
		default:
			break;

		}

	}

	return NULL;

}

// Create the texture that displays the sizes currently in the size bag
SDL_Texture* create_SizeBag_Text()
{

	SDL_Texture* texture;

	int height = 1;
	int width = 1;

	// For MULTRIS and CUSTOM mode
	if (MODE == 0 || CUSTOM_MODE == true)
	{

		char* stringOne = createSizeBagStringOne();
		char* stringTwo =  createSizeBagStringTwo();

		// Width of texture is the width of the longer line and height is length of two characters
		width = SDL_max(getStringLength(stringOne, 1.0), getStringLength(stringTwo, 1.0));
		height = getStringLength("00", 1.0);

		// Create texture and print sizes two it
		texture = createTexture(width, height);
		printToTexture(stringOne, texture, 0, 0, 1.0, WHITE);
		printToTexture(stringTwo, texture, 0, FONT_HEIGHT + STRING_GAP, 1.0, WHITE);

		// Free strings
		SDL_free(stringOne);
		SDL_free(stringTwo);

	}
	else	// In NUMERICAL mode, just print the one size
	{

		width = getIntStringLength(MODE, 1.0);
		height = getStringLength("0", 1.0);

		texture = createTexture(width, height);
		intToTexture(MODE, texture, 0, 0, 1.0, WHITE);

	}

	return texture;

}

// Create the texture that displays for playing custom mode
SDL_Texture* create_Custom_Text()
{

	SDL_Texture* texture;

	int height = FONT_HEIGHT;
	int width = 3 * FONT_WIDTH + 2 * STRING_GAP;

	texture = createTexture(width, height);

	updateCustomText(texture, 1);

	return texture;

}

//Create the texture that displays for editing your controls
SDL_Texture* create_Controls_Text()
{

	SDL_Texture* texture;

	int width = MAX_PIECE_SIZE * BASE_PLAYFIELD_WIDTH * FONT_WIDTH;
	int height = (NUM_OF_CONTROLS + 1) * 3 * FONT_HEIGHT + (NUM_OF_CONTROLS * 3) * STRING_GAP;

	texture = createTexture(width, height);

	updateControlsText(texture, 0, false);

	return texture;

}

//Create a UI list given an arbitrary number of strings
UI_list* _create_list(unsigned short color, const char* strings, ...)
{

	//Return NULL, without allocating any memory, if no strings are given
	if (strings == NULL)
		return NULL;

	//Allocate memory for list and underlying UI element
	UI_list* list = SDL_calloc(1, sizeof(UI_list));
	list->ui = SDL_calloc(1, sizeof(UI));

	//Store each argument into entry_texts array
	va_list valist;
	va_start(valist, strings);
	const char* current_string = strings;
	list->num_entries = 0;
	while (true)
    {

        if (current_string == NULL)
            break;
        else
		{

			//Resize entry_texts array
			if (list->num_entries == 0)
				list->entry_texts = SDL_calloc(list->num_entries + 1, sizeof(char*));
			else
				list->entry_texts = SDL_realloc(list->entry_texts, (list->num_entries + 2) * sizeof(char*));

			//Copy current_string into the entry_texts array
				//Append a termination character to the end of it
			int len = SDL_strlen(current_string);
			list->entry_texts[list->num_entries] = SDL_calloc(len + 1, sizeof(char));
			for (int i = 0; i < len; i++)
				list->entry_texts[list->num_entries][i] = current_string[i];
			list->entry_texts[list->num_entries][len] = '\0';

			list->num_entries++;

		}

        current_string = va_arg(valist, const char*);

    }
	va_end(valist);

	//Find the longest string
		//This is used to decide the width of the texture
	char* longest = list->entry_texts[0];
	for (int i = 1; i < list->num_entries; i++)
		if (SDL_strlen(list->entry_texts[i]) > SDL_strlen(longest))
			longest = list->entry_texts[i];

	//Create texture for list
	int width = getStringLength(longest, 1.0);
	int height = FONT_HEIGHT * list->num_entries + (list->num_entries - 1) * STRING_GAP;
	list->ui->texture = createTexture(width, height);

	//Print each entry_text to the list texture
	SDL_Texture* texture = list->ui->texture;
	for (int i = 0; i < list->num_entries; i++)
	{

		char* str = list->entry_texts[i];
		int x = 0;
		int y = (FONT_HEIGHT + STRING_GAP) * i;

		printToTexture(str, texture, x, y, 1.0, color);

	}

	return list;

}

// Create the "Connect" UI element that is displayed on the multiplayer lobby screen for connecting to a server
UI_list* create_Connect_List()
{

	// Initialize the list
	UI_list* list;
	list = create_list(WHITE, "IP", "PORT", "CONNECT");

	list->selected_entry = 0;

	list->ui->x = 28;
	list->ui->y = 14;

	list->ui->currentlyInteracting = true;

	return list;

}

//Create the "Modes" UI element on the title screen
UI_list* create_Modes_List()
{

	//Initialize list
		// Add "MULTIPLAYER" mode if DEBUG = 1 since this is still an experimental feature
	UI_list* list;
	if (DEBUG == false)
		list = create_list(WHITE, "MULTRIS", "NUMERICAL", "CUSTOM", "OPTIONS", "EXIT");
	else
		list = create_list(WHITE, "MULTRIS", "NUMERICAL", "CUSTOM", "MULTIPLAYER", "OPTIONS", "EXIT");

	//First entry is selected by default
	list->selected_entry = 0;

	//Position of Modes UI element
	list->ui->x = 28;
	list->ui->y = 14;

	//This UI element starts off active
	list->ui->currentlyInteracting = true;

	return list;

}

//Create the "Numerical" UI element on the title screen where you choose what size of
//polyomino to play with
UI_list* create_Numerical_List()
{

	UI_list* list = create_list(GREEN, "1", "2", "3", "4", "5", "6", "7", "8");

	//This list has the unique aspect of entries being different colors. This loop is making the appropriate
	//entries have red text
	for (unsigned short i = list->num_entries - 1; i > PROGRESS - 1; i--)
	{

		int texture_height;
		SDL_QueryTexture(list->ui->texture, NULL, NULL, NULL, &texture_height);

		int Y = texture_height - ((list->num_entries - i)) * SPRITE_HEIGHT;
		Y = Y - (list->num_entries - i - 1) * STRING_GAP;

		printToTexture(list->entry_texts[i], list->ui->texture, 0, Y, 1.0, RED);

	}

	list->selected_entry = 0;

	list->ui->x = 182;
	list->ui->y = 28;

	//This list starts off inactive and only becomes active if the player has highlighted
	//NUMERICAL mode
	list->ui->currentlyInteracting = false;

	return list;

}

// Create the "Custom" UI element on the title screen
UI_list* create_Custom_List()
{

	UI_list* list = create_list(YELLOW, "<   >");

	list->selected_entry = 0;

	list->ui->x = 182;
	list->ui->y = 42;

	list->ui->currentlyInteracting = false;
	return list;

}

//Create the "Options" UI element on the title screen
UI_list* create_Options_List()
{

	UI_list* list = create_list(WHITE, "FULLSCREEN", "VOLUME", "LIMIT FPS", "SHOW FPS", "CENTER DOT", "GRAPHICS", "CONTROLS");

	list->selected_entry = 0;

	list->ui->x = 42;

	list->ui->y = 84;
	if (DEBUG == true)
		list->ui->y = 98;

	list->ui->currentlyInteracting = false;

	return list;

}

// Get the multiplier for the size of the sizeBag Texture
float getSizeBagMulti(unsigned short size)
{

	if (size < 10)
		return 0.75;
	else if (size < 12)
		return 0.65;
	else if (size < 14)
		return 0.50;
	else
		return 0.40;

}

//Get the X coord of where to draw the SizeBag to the screen depending on the current game mode
int getSizeBagX(unsigned short size, float multiplier)
{

	int base, offset = 0;

	// For MULTRIS and CUSTOM mode
	if (MODE == 0 || CUSTOM_MODE == true)
	{

		// Get the top and bottom strings
		char* stringOne = createSizeBagStringOne();
		char* stringTwo = createSizeBagStringTwo();

		// Calcuate the offset based on length of the longest string
		offset = SDL_max(getStringLength(stringOne, multiplier), getStringLength(stringTwo, multiplier)) / 2;

		// Free strings
		SDL_free(stringOne);
		SDL_free(stringTwo);

	}
	else	// In NUMERICAL mode, only one size is printed
		offset = getIntStringLength(MODE, multiplier) / 2;

	if (size == 0 || size == MAX_PIECE_SIZE)
		base = 318;
	else if (size == 1 || size == 2 || size == 3 || size == 4 || size == 5 || size == 6 || size == 7)
		base = 66;
	else	
		base = 318;

	return (base - offset);

}

//Get the Y coord of where to draw the SizeBag to the screen depending on the current game mode
int getSizeBagY(unsigned short size)
{

	if (size == 0 || size == MAX_PIECE_SIZE)
		return 472;
	else if (size == 1 || size == 2)
		return 148;
	else if (size == 3)
		return 172;
	else if (size == 4)
		return 184;
	else if (size == 5)
		return 220;
	else if (size == 6)
		return 244;
	else if (size == 7)
		return 280;
	else
		return 472;

}

//Get the Y position, relative to the renderer, of an entry in a UI list
	//Refers to the top left of the entry string
	//Returns -1 if the given string is not found in the list
	//This doesn't account for the list being rendered at multiplier other than 1.0
int getListEntryY(UI_list* list, const char* str)
{

	for (int i = 0; i < list->num_entries; i++)
		if (strcmp(str, list->entry_texts[i]) == 0)
			return list->ui->y + (FONT_HEIGHT + STRING_GAP) * i;
	return -1;

}

//Return the string that is currently selected in the list
const char* getListSelectedString(UI_list* list)
{

	return list->entry_texts[list->selected_entry];

}

//Get the Y position, relative to the renderer, of the highlighted entry in a UI list
int getListSelectedEntryY(UI_list* list)
{

	return getListEntryY(list, getListSelectedString(list));

}

//Function for deleting a UI_list object and freeing the memory it takes up
void delete_UI_list(UI_list** list)
{

	//Free underlying UI element
	SDL_DestroyTexture((*list)->ui->texture);
	(*list)->ui->texture = NULL;
	SDL_free((*list)->ui);
	(*list)->ui = NULL;

	//Free all entries
	for (int i = 0; i < (*list)->num_entries; i++)
	{

		SDL_free((*list)->entry_texts[i]);
		(*list)->entry_texts[i] = NULL;

	}
	SDL_free((*list)->entry_texts);
	(*list)->entry_texts = NULL;

	//Free list itself
	SDL_free(*list);
	*list = NULL;

}

//All the title pieces are hard-coded
piece** makeTitlePieces()
{

	//The seed for the random generation of the title will be the same for the duration of
	//the current game instance
		//This means that the colors of the title will not change unless the game is restarted
			//This is to simplify the way in which the movingPieces in the title are
			//updated
	static time_t title_time;
	static bool first_run = true;
	if (first_run == true)
	{

		title_time = time(NULL);
		first_run = false;

	}
	srand((int)title_time);

	//Allocate memory for array of pieces
	piece** titlePieces = SDL_calloc(NUM_OF_TITLE_PIECES, sizeof(piece));

	piece* currentPiece = NULL;

	//All sizes are hard-coded
	unsigned short sizes[] = { 3, 1, 1, 2, 3, 3, 1, 2, 2, 1, 1, 3, 1, 3, 2, 1, 3, 4, 1 };

	//For every piece
	for (unsigned short i = 0; i < NUM_OF_TITLE_PIECES; i++)
	{

		//Allocate memory for piece and for pieces array of blocks
		currentPiece = SDL_malloc(sizeof(*currentPiece));
		currentPiece->numOfBlocks = sizes[i];
		currentPiece->blocks = SDL_malloc(currentPiece->numOfBlocks*sizeof(*currentPiece->blocks));

		if (currentPiece != NULL && currentPiece->blocks != NULL)
		{

			//Randomize color for piece
			currentPiece->color = (rand() % (RED - YELLOW + 1) + YELLOW);

			//Shapes of every piece are hard-coded
			if (i == 0)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;
				currentPiece->blocks[1].X = 0;
				currentPiece->blocks[1].Y = 1;
				currentPiece->blocks[2].X = 0;
				currentPiece->blocks[2].Y = 2;

			}
			else if (i == 1)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;

			}
			else if (i == 2)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;

			}
			else if (i == 3)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;
				currentPiece->blocks[1].X = 0;
				currentPiece->blocks[1].Y = 1;

			}
			else if (i == 4)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;
				currentPiece->blocks[1].X = 0;
				currentPiece->blocks[1].Y = 1;
				currentPiece->blocks[2].X = 0;
				currentPiece->blocks[2].Y = 2;

			}
			else if (i == 5)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;
				currentPiece->blocks[1].X = 1;
				currentPiece->blocks[1].Y = 0;
				currentPiece->blocks[2].X = 1;
				currentPiece->blocks[2].Y = -1;
			
			}
			else if (i == 6)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;

			}
			else if (i == 7)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;
				currentPiece->blocks[1].X = 0;
				currentPiece->blocks[1].Y = 1;

			}
			else if (i == 8)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;
				currentPiece->blocks[1].X = 1;
				currentPiece->blocks[1].Y = 0;

			}
			else if (i == 9)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;

			}
			else if (i == 10)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;

			}
			else if (i == 11)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;
				currentPiece->blocks[1].X = 0;
				currentPiece->blocks[1].Y = 1;
				currentPiece->blocks[2].X = 0;
				currentPiece->blocks[2].Y = 2;

			}
			else if (i == 12)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;

			}
			else if (i == 13)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;
				currentPiece->blocks[1].X = 0;
				currentPiece->blocks[1].Y = 1;
				currentPiece->blocks[2].X = 0;
				currentPiece->blocks[2].Y = 2;

			}
			else if (i == 14)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;
				currentPiece->blocks[1].X = 0;
				currentPiece->blocks[1].Y = 1;

			}
			else if (i == 15)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;

			}
			else if (i == 16)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;
				currentPiece->blocks[1].X = 0;
				currentPiece->blocks[1].Y = 1;
				currentPiece->blocks[2].X = 0;
				currentPiece->blocks[2].Y = 2;

			}
			else if (i == 17)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;
				currentPiece->blocks[1].X = 1;
				currentPiece->blocks[1].Y = 0;
				currentPiece->blocks[2].X = 1;
				currentPiece->blocks[2].Y = -1;
				currentPiece->blocks[3].X = 1;
				currentPiece->blocks[3].Y = -2;

			}
			else if (i == 18)
			{

				currentPiece->blocks[0].X = 0;
				currentPiece->blocks[0].Y = 0;

			}

			//Calculate the width and height of the current piece
			currentPiece->width = calcWidth(currentPiece);
			currentPiece->height = calcHeight(currentPiece);

			// Create the centerBlock
			createCenterBlock(currentPiece);

			//Append piece to array of title pieces
			if (titlePieces != NULL)
				titlePieces[i] = currentPiece;

		}

	}

	//Reset random seed to current time
		// Add 1 to current time to address bug where first and second piece can be the same if player
		// starts game too quickly after opening the game (within 1 second)
	srand((int)time(NULL) + 1);

	return titlePieces;

}

//Function for creating the Title card texture
SDL_Texture* create_Title_Text()
{

	SDL_Texture* texture;

	texture = createTexture(TITLE_TEXTURE_WIDTH, TITLE_TEXTURE_HEIGHT);

	//The coordinates of each title card piece
	unsigned short pieceX[] = {0,1,2,2,4,5,6,8,8,10,7,8,9,11,12,13,15,17,19};
	unsigned short pieceY[] = {0,1,0,1,0,1,0,0,2,2,4,4,4,4,4,6,4,4,4};

	//Create title card pieces
	piece** titlePieces = makeTitlePieces();

	//Draw the title card
	for (unsigned short i = 0; i < NUM_OF_TITLE_PIECES; i++)
		drawPiece(*titlePieces[i], texture, FONT_WIDTH * pieceX[i], FONT_HEIGHT * pieceY[i]);

	//free titlePieces
	for (unsigned short i = 0; i < NUM_OF_TITLE_PIECES; i++)
		delPiece(&titlePieces[i]);
	SDL_free(titlePieces);
	titlePieces = NULL;

	return texture;

}


//Return the pieces in the title that will continue falling once the title reaches the 
//bottom of the screen.
	//Also free all pieces in the array passed to the function.
		//This is because we use a copy of the title screen pieces, so we want to delete that copy
piece** getMovingPieces(piece** titlePieces)
{

	//Only 14 pieces continue falling when the title hits the bottom of the screen
	piece** movingPieces = SDL_malloc(NUM_MOVING_TITLE_PIECES * sizeof(**movingPieces));
	
	if (movingPieces != NULL)
	{
		
		//Allocate memory for the block array of each moving piece
			//titlepieces[0], [1], [2], [3], [4], [5], [6], [7], [8], [9], [10], [12], [14],
			//and [18] are the pieces that must continue falling even after the rest of the
			//title has finished
		for (unsigned short i = 0; i < NUM_MOVING_TITLE_PIECES; i++)
			movingPieces[i] = SDL_malloc(sizeof(*movingPieces[i]));

		//Allocate memory for the correct number of blocks for each movingPiece
		movingPieces[0]->blocks = SDL_malloc(titlePieces[0]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[1]->blocks = SDL_malloc(titlePieces[1]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[2]->blocks = SDL_malloc(titlePieces[2]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[3]->blocks = SDL_malloc(titlePieces[3]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[4]->blocks = SDL_malloc(titlePieces[4]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[5]->blocks = SDL_malloc(titlePieces[5]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[6]->blocks = SDL_malloc(titlePieces[6]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[7]->blocks = SDL_malloc(titlePieces[7]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[8]->blocks = SDL_malloc(titlePieces[8]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[9]->blocks = SDL_malloc(titlePieces[9]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[10]->blocks = SDL_malloc(titlePieces[10]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[11]->blocks = SDL_malloc(titlePieces[12]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[12]->blocks = SDL_malloc(titlePieces[14]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[13]->blocks = SDL_malloc(titlePieces[18]->numOfBlocks * sizeof(*movingPieces[0]->blocks));

		// Create the centerBlocks for all movingPieces
		createCenterBlock(movingPieces[0]);
		createCenterBlock(movingPieces[1]);
		createCenterBlock(movingPieces[2]);
		createCenterBlock(movingPieces[3]);
		createCenterBlock(movingPieces[4]);
		createCenterBlock(movingPieces[5]);
		createCenterBlock(movingPieces[6]);
		createCenterBlock(movingPieces[7]);
		createCenterBlock(movingPieces[8]);
		createCenterBlock(movingPieces[9]);
		createCenterBlock(movingPieces[10]);
		createCenterBlock(movingPieces[11]);
		createCenterBlock(movingPieces[12]);
		createCenterBlock(movingPieces[13]);

		//Copy the pieces over for each movingPiece
		copyPiece(titlePieces[0], movingPieces[0]);
		copyPiece(titlePieces[1], movingPieces[1]);
		copyPiece(titlePieces[2], movingPieces[2]);
		copyPiece(titlePieces[3], movingPieces[3]);
		copyPiece(titlePieces[4], movingPieces[4]);
		copyPiece(titlePieces[5], movingPieces[5]);
		copyPiece(titlePieces[6], movingPieces[6]);
		copyPiece(titlePieces[7], movingPieces[7]);
		copyPiece(titlePieces[8], movingPieces[8]);
		copyPiece(titlePieces[9], movingPieces[9]);
		copyPiece(titlePieces[10], movingPieces[10]);
		copyPiece(titlePieces[12], movingPieces[11]);
		copyPiece(titlePieces[14], movingPieces[12]);
		copyPiece(titlePieces[18], movingPieces[13]);
		
	}

	//Delete the title pieces that got passed
	for (unsigned short i = 0; i < NUM_OF_TITLE_PIECES; i++)
		delPiece(&titlePieces[i]);
	SDL_free(titlePieces);
	titlePieces = NULL;

	return movingPieces;

}

//This function is for when the title has hit the floor, but a few pieces of it should still 
//continue falling
bool updateTitle(SDL_Texture* texture, piece** movingPieces)
{
	
	//Return if the "update" animation is finished playing
	bool updateOver = false;

	//X values of each piece that moves after the rest of the title has finished dropping
	unsigned short X[] = {0,1,2,2,4,5,6,8,8,10,7,9,12,19};
	//The Y value that each movingPiece should stop at
	unsigned short endY[] = {4,6,4,5,4,5,4,1,3,6,6,6,5,6};
	static double* Y;
	if (Y == NULL)
	{

		//Push the address of Y onto varVector so that it will be freed when the title screen 
		//is exited
        pushAddress((void**)&Y, VARIABLE);

		//Initialize the starting Y values of each movingPiece
		Y = SDL_malloc(NUM_MOVING_TITLE_PIECES * sizeof(Y[0]));
		memcpy(Y,(double[]){0,1,0,1,0,1,0,0,2,2,4,4,4,4},NUM_MOVING_TITLE_PIECES*sizeof(Y[0]));

	}

	//Save colors of every pieces
	unsigned short color[NUM_MOVING_TITLE_PIECES];
	for (unsigned short i = 0; i < NUM_MOVING_TITLE_PIECES; i++)
		color[i] = movingPieces[i]->color;

	//Erase all the pieces that are moving this frame
	for (unsigned short i = 0; i < NUM_MOVING_TITLE_PIECES; i++)
	{

		//Check if the piece is dropping down 1 cell during this frame
		if ((int)(*(Y + i) + INITIAL_SPEED * globalInstance->frame_time) > (int)(*(Y + i)))
		{

			//If it is, set its color to black
				//We set the color to black so that we can redraw the same piece on top of
				//itself. effectively erasing it since it is being drawn over with a black
				//copy of itself
			movingPieces[i]->color = BLACK;

			//Now redraw it
			drawPiece(*movingPieces[i],texture,SPRITE_WIDTH*X[i],SPRITE_HEIGHT*(int)*(Y+i));

			//And then restore the color of the piece
			movingPieces[i]->color = color[i];

		}
		
		//Drop the piece
		if (*(Y + i) <= endY[i])
			*(Y + i) += INITIAL_SPEED * globalInstance->frame_time;	

	}

	//Now, redraw all the pieces that moved this frame
	for (unsigned short i = 0; i < NUM_MOVING_TITLE_PIECES; i++)
	{

		//Again, we only redraw it if it actually changed cells over this frame
		if ((int)(*(Y + i) - INITIAL_SPEED * globalInstance->frame_time) < (int)(*(Y + i)))
			drawPiece(*movingPieces[i],texture,SPRITE_WIDTH*X[i],SPRITE_HEIGHT*(int)*(Y+i));

	}

	//movingPiece[1] has the farthest distance to fall, so once its finished falling we
	//know that the animation is complete
	if (Y[1] >= endY[1])
		updateOver = true;

	return updateOver;

}

//Get the X coordinate of where to draw the score to the screen depending on the current game mode size
int getScoreDrawX(unsigned short size)
{

	if (size == 0 || size == MAX_PIECE_SIZE)
		return 270;
	else if (size == 1)
		return 185;
	else if (size == 2)
		return 210;
	else if (size == 3)
		return 246;
	else if (size == 5)
		return 306;
	else if (size == 6)
		return 330;
	else if (size == 7)
		return 366;
	else
		return 270;

}

//Get the Y coordinate of where to draw the score to the screen depending on the current game mode size
int getScoreDrawY(unsigned short size)
{

	if (size == 0 || size == MAX_PIECE_SIZE)
		return 70;
	else if (size == 1 || size == 2 || size == 3) 
		return 70;
	else if (size == 5)
		return 94;
	else if (size == 6)
		return 106;
	else if (size == 7)
		return 130;
	else
		return 70;

}

//Get the X coordinate of where to draw the foreground to the screen depending on the current game mode size
int getForegroundX(unsigned short size)
{

	if (size == 0 || size == MAX_PIECE_SIZE)
		return SPRITE_WIDTH;
	else if (size == 1)
		return 132;
	else if (size == 2 || size == 3 || size == 4 || size == 5 || size == 6 || size == 7)
		return 132;
	else
		return SPRITE_WIDTH;

}

//Get the Y coordinate of where to draw the foreground to the screen depending on the current game mode size
int getForegroundY(unsigned short size)
{

	if (size == 0 || size == MAX_PIECE_SIZE)
		return SPRITE_HEIGHT;
	else if (size == 1)
		return 60;
	else if (size == 2)
		return 48;
	else if (size == 3)
		return 24;
	else if (size == 4 || size == 5 || size == 6 || size == 7)
		return 12;
	else
		return SPRITE_HEIGHT;

}

//Get the X coordinate of where to draw the level to the screen depending on the current game mode size
int getLevelX(unsigned short size, unsigned short level)
{

	int base;
	int offset;

	offset = 0.5 * getIntStringLength(level, 1.0);

	if (size == 0 || size == MAX_PIECE_SIZE)
		base = 318;
	else if (size == 1 || size == 2 || size == 3 || size == 4 || size == 5 || size == 6 || size == 7)
		base = 66;
	else
		base = 318;

	return (base - offset);

}

//Get the Y coordinate of where to draw the level to the screen depending on the current game mode size
int getLevelY(unsigned short size)
{

	if (size == 0 || size == MAX_PIECE_SIZE)
		return 115;
	else if (size == 1 || size == 2)
		return 31;
	else if (size == 3)
		return 55;
	else if (size == 4) 
		return 67;
	else if (size == 5)
		return 103;
	else if (size == 6)
		return 127;
	else if (size == 7)
		return 163;
	else
		return 115;

}

//Get the X coordinate of where to draw the "lines until levelup" to the screen depending on the current game
//mode size
int getLinesX(unsigned short size, unsigned short lines)
{

	int base, offset;
	offset = getIntStringLength(lines, 1.0) / 2;

	if (size == 0 || size == MAX_PIECE_SIZE)
		base = 318;
	else if (size == 1 || size == 2 || size == 3 || size == 4 || size == 5 || size == 6 || size == 7)
		base = 66;
	else
		base = 318;

	return (base - offset);

}

//Get the Y coordinate of where to draw the "lines until levelup" to the screen depending on the current game
//mode size
int getLinesY(unsigned short size)
{

	if (size == 0 || size == MAX_PIECE_SIZE)
		return 189;
	else if (size == 1 || size == 2)
		return 105;
	else if (size == 3)
		return 129;
	else if (size == 4)
		return 141;
	else if (size == 5)
		return 177;
	else if (size == 6)
		return 201;
	else if (size == 7)
		return 237;
	else
		return 189;

}

//Get the X coord of where to draw the Next piece to the screen depending on the current game mode
int getNextX(unsigned short size, int width)
{

	int base, offset;
	offset = width / 2;

	if (size == 0 || size == MAX_PIECE_SIZE)
		base = 318;
	else if (size == 1)
		base = 234;
	else if (size == 2)
		base = 258;
	else if (size == 3)
		base = 294;
	else if (size == 5)
		base = 360;
	else if (size == 6)
		base = 378;
	else if (size == 7)
		base = 408;
	else
		base = 318;

	return (base - offset);

}

//Get the Y coord of where to draw the Next piece to the screen depending on the current game mode
int getNextY(unsigned short size, int height)
{

	int base, offset;
	offset = height / 2;

	if (size == 0 || size == MAX_PIECE_SIZE)
		base = 282;
	else if (size == 1)
		base = 121;
	else if (size == 2)
		base = 126;
	else if (size == 3)
		base = 132;
	else if (size == 4)
		base = 138;
	else if (size == 5)
		base = 168;
	else if (size == 6)
		base = 186;
	else if (size == 7)
		base = 216;
	else
		base = 282;

	return (base - offset);

}

//Get the X coord of where to draw the Hold piece to the screen depending on the current game mode
int getHoldX(unsigned short size, int width)
{

	int base, offset;
	offset = width * HOLD_TEXTURE_MULTI / 2;

	if (size == 0 || size == MAX_PIECE_SIZE)
		base = 318;
	else if (size == 1)
		base = 234;
	else if (size == 2)
		base = 258;
	else if (size == 3)
		base = 294;
	else if (size == 5)
		base = 354;
	else if (size == 6)
		base = 378;
	else if (size == 7)
		base = 414;
	else
		base = 318;

	return (base - offset);

}

//Get the Y coord of where to draw the Hold piece to the screen depending on the current game mode
int getHoldY(unsigned short size, int height)
{

	int base, offset;
	offset = height * HOLD_TEXTURE_MULTI / 2;

	if (size == 0 || size == MAX_PIECE_SIZE)
		base = 403;
	else if (size == 1)
		base = 168;
	else if (size == 2)
		base = 186;
	else if (size == 3)
		base = 198;
	else if (size == 4)
		base = 216;
	else if (size == 5)
		base = 258;
	else if (size == 6)
		base = 288;
	else if (size == 7)
		base = 330;
	else
		base = 403;

	return (base - offset);

}

//Draw extra background tiles to fill in gaps
void drawBackgroundExtras(SDL_Texture* background, unsigned short size)
{

	//Extra Background walls are specific to the current game mode
	if (size == 1)
	{

		drawRectangle(WALL_SPRITE_ID, background, 120, 0, 4, 4, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 120, 132, 6, 5, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 0, 180, 10, 1, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 276, 96, 2, 8, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 180, 96, 1, 3, GRAY, true);

	}
	else if (size == 2)
	{

		drawRectangle(WALL_SPRITE_ID, background, 120, 0, 6, 3, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 0, 180, 18, 3, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 204, 96, 1, 7, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 300, 96, 2, 10, GRAY, true);

	}
	else if (size == 3) {

		drawRectangle(WALL_SPRITE_ID, background, 0, 0, 10, 2, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 120, 0, 9, 1, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 0, 204, 10, 1, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 0, 216, 21, 1, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 240, 96, 1, 10, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 336, 96, 2, 11, GRAY, true);

	}
	else if (size == 4) {

		drawRectangle(WALL_SPRITE_ID, background, 0, 0, 10, 3, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 0, 216, 10, 4, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 264, 96, 1, 14, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 360, 96, 2, 14, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 276, 252, 7, 1, GRAY, true);

	}
	else if (size == 5) {

		drawRectangle(WALL_SPRITE_ID, background, 0, 0, 10, 6, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 0, 252, 10, 6, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 300, 0, 10, 2, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 300, 300, 10, 2, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 300, 120, 1, 15, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 396, 120, 2, 15, GRAY, true);

	}
	else if (size == 6) {

		drawRectangle(WALL_SPRITE_ID, background, 0, 0, 10, 8, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 0, 276, 10, 9, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 324, 0, 10, 3, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 324, 240, 1, 12, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 420, 240, 1, 12, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 432, 132, 1, 21, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 336, 336, 7, 4, GRAY, true);

	}
	else if (size == 7) {

		drawRectangle(WALL_SPRITE_ID, background, 0, 0, 10, 11, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 0, 312, 10, 11, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 360, 0, 10, 5, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 360, 384, 10, 5, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 468, 156, 1, 19, GRAY, true);

	}

}

//Get the width, in pixels, of the entire game content
int getGameWidth(unsigned short size, bool multiplayer)
{

	int returnValue = 0;

	if (size == 0 || size == MAX_PIECE_SIZE)
		returnValue = 384;
	else if (size == 1)
		returnValue = 300;
	else if (size == 2)
		returnValue = 324;
	else if (size == 3)
		returnValue = 360;
	else if (size == 4)
		returnValue = 384;
	else if (size == 5)
		returnValue = 420;
	else if (size == 6)
		returnValue = 444;
	else if (size == 7)
		returnValue = 480;
	else
		returnValue = 384;

	// Double the width if in a multiplayer game
	if (multiplayer)
		returnValue *= 2;
	
	return returnValue;

}

//Get the height, in pixels, of the entire game content
int getGameHeight(unsigned short size)
{

	if (size == 0 || size == MAX_PIECE_SIZE)
		return 504;
	else if (size == 1)
		return 192;
	else if (size == 2)
		return 216;
	else if (size == 3)
		return 228;
	else if (size == 4)
		return 264;
	else if (size == 5)
		return 324;
	else if (size == 6)
		return 384;
	else if (size == 7)
		return 444;
	else
		return 504;

}

// Get the X coord of where to draw the ConnectionMessage text to the screen in the multiplayer lobby
int getConnectionMessageX(char* message, float multi)
{

	int start = getForegroundX(MAX_PIECE_SIZE);
	int width = SDL_round(BASE_PLAYFIELD_WIDTH * MAX_PIECE_SIZE) * SPRITE_WIDTH;

	return (start + 0.5 * (width - getStringLength(message, multi)));

}

// Get the Y coord of where to draw the ConnectionMessage text to the screen in the multiplayer lobby
int getConnectionMessageY(float multi)
{

	int start = getForegroundY(MAX_PIECE_SIZE);
	int height = SDL_round(BASE_PLAYFIELD_HEIGHT * MAX_PIECE_SIZE) * SPRITE_HEIGHT;

	return (start + 0.5 * (height - FONT_HEIGHT * multi));

}

//Get the X coord of where to draw the Pause text to the screen depending on the current game mode
int getPausedX(unsigned short size, float multi)
{

	// Use size = MAX_PIECE_SIZE for placement if in MULTRIS mode or CUSTOM mode.
	if (size == 0 || size > MAX_PIECE_SIZE)
		size = MAX_PIECE_SIZE;

	int start = getForegroundX(size);
	int width = SDL_round(BASE_PLAYFIELD_WIDTH * size) * SPRITE_WIDTH;

	return (start + 0.5 * (width - getStringLength("PAUSED", multi)));

}

//Get the Y coord of where to draw the Pause text to the screen depending on the current game mode
int getPausedY(unsigned short size, float multi)
{

	if (size == 0 || size > MAX_PIECE_SIZE)
		size = MAX_PIECE_SIZE;

	int start = getForegroundY(size);
	int height = SDL_round(BASE_PLAYFIELD_HEIGHT * size) * SPRITE_HEIGHT;

	return (start + 0.5 * (height - FONT_HEIGHT * multi));

}