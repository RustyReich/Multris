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

//memory.c
void declare_HUD_Text(SDL_Texture** ptr, int type);
void declare_Piece_Text(SDL_Texture** ptr, piece* Piece);
void declare_unsigned_short(void** ptr, unsigned short value);
void declare_double(void** ptr, double value);
void declare_int(void** ptr, int value);
void declare_char(void** ptr, char value);
void declare_bool(void** ptr, bool value);
void declare_unsigned_int(void** ptr, unsigned int value);
void declare_UI_list(UI_list** ptr, int type);
void freeVars();

unsigned short controlsScreen(piece** Piece)
{

    //Variables
    static int* nextText_Width; declare(nextText_Width, 0);
    static int* nextText_Height; declare(nextText_Height, 0);
    static bool* firstLoop; declare(firstLoop, true);  
    static unsigned short* selected_control; declare(selected_control, 0);  

    //Textures
    static SDL_Texture* Texture_Score; declare_HUD_Text(&Texture_Score, SCORE_TEXT);
    static SDL_Texture* Texture_Level; declare_HUD_Text(&Texture_Level, LEVEL_TEXT);
    static SDL_Texture* Texture_Lines; declare_HUD_Text(&Texture_Lines, LINES_TEXT);
    static SDL_Texture* Texture_Next; declare_Piece_Text(&Texture_Next, *Piece);
    static SDL_Texture* Texture_Controls; declare_HUD_Text(&Texture_Controls, CONTROLS_TEXT);
    static SDL_Texture* Texture_Cursor; declare_HUD_Text(&Texture_Cursor, CURSOR_TEXT);

    if (*firstLoop == true)
    {

        SDL_QueryTexture(Texture_Next, NULL, NULL, nextText_Width, nextText_Height);

        *firstLoop = false;

    }

    // Control Logic ---------------------------------------------------

    if (onPress(DOWN_BUTTON))
    {

        if (*selected_control < NUM_OF_CONTROLS - 1)
            (*selected_control)++;

    }

    if (onPress(UP_BUTTON))
    {

        if (*selected_control > 0)
            (*selected_control)--;

    }

    if (onPress(EXIT_BUTTON))
    {

        freeVars();
        return RESET;

    }

    // -----------------------------------------------------------------

    // Rendering --------------------------------------------------------
    
    drawTexture(Texture_Score, 277, 70, 1.0);
    drawTexture(Texture_Level, 312, 115, 1.0);
    drawTexture(Texture_Lines, 312, 189, 1.0);
    drawTexture(Texture_Next, 318 - (*nextText_Width / 2), 282 - (*nextText_Height / 2), 1.0);
    drawTexture(Texture_Controls, 12, 14, 0.9);
    drawTexture(Texture_Cursor, 14, 14 + ((*selected_control) * 3 * (FONT_HEIGHT + STRING_GAP)) * 0.9, 1.0);

    // ------------------------------------------------------------------

    return CONTROLS_SCREEN;

}

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