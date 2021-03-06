#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>

#include "HEADERS/MGF.h"

#define TITLE_TEXTURE_HEIGHT	84
#define TITLE_TEXTURE_WIDTH		240
#define NUM_OF_TITLE_PIECES		19

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
int getIntStringLength(int num, float multiplier);
SDL_Texture* createTexture(int width, int height);
void drawTexture(SDL_Texture* texture, int X, int Y, float multiplier);
void drawPiece(piece Piece, SDL_Texture* dstTexture, unsigned short X, unsigned short Y);
void clearTexture(SDL_Texture* texture);

//file.c
unsigned int loadTop();
unsigned short getLineCount(char* fileName);
char* getOptionName(int line);
int getOptionValue(const char* str);

//generate.c
void delPiece(piece** Piece);
unsigned short calcWidth(piece* Piece);
unsigned short calcHeight(piece* Piece);
void copyPiece(piece* piece1, piece* piece2);

//memory.c
varVector** pushAddress(void** ptr, unsigned short type);

//Draw the playfield
void drawPlayField(SDL_Texture* background, unsigned short size)
{

	int X = 0;
	int Y = 0;

	if (size == 1)
	{

		X = SPRITE_WIDTH * 10;
		Y = 48;

	}

	int width = round(BASE_PLAYFIELD_WIDTH * size) + 2;
	int height = round(BASE_PLAYFIELD_HEIGHT * size) + 2;
    drawRectangle(WALL_SPRITE_ID, background, X, Y, width, height, GRAY, false);
		
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
	printToTexture("TOP   ", content, 0.5 * (contentWidth - getStringLength("TOP   ", 1.0)), 0, 1, WHITE);
	printToTexture(top, content, 0.5 * (contentWidth - getStringLength("000000", 1.0)),
					(FONT_HEIGHT + STRING_GAP), 1, WHITE);
	intToTexture(topScore, content,
					0.5 * (contentWidth - getStringLength("000000", 1.0)) + zeroLength * (FONT_WIDTH + STRING_GAP), 
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
	else if (size == 1)
	{

		X = SPRITE_WIDTH * 14;
		Y = 0;

	}

	//Draw box
    drawRectangle(WALL_SPRITE_ID, background, X, Y, width_in_sprites + 2, height_in_sprites + 2, GRAY, false);

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
	else if (size == 1)
	{

		X = 0;
		Y = 48;

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
		X = SPRITE_WIDTH * (round(BASE_PLAYFIELD_WIDTH * size) + 1);
		Y = FONT_HEIGHT * 17;

	}
	else if (size == 1)
	{

		X = 192;
		Y = 84;

	}

	//Draw box
	drawRectangle(WALL_SPRITE_ID, background, X, Y, width_in_sprites + 2, height_in_sprites + 2, GRAY, false);
	
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
	printToTexture("HOLD", content, 0.5 * (contentWidth - getStringLength("HOLD", 1.0)), 0, 1, WHITE);

	int X = 0;
	int Y = 0;

	if (size == 0 || size == MAX_PIECE_SIZE)
	{

		//Set X and Y
		X = SPRITE_WIDTH * (round(BASE_PLAYFIELD_WIDTH * size) + 1);
		Y = FONT_HEIGHT * 28;

		int width_in_sprites = size + 1;
		int height_in_sprites = size + 2;
		int width_in_pixels = SPRITE_WIDTH * width_in_sprites;

		//Draw filled box
		drawRectangle(WALL_SPRITE_ID, background, X, Y, width_in_sprites + 2, height_in_sprites, GRAY, true);

		//Cut out a box in the middle that is the correct size and in the correct space
		int width_difference = size + 1 - HOLD_TEXTURE_MULTI * width_in_sprites;
		drawRectangle(WALL_SPRITE_ID, background, X + SPRITE_WIDTH * width_difference, Y + SPRITE_HEIGHT,
						ceil(HOLD_TEXTURE_MULTI * width_in_sprites),
						ceil(HOLD_TEXTURE_MULTI * height_in_sprites), BLACK, true); 

		//Draw the 'content' texture
		SDL_SetRenderTarget(globalInstance->renderer, background);
		drawTexture(content, X + SPRITE_WIDTH + 0.5 * (width_in_pixels - contentWidth),
					Y + SPRITE_HEIGHT + STRING_GAP, 1.0);

	}
	else if (size == 1)
	{

		X = 192;
		Y = 132;

		int width_in_sprites = 5;
		int height_in_sprites = size + 2;
		int width_in_pixels = SPRITE_WIDTH * width_in_sprites;

		//Draw box
		drawRectangle(WALL_SPRITE_ID, background, X, Y, width_in_sprites + 2, height_in_sprites + 2, GRAY, false);

		//Draw the 'content' texture
		SDL_SetRenderTarget(globalInstance->renderer, background);
		drawTexture(content, X + SPRITE_WIDTH + 0.5 * (width_in_pixels - contentWidth),
					Y + SPRITE_HEIGHT + STRING_GAP, 1.0);

	}

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

	if (size == 0 || size == MAX_PIECE_SIZE)
	{

		//Set X and Y
		X = SPRITE_WIDTH * (round(BASE_PLAYFIELD_WIDTH * size) + 1);
		Y = FONT_HEIGHT * 37;

	}
	else if (size == 1)
	{

		width_in_sprites = 9;
		height_in_sprites = 3;

		Y = 120;

	}

	int width_in_pixels = SPRITE_WIDTH * width_in_sprites;
	int height_in_pixels = SPRITE_HEIGHT * height_in_sprites;

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

//Function for redrawing the values texture
void updateValuesText(SDL_Texture* texture)
{

	clearTexture(texture);

	//Print ":" after each modifiable option
	printToTexture(":", texture, 0, 0, 1.0, WHITE);
	printToTexture(":", texture, 0, 14, 1.0, WHITE);
	printToTexture(":", texture, 0, 28, 1.0, WHITE);

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

	//Iterate throught every control
	for (unsigned short i = 0; i < NUM_OF_CONTROLS; i++)
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

		name[SDL_strlen(name)] = '\0';

		//Print the name
		printToTexture(name, texture, x, y, 1.0, WHITE);

		//Underline the name
		char underlines[SDL_strlen(name) + 1];
		underlines[SDL_strlen(name)] = '\0';
		for (unsigned short j = 0; j < SDL_strlen(name); j++)
			underlines[j] = '_';

		printToTexture(underlines, texture, x, y + STRING_GAP + 2, 1.0, WHITE);

		//Get the button for the control
		char* button = (char*)SDL_GetKeyName(SDL_GetKeyFromScancode(globalInstance->controls[i].button));
		int len = SDL_strlen(button) + 2;
		char button_string[len + 1];
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

	}

}

//Create the texture that displays for editing your controls
SDL_Texture* create_Controls_Text()
{

	SDL_Texture* texture;

	int width = MAX_PIECE_SIZE * BASE_PLAYFIELD_WIDTH * FONT_WIDTH;
	int height = NUM_OF_CONTROLS * 3 * FONT_HEIGHT + (NUM_OF_CONTROLS * 3 - 1) * STRING_GAP;

	texture = createTexture(width, height);

	updateControlsText(texture, 0, false);

	return texture;

}

//Create a UI list given an arbitrary number of strings
UI_list* _create_list(const char* strings, ...)
{

	//Return NULL, without allocating any memory, if no strings are given
	if (strings == NULL)
		return NULL;

	//Allocate memory for list and underlying UI element
	UI_list* list = calloc(1, sizeof(UI_list));
	list->ui = calloc(1, sizeof(UI));

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
				list->entry_texts = calloc(list->num_entries + 1, sizeof(char*));
			else
				list->entry_texts = 
						realloc(list->entry_texts, (list->num_entries + 2) * sizeof(char*));

			//Copy current_string into the entry_texts array
				//Append a termination character to the end of it
			int len = SDL_strlen(current_string);
			list->entry_texts[list->num_entries] = calloc(len + 1, sizeof(char));
			for (int i = 0; i < len; i++)
				list->entry_texts[list->num_entries][i] = current_string[i];
			list->entry_texts[list->num_entries][len] = '\0';

			list->num_entries++;

		}

        current_string = va_arg(valist, const char*);

    }
	va_end(valist);

	//Find the longest string
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

		printToTexture(str, texture, x, y, 1.0, WHITE);

	}

	return list;

}

