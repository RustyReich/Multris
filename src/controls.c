#include <stdbool.h>

#include "HEADERS/Structures.h"
#include "HEADERS/technicalDefinitions.h"

//texture.c
SDL_Texture* createTexture(SDL_Renderer*, unsigned short, unsigned short);
void drawTexture(SDL_Texture*, unsigned short, unsigned short, float, SDL_Renderer*);

//audio.c
void playSound(sound*, SDL_AudioDeviceID*, SDL_AudioSpec*);
sound* loadSound(char*, SDL_AudioSpec*);
void delSound(sound**);
void setVolume(sound**, SDL_AudioSpec*, unsigned short);

//draw.c
void print_string(char*, unsigned short, unsigned short, unsigned short, unsigned short, SDL_Renderer*, sprite*);

unsigned short controlsScreen(sprite* Sprites, SDL_AudioDeviceID* audioDevice, SDL_AudioSpec* wavSpec, SDL_Renderer* renderer)
{

    //Get current keyboard state
	static Uint8* keys = NULL;
	if (keys == NULL)
		keys = (Uint8*)SDL_GetKeyboardState(NULL);

    //Textures
    static SDL_Texture* Texture_Controls;
    if (Texture_Controls == NULL)
    {

        //Make the texture the size of the play area
        Texture_Controls = createTexture(renderer, CHAR_DIMENSION * WIDTH_OF_PLAYSPACE, 
                                        CHAR_DIMENSION * (HEIGHT_IN_CHARS - 2));

        //Print controls to texture
        SDL_SetRenderTarget(renderer, Texture_Controls);
        print_string("LEFT RIGHT ARROWS : MOVE", 1 * (CHAR_DIMENSION + LETTER_GAP), 1 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer, Sprites);
        print_string("DOWN ARROW : SOFT DROP", 1 * (CHAR_DIMENSION + LETTER_GAP), 2 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer, Sprites);
        print_string("UP ARROW : HARD DROP", 1 * (CHAR_DIMENSION + LETTER_GAP), 3 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer, Sprites);
        print_string("SPACE : HOLD", 1 * (CHAR_DIMENSION + LETTER_GAP), 4 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer, Sprites);
        print_string("Z : ROTATE CCW", 1 * (CHAR_DIMENSION + LETTER_GAP), 5 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer, Sprites);
        print_string("X : ROTATE CW", 1 * (CHAR_DIMENSION + LETTER_GAP), 6 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer, Sprites);
        print_string("C : MIRROR PIECE", 1 * (CHAR_DIMENSION + LETTER_GAP), 7 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer, Sprites);
        print_string("ENTER : SELECT PAUSE", 1 * (CHAR_DIMENSION + LETTER_GAP), 8 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer, Sprites);
        print_string("ESC : EXIT GAME FROM MENU", 1 * (CHAR_DIMENSION + LETTER_GAP), 9 * (CHAR_DIMENSION + LETTER_GAP), 1, WHITE, renderer, Sprites);
        SDL_SetRenderTarget(renderer, NULL);

    }

    //Sounds
    static sound* Sound_Rotate;
    if (Sound_Rotate == NULL)
    {

        Sound_Rotate = loadSound("AUDIO/rotate.wav", wavSpec);
        setVolume(&Sound_Rotate, wavSpec, GLOBAL_VOLUME * 10);

    }

    //Rendering --------------------------------------------------------

    drawTexture(Texture_Controls, CHAR_DIMENSION, CHAR_DIMENSION, 1.0, renderer);

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
            playSound(Sound_Rotate, audioDevice, wavSpec);

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