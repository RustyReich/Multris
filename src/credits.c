#include "MGF.h"

unsigned short creditsScreen(piece** Piece)
{

    //Variables
    DECLARE_VARIABLE(int, nextText_Width, 0);
    DECLARE_VARIABLE(int, nextText_Height, 0);
    DECLARE_VARIABLE(bool, firstLoop, true);

    //Textures
    static SDL_Texture* Texture_Score; declare_HUD_Text(&Texture_Score, SCORE_TEXT);
    static SDL_Texture* Texture_Level; declare_HUD_Text(&Texture_Level, LEVEL_TEXT);
    static SDL_Texture* Texture_Lines; declare_HUD_Text(&Texture_Lines, LINES_TEXT);
    static SDL_Texture* Texture_Next; declare_Piece_Text(&Texture_Next, *Piece, false);
    static SDL_Texture* Texture_Credits; declare_HUD_Text(&Texture_Credits, CREDITS_TEXT);

    //Get the width and height of the Next Piece texture during the first frame
    if (*firstLoop == true)
    {

        SDL_QueryTexture(Texture_Next, NULL, NULL, nextText_Width, nextText_Height);
        *firstLoop = false;

    }

    // Control Logic ---------------------------------------------------

    //Exit credits screen if EXIT_BUTTON or SELECT_BUTTON is pressed
    if (onPress(EXIT_BUTTON) || onPress(SELECT_BUTTON))
    {
        playSound(LAND_SOUND);
        freeVars();
        return RESET;
    }

    // Rendering --------------------------------------------------------
    
	drawTexture(Texture_Score, getScoreDrawX(MODE), getScoreDrawY(MODE), 1.0);
    drawTexture(Texture_Level, 312, 115, 1.0);
    drawTexture(Texture_Lines, 312, 189, 1.0);
    drawTexture(Texture_Next, 318 - (*nextText_Width / 2), 282 - (*nextText_Height / 2), 1.0);
    drawTexture(Texture_Credits, SPRITE_WIDTH * 2, SPRITE_HEIGHT * 2, 1.0);

    // ------------------------------------------------------------------

    return CREDITS_SCREEN;
    
}