//Create the "Modes" UI element on the title screen
UI_list* create_Modes_List()
{

	//Initialize list
	UI_list* list = create_list("MULTRIS", "NUMERICAL", "OPTIONS", "EXIT");

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

	UI_list* list = create_list("1", "2", "3", "4", "5", "6", "7", "8");

	list->selected_entry = 0;

	list->ui->x = 182;
	list->ui->y = 28;

	//This list starts off inactive and only becomes active if the player has highlighted
	//NUMERICAL mode
	list->ui->currentlyInteracting = false;

	return list;

}

//Create the "Options" UI element on the title screen
UI_list* create_Options_List()
{

	UI_list* list = create_list("FULLSCREEN", "VOLUME", "LIMIT FPS", "CONTROLS");

	list->selected_entry = 0;

	list->ui->x = 42;
	list->ui->y = 70;

	list->ui->currentlyInteracting = false;

	return list;

}

//Get the Y position, relative to the renderer, of an entry in a UI list
	//Refers to the top left of the entry string
	//Returns -1 if the given string is not found in the list
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
	free((*list)->ui);
	(*list)->ui = NULL;

	//Free all entries
	for (int i = 0; i < (*list)->num_entries; i++)
	{

		free((*list)->entry_texts[i]);
		(*list)->entry_texts[i] = NULL;

	}
	free((*list)->entry_texts);
	(*list)->entry_texts = NULL;

	//Free list itself
	free(*list);
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

	piece** titlePieces = calloc(NUM_OF_TITLE_PIECES, sizeof(piece));

	piece* currentPiece = NULL;

	unsigned short sizes[] = { 3, 1, 1, 2, 3, 3, 1, 2, 2, 1, 1, 3, 1, 3, 2, 1, 3, 4, 1 };

	for (unsigned short i = 0; i < NUM_OF_TITLE_PIECES; i++)
	{

		currentPiece = malloc(sizeof(*currentPiece));
		currentPiece->numOfBlocks = sizes[i];
		currentPiece->blocks = malloc(currentPiece->numOfBlocks*sizeof(*currentPiece->blocks));

		if (currentPiece != NULL && currentPiece->blocks != NULL)
		{

			currentPiece->color = (rand() % (RED - YELLOW + 1) + YELLOW);

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

			if (titlePieces != NULL)
				titlePieces[i] = currentPiece;

		}

	}

	//Reset random seed to current time
	srand((int)time(NULL));

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
	free(titlePieces);
	titlePieces = NULL;

	return texture;

}


