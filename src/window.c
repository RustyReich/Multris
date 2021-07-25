#include "HEADERS/Structures.h"
#include "HEADERS/technicalDefinitions.h"

void setWindowMode(unsigned short mode, SDL_Renderer* renderer, SDL_Window* window, SDL_DisplayMode DM)
{

    if (mode == WINDOWED)
    {

        SDL_SetWindowFullscreen(window, 0);

	    //Reset scale on renderer;
		SDL_RenderSetScale(renderer, 1, 1);
		SDL_RenderSetViewport(renderer, NULL);

    }
    else if (mode == FULLSCREEN)
    {

        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

		//Scale renderer accordingly
		SDL_RenderSetScale(renderer, (float)DM.h / WINDOW_BOX_DIMENSION, (float)DM.h / WINDOW_BOX_DIMENSION);
		SDL_Rect rect = { .x = (int)((DM.w * ((float)WINDOW_BOX_DIMENSION / DM.h) - WINDOW_BOX_DIMENSION) / 2), 
			.y = 0, .w = WINDOW_BOX_DIMENSION, .h = WINDOW_BOX_DIMENSION};
		SDL_RenderSetViewport(renderer, &rect);

    }

}