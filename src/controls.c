#include <stdbool.h>

#include "HEADERS/MGF.h"

//draw.c
void drawTexture(SDL_Texture* texture, int X, int Y, float multiplier);

//audio.c
void _playSound(int id);

//memory.c
void declare_HUD_Text(SDL_Texture** ptr, int type);
void declare_Piece_Text(SDL_Texture** ptr, piece* Piece, bool drawCenterDot);
void declare_unsigned_short(void** ptr, unsigned short value);
void declare_double(void** ptr, double value);
void declare_int(void** ptr, int value);
void declare_char(void** ptr, char value);
void declare_bool(void** ptr, bool value);
void declare_unsigned_int(void** ptr, unsigned int value);
void declare_short(void** ptr, short value);
void declare_UI_list(UI_list** ptr, int type);
void freeVars();

//layout.c
void updateControlsText(SDL_Texture* texture, int selected_control, bool editing);
int getScoreDrawX(unsigned short size);
int getScoreDrawY(unsigned short size);

//file.c
void saveToFile(const char* file_path, const char* str, int value);

int getPressedKey();

//Lookup table for control names
static const char* control_name[] =
{

    [0]     = "LEFT",
    [1]     = "RIGHT",
    [2]     = "SOFT DROP",
    [3]     = "HARD DROP",
    [4]     = "HOLD",
    [5]     = "ROTATE CCW",
    [6]     = "ROTATE CW",
    [7]     = "MIRROR",
    [8]     = "SELECT",
    [9]     = "EXIT",
    [10]    = "DOWN",
    [11]    = "UP",

};

unsigned short controlsScreen(piece** Piece)
{

    //Variables
    DECLARE_VARIABLE(int, nextText_Width, 0);
    DECLARE_VARIABLE(int, nextText_Height, 0);
    DECLARE_VARIABLE(bool, firstLoop, true);
    DECLARE_VARIABLE(unsigned short, selected_control, 0);
    DECLARE_VARIABLE(bool, editing_control, false);
    DECLARE_VARIABLE(int, new_key, -1);

    //Textures
    static SDL_Texture* Texture_Score; declare_HUD_Text(&Texture_Score, SCORE_TEXT);
    static SDL_Texture* Texture_Level; declare_HUD_Text(&Texture_Level, LEVEL_TEXT);
    static SDL_Texture* Texture_Lines; declare_HUD_Text(&Texture_Lines, LINES_TEXT);
    static SDL_Texture* Texture_Next; declare_Piece_Text(&Texture_Next, *Piece, false);
    static SDL_Texture* Texture_Controls; declare_HUD_Text(&Texture_Controls, CONTROLS_TEXT);
    static SDL_Texture* Texture_Cursor; declare_HUD_Text(&Texture_Cursor, CURSOR_TEXT);

    //Get the width and height of the Next Piece texture during the first frame
    if (*firstLoop == true)
    {

        SDL_QueryTexture(Texture_Next, NULL, NULL, nextText_Width, nextText_Height);
        *firstLoop = false;

    }

    // Control Logic ---------------------------------------------------

    if (onPress(SELECT_BUTTON))
    {

        //If you press SELECT when you're not currently editing a control, you start editing the currently
        //selected control
        if ((*editing_control) == false)
        {

            playSound(ROTATE_SOUND);
            *editing_control = true;

            //Update controls texture, which will make the currently edited control red
            updateControlsText(Texture_Controls, *selected_control, *editing_control);

        }
    
    }
    else
    {

        //If you're editing the currently selected control
            //Make sure that SELECT is not being held down. This is to allow you to reset the current SELECT
            //button
        if (*editing_control && !onHold(SELECT_BUTTON))
        {

            //Get the currently pressed key
            int key = getPressedKey();

            //If they key that is currently pressed is a valid key, set it to new_key
            if (key != -1 && *new_key == -1)
                *new_key = key;

            //Once we have a valid key, we wait until that key is no longer being pressed
                //This means that the control is only set upon the release of that new key
                    //This is to avoid the control being immediately activated since it's new button is pressed
            if (*new_key != -1 && !globalInstance->keys[*new_key])
            {

                playSound(ROTATE_SOUND);

                //Set button for selected_control to the new key and get out of editing_control mode
                globalInstance->controls[*selected_control].button = *new_key;
                *editing_control = false;

                //Save this control
                saveToFile("SAVES/controls.cfg", control_name[*selected_control], *new_key);

                //Update the controls texture, returning the selected_control back to yellow
                updateControlsText(Texture_Controls, *selected_control, *editing_control);

                //Reset new_key to -1
                *new_key = -1;

            }

        }

    }

    //Controls for moving up and down the list of controls
    if (onPress(DOWN_BUTTON) && !(*editing_control))
    {
        if (*selected_control < NUM_OF_CONTROLS - 1)
        {
            playSound(MOVE_SOUND);
            (*selected_control)++;
        }
    }
    if (onPress(UP_BUTTON) && !(*editing_control))
    {
        if (*selected_control > 0)
        {
            playSound(MOVE_SOUND);
            (*selected_control)--;
        }
    }

    //Exit controls screen if EXIT_BUTTON is pressed
    if (onPress(EXIT_BUTTON) && !(*editing_control))
    {
        playSound(LAND_SOUND);
        freeVars();
        return RESET;
    }

    // -----------------------------------------------------------------

    // Rendering --------------------------------------------------------
    
	drawTexture(Texture_Score, getScoreDrawX(MODE), getScoreDrawY(MODE), 1.0);
    drawTexture(Texture_Level, 312, 115, 1.0);
    drawTexture(Texture_Lines, 312, 189, 1.0);
    drawTexture(Texture_Next, 318 - (*nextText_Width / 2), 282 - (*nextText_Height / 2), 1.0);
    drawTexture(Texture_Controls, 12, 14, 0.9);
    drawTexture(Texture_Cursor, 14, 14 + ((*selected_control) * 3 * (FONT_HEIGHT + STRING_GAP)) * 0.9, 1.0);

    // ------------------------------------------------------------------

    return CONTROLS_SCREEN;

}

