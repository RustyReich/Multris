#include <stdbool.h>

#include "HEADERS/MGF.h"

//draw.c
SDL_Texture* createTexture(int width, int height);
void drawTexture(SDL_Texture* texture, int X, int Y, float multiplier);
void printToTexture(char* string, SDL_Texture* dstTexture, int X, int Y, 
	float multiplier, unsigned short color);

//audio.c
void playSound(sound* Sound);
sound* loadSound(char* file);
void delSound(sound** Sound);
void setVolume(sound** Sound, unsigned short volume);

void updateControls()
{

    //Iterate over all controls
    for (int i = 0; i < NUM_OF_CONTROLS; i++)
    {
        
        //Update logic
        if (globalInstance->keys[globalInstance->controls[i].button])
        {

            if (globalInstance->controls[i].onHold == false)
            {

                globalInstance->controls[i].onPress = true;
                globalInstance->controls[i].onHold = true;

            }
            else
                globalInstance->controls[i].onPress = false;

        }
        else
            globalInstance->controls[i].onHold = false;

    }

}

void setControls()
{

    //Valid buttons are SDL_SCANCODE_A to SDL_SCANCODE_NUMLOCKCLEAR

}

unsigned short controlsScreen()
{

    //Get current keyboard state
	static Uint8* keys = NULL;
	if (keys == NULL)
		keys = (Uint8*)SDL_GetKeyboardState(NULL);

    //Textures
    static SDL_Texture* Texture_Controls;
    if (Texture_Controls == NULL)
    {

        //Get playspace area
        unsigned short mapWidth = round(BASE_PLAYFIELD_WIDTH * MAX_PIECE_SIZE);
        unsigned short mapHeight = round(BASE_PLAYFIELD_HEIGHT * MAX_PIECE_SIZE);

        //Make the texture the size of the play area
        Texture_Controls = createTexture(FONT_WIDTH * mapWidth, 
            FONT_HEIGHT * mapHeight);

        //Print controls to texture
        printToTexture("LEFT RIGHT ARROWS : MOVE", Texture_Controls, 
            1 * (FONT_WIDTH + STRING_GAP), 1 * (FONT_HEIGHT + STRING_GAP), 1, 
            WHITE);
        printToTexture("DOWN ARROW : SOFT DROP", Texture_Controls, 
            1 * (FONT_WIDTH + STRING_GAP), 2 * (FONT_HEIGHT + STRING_GAP), 1, 
            WHITE);
        printToTexture("UP ARROW : HARD DROP", Texture_Controls, 
            1 * (FONT_WIDTH + STRING_GAP), 3 * (FONT_HEIGHT + STRING_GAP), 1, 
            WHITE);
        printToTexture("SPACE : HOLD", Texture_Controls, 
            1 * (FONT_WIDTH + STRING_GAP), 4 * (FONT_HEIGHT + STRING_GAP), 1, 
            WHITE);
        printToTexture("Z : ROTATE CCW", Texture_Controls, 
            1 * (FONT_WIDTH + STRING_GAP), 5 * (FONT_HEIGHT + STRING_GAP), 1, 
            WHITE);
        printToTexture("X : ROTATE CW", Texture_Controls, 
            1 * (FONT_WIDTH + STRING_GAP), 6 * (FONT_HEIGHT + STRING_GAP), 1, 
            WHITE);
        printToTexture("C : MIRROR PIECE", Texture_Controls, 
            1 * (FONT_WIDTH + STRING_GAP), 7 * (FONT_WIDTH + STRING_GAP), 1, 
            WHITE);
        printToTexture("ENTER : SELECT PAUSE", Texture_Controls, 
            1 * (FONT_WIDTH + STRING_GAP), 8 * (FONT_HEIGHT + STRING_GAP), 1, 
            WHITE);
        printToTexture("ESC : EXIT GAME FROM MENU", Texture_Controls, 
            1 * (FONT_WIDTH + STRING_GAP), 9 * (FONT_HEIGHT + STRING_GAP), 1, 
            WHITE);

    }

    //Sounds
    static sound* Sound_Rotate;
    if (Sound_Rotate == NULL)
    {

        Sound_Rotate = loadSound("AUDIO/rotate.wav");
        setVolume(&Sound_Rotate, globalInstance->global_volume * 10);

    }

    //Rendering --------------------------------------------------------

    drawTexture(Texture_Controls, FONT_WIDTH, FONT_HEIGHT, 1.0);

    static char* inputLock;		//This prevents the user from holding down certain buttons
	if (inputLock == NULL)
	{

		inputLock = malloc(sizeof(*inputLock));
		if (inputLock != NULL)
			*inputLock = true;
    }
    else
    {

        //Return to title screen if ENTER is pressed
        if (keys[SDL_SCANCODE_RETURN] && *inputLock == false)
        {
            
            //play ROTATE sound
            playSound(Sound_Rotate);

            //Free Textures
            SDL_DestroyTexture(Texture_Controls);
            Texture_Controls = NULL;

            //Free sounds
            delSound(&Sound_Rotate);

            //Reset keys array pointer
            keys = NULL;

            //Free Single variables
            free(inputLock);
            inputLock = NULL;

            return RESET;

        }
        else if (!keys[SDL_SCANCODE_RETURN])
            *inputLock = false;

    }

    return CONTROLS_SCREEN;

}