//Return the pieces in the title that will continue falling once the title reaches the 
//bottom of the screen.
	//Also free all pieces in the array passed to the function
piece** getMovingPieces(piece** titlePieces)
{

	piece** movingPieces = malloc(5 * sizeof(**movingPieces));
	
	if (movingPieces != NULL)
	{
		
		//Allocate memory for the block array of each moving piece
			//titlepieces[0], [1], [2], [3], [4], [5], [6], [7], [8], [9], [10], [12], [14],
			//and [18] are the pieces that must continue falling even after the rest of the
			//title has finished
		for (unsigned short i = 0; i < 14; i++)
			movingPieces[i] = malloc(sizeof(*movingPieces[i]));
		movingPieces[0]->blocks = 
					malloc(titlePieces[0]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[1]->blocks = 
					malloc(titlePieces[1]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[2]->blocks = 
					malloc(titlePieces[2]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[3]->blocks = 
					malloc(titlePieces[3]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[4]->blocks = 
					malloc(titlePieces[4]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[5]->blocks = 
					malloc(titlePieces[5]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[6]->blocks = 
					malloc(titlePieces[6]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[7]->blocks = 
					malloc(titlePieces[7]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[8]->blocks = 
					malloc(titlePieces[8]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[9]->blocks = 
					malloc(titlePieces[9]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[10]->blocks = 
					malloc(titlePieces[10]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[11]->blocks = 
					malloc(titlePieces[12]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[12]->blocks = 
					malloc(titlePieces[14]->numOfBlocks * sizeof(*movingPieces[0]->blocks));
		movingPieces[13]->blocks = 
					malloc(titlePieces[18]->numOfBlocks * sizeof(*movingPieces[0]->blocks));

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

	for (unsigned short i = 0; i < NUM_OF_TITLE_PIECES; i++)
		delPiece(&titlePieces[i]);
	free(titlePieces);
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
		Y = malloc(NUM_MOVING_TITLE_PIECES * sizeof(Y[0]));
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

int getScoreDrawX(unsigned short size)
{

	if (size == 0 || size == MAX_PIECE_SIZE)
		return 277;
	else if (size == 1)
		return 192;
	else
		return 277;

}

int getScoreDrawY(unsigned short size)
{

	if (size == 0 || size == MAX_PIECE_SIZE)
		return 70;
	else if (size == 1)
		return 70;
	else
		return 70;

}

int getForegroundX(unsigned short size)
{

	if (size == 0 || size == MAX_PIECE_SIZE)
		return SPRITE_WIDTH;
	else if (size == 1)
		return 132;
	else
		return SPRITE_WIDTH;

}

int getForegroundY(unsigned short size)
{

	if (size == 0 || size == MAX_PIECE_SIZE)
		return SPRITE_HEIGHT;
	else if (size == 1)
		return 60;
	else
		return 0;

}

int getLevelX(unsigned short size, unsigned short level)
{

	int base;
	int offset;

	offset = 0.5 * getIntStringLength(level, 1.0);

	if (size == 0 || size == MAX_PIECE_SIZE)
		base = 318;
	else if (size == 1)
		base = 66;
	else
		base = 318;

	return (base - offset);

}

int getLevelY(unsigned short size, unsigned short level)
{

	if (size == 0 || size == MAX_PIECE_SIZE)
		return 115;
	else if (size == 1)
		return 31;
	else
		return 115;

}

int getLinesX(unsigned short size, unsigned short lines)
{

	int base, offset;
	offset = getIntStringLength(lines, 1.0) / 2;

	if (size == 0 || size == MAX_PIECE_SIZE)
		base = 318;
	else if (size == 1)
		base = 66;
	else
		base = 318;

	return (base - offset);

}

int getLinesY(unsigned short size, unsigned short lines)
{

	if (size == 0 || size == MAX_PIECE_SIZE)
		return 189;
	else if (size == 1)
		return 105;
	else
		return 189;

}

int getNextX(unsigned short size, int width)
{

	int base, offset;
	offset = width / 2;

	if (size == 0 || size == MAX_PIECE_SIZE)
		base = 318;
	else if (size == 1)
		base = 234;
	else
		base = 318;

	return (base - offset);

}

int getNextY(unsigned short size, int height)
{

	int base, offset;
	offset = height / 2;

	if (size == 0 || size == MAX_PIECE_SIZE)
		base = 282;
	else if (size == 1)
		base = 121;
	else
		base = 282;

	return (base - offset);

}

int getHoldX(unsigned short size, int width)
{

	int base, offset;
	offset = width * HOLD_TEXTURE_MULTI / 2;

	if (size == 0 || size == MAX_PIECE_SIZE)
		base = 318;
	else if (size == 1)
		base = 234;
	else
		base = 318;

	return (base - offset);

}

int getHoldY(unsigned short size, int height)
{

	int base, offset;
	offset = height * HOLD_TEXTURE_MULTI / 2;

	if (size == 0 || size == MAX_PIECE_SIZE)
		base = 403;
	else if (size == 1)
		base = 168;
	else
		base = 403;

	return (base - offset);

}

int getFpsX(unsigned short size)
{

	int base, offset;
	offset = getIntStringLength(globalInstance->FPS, 1.0) / 2;

	if (size == 0 || size == MAX_PIECE_SIZE)
		base = 318;
	else if (size == 1)
		base = 66;
	else	
		base = 318;

	return (base - offset);

}

int getFpsY(unsigned short size)
{

	if (size == 0 || size == MAX_PIECE_SIZE)
		return 474;
	else if (size == 1)
		return 150;
	else
		return 474;

}

//Draw extra background tiles to fill in gaps
void drawBackgroundExtras(SDL_Texture* background, unsigned short size)
{

	if (size == 1)
	{

		drawRectangle(WALL_SPRITE_ID, background, 120, 0, 4, 4, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 120, 132, 6, 5, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 0, 180, 10, 1, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 276, 96, 2, 8, GRAY, true);
		drawRectangle(WALL_SPRITE_ID, background, 180, 96, 1, 3, GRAY, true);

	}

}

//Get the width, in pixels, of the entire game content
int getGameWidth(unsigned short size)
{

	if (size == 0 || size == MAX_PIECE_SIZE)
		return 384;
	else if (size == 1)
		return 300;
	else
		return 384;

}

//Get the height, in pixels, of the entire game content
int getGameHeight(unsigned short size)
{

	if (size == 0 || size == MAX_PIECE_SIZE)
		return 504;
	else if (size == 1)
		return 192;
	else
		return 504;

}

int getPausedX(unsigned short size, float multi)
{

	if (size == 0)
		size = MAX_PIECE_SIZE;

	int start = getForegroundX(size);
	int width = round(BASE_PLAYFIELD_WIDTH * size) * SPRITE_WIDTH;

	return (start + 0.5 * (width - getStringLength("PAUSED", multi)));

}

int getPausedY(unsigned short size, float multi)
{

	if (size == 0)
		size = MAX_PIECE_SIZE;

	int start = getForegroundY(size);
	int height = round(BASE_PLAYFIELD_HEIGHT * size) * SPRITE_HEIGHT;

	return (start + 0.5 * (height - FONT_HEIGHT * multi));

}