//Function for updating the onPress and onHold status of a control
void updateControls()
{

    //Iterate over all controls
    for (int i = 0; i < NUM_OF_CONTROLS; i++)
    {
        
        //Update logic
            //If button[i] is currently pushed
        if (globalInstance->keys[globalInstance->controls[i].button])
        {

            //If this is the first frame of the button being pushed
            if (globalInstance->controls[i].onHold == false)
            {

                //onHold and onPress are true
                globalInstance->controls[i].onPress = true;
                globalInstance->controls[i].onHold = true;

            }
            else
                //onPress is only true for a single frame
                globalInstance->controls[i].onPress = false;

            // Hide cursor if a button is pressed and the cursor is currently visible
            if (SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE)
                SDL_ShowCursor(SDL_DISABLE);

        }
        else
        {

            //If button is not being pushed, onHold and onPress are both false
            globalInstance->controls[i].onHold = false;
            globalInstance->controls[i].onPress = false;

        }

    }

}

//Get the key that is currently pressed
    //Returns -1 if no valid keys are pressed
    //If multiple are pressed, it returns the one that comes earliest in the SCANCODE list
int getPressedKey()
{

    //Valid buttons are SDL_SCANCODE_A to SDL_SCANCODE_NUMLOCKCLEAR
    for (int i = SDL_SCANCODE_A; i <= SDL_SCANCODE_NUMLOCKCLEAR; i++)
        if (globalInstance->keys[i])
            return i;
    return -1;

}

//Check if the specified key ID is out of range of valid keys
bool invalidKey(int key)
{

    return (key < SDL_SCANCODE_A || key > SDL_SCANCODE_NUMLOCKCLEAR);

}