#include "MGF.h"

//Initialize global variables
bool UPDATE_FULLSCREEN_MODE = true;

bool FULLSCREEN_MODE = false;
unsigned short VOLUME = 10;
bool LIMIT_FPS = true;
bool SHOW_FPS = true;
bool CENTER_DOT = true;
bool GRAPHICS = true;

int PROGRESS = 1;

int MODE = 0;
int MAP_WIDTH = 0;
int MAP_HEIGHT = 0;

bool CUSTOM_MODE = false;

int BLOCK_SPRITE_ID = 1;
int WALL_SPRITE_ID = 2;

bool DEBUG = false;

bool MULTIPLAYER = false;

//Global Instance
gameInstance *globalInstance;

int main(int argc, char *argv[]) {

	//Added just to get rid of unused-parameter warning
	(void)argc;
	(void)argv;

	//Initialize SDL stuff
    if( SDL_Init( SDL_INIT_VIDEO ) < 0) {

        fprintf( stderr, "Could not initialize SDL video: %s\n", SDL_GetError() );
        exit( -1 );

    }
	if ( SDL_Init( SDL_INIT_AUDIO ) < 0) {

		fprintf( stderr, "Could not initialize SDL audio: %s\n", SDL_GetError() );
		exit( -1 );

	}

	// Force openGL on Windows because somewhere there is a bug when resizing the window on Direc3D
	#ifdef _WIN32
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
	#endif

	//Set MAP_WIDTH and MAP_HEIGHT
	MAP_WIDTH = calcMapWidth();
	MAP_HEIGHT = calcMapHeight();

	//Set seed for random number generator
	srand((int)time(NULL));

	//Initialize game instance
	initInstance(&globalInstance);

	//Used to check if this is the very first frame of the game
	bool firstLoop = true;

	//Keep track of the number of frames drawn in the last 10th of a second
	int frames_per_DS = 0;

	//Variables for estimating FPS every 100ms
	Uint32 prevTicks = 0;
	int deltaTicks = 0;

	//Variables for limiting FPS
	Uint32 ticksLastFrame = SDL_GetTicks();
	//targetFrameTime will be roughly double the refresh rate of the primary display
	double targetFrameTime = (double)1 / (double)(globalInstance->DM[0].refresh_rate * 2);

	//Create SAVES files if they do not exist or are broken
	if (!fileExists("SAVES/options.cfg") || brokenOptions())
		createOptions();
	if (!fileExists("SAVES/controls.cfg") || brokenControls())
		createControls();
	if (!fileExists("SAVES/window.cfg") || brokenWindowFile())
		createWindowFile();
	if (!fileExists("SAVES/progress.md") || brokenProgress())
		createProgressFile();

	//Now load info from SAVES files

	if (fileExists("SAVES/window.cfg")) {
		
		//Get minimizedWindow size and position
		globalInstance->minimizedWindow_W = getFileValue("SAVES/window.cfg", "WIDTH");
		globalInstance->minimizedWindow_H = getFileValue("SAVES/window.cfg", "HEIGHT");
		globalInstance->minimizedWindow_X = getFileValue("SAVES/window.cfg", "X");
		globalInstance->minimizedWindow_Y = getFileValue("SAVES/window.cfg", "Y");

		int w = globalInstance->minimizedWindow_W;
		int h = globalInstance->minimizedWindow_H;
		int x = globalInstance->minimizedWindow_X;
		int y = globalInstance->minimizedWindow_Y;

		//Set window size
		SDL_SetWindowSize(globalInstance->window, w, h);
		
		//Set window position
		SDL_SetWindowPosition(globalInstance->window, x, y);

	}
	
	if (fileExists("SAVES/controls.cfg")) {

	    globalInstance->controls[LEFT_BUTTON].button = getFileValue("SAVES/controls.cfg", "LEFT");
		globalInstance->controls[RIGHT_BUTTON].button = getFileValue("SAVES/controls.cfg", "RIGHT");
		globalInstance->controls[SOFT_DROP_BUTTON].button = getFileValue("SAVES/controls.cfg", "SOFT DROP");
		globalInstance->controls[HARD_DROP_BUTTON].button = getFileValue("SAVES/controls.cfg", "HARD DROP");
		globalInstance->controls[HOLD_BUTTON].button = getFileValue("SAVES/controls.cfg", "HOLD");
		globalInstance->controls[ROTATE_CCW_BUTTON].button = getFileValue("SAVES/controls.cfg", "ROTATE CCW");
		globalInstance->controls[ROTATE_CW_BUTTON].button = getFileValue("SAVES/controls.cfg", "ROTATE CW");
		globalInstance->controls[MIRROR_BUTTON].button = getFileValue("SAVES/controls.cfg", "MIRROR");
		globalInstance->controls[SELECT_BUTTON].button = getFileValue("SAVES/controls.cfg", "SELECT");
		globalInstance->controls[EXIT_BUTTON].button = getFileValue("SAVES/controls.cfg", "EXIT");
		globalInstance->controls[DOWN_BUTTON].button = getFileValue("SAVES/controls.cfg", "DOWN");
		globalInstance->controls[UP_BUTTON].button = getFileValue("SAVES/controls.cfg", "UP");

	}

	if (fileExists("SAVES/options.cfg")) {

		FULLSCREEN_MODE = getFileValue("SAVES/options.cfg", "FULLSCREEN");
		VOLUME = getFileValue("SAVES/options.cfg", "VOLUME");
		LIMIT_FPS = getFileValue("SAVES/options.cfg", "LIMIT FPS");
		SHOW_FPS = getFileValue("SAVES/options.cfg", "SHOW FPS");
		CENTER_DOT = getFileValue("SAVES/options.cfg", "CENTER DOT");
		GRAPHICS = getFileValue("SAVES/options.cfg", "GRAPHICS");
		DEBUG = getFileValue("SAVES/options.cfg", "DEBUG");

		// Change sprite IDs depending on if using new or old graphics
		if (GRAPHICS == false)
		{

			WALL_SPRITE_ID = 0;
			BLOCK_SPRITE_ID = 3;

		}
		else if (GRAPHICS == true)
		{

			WALL_SPRITE_ID = 2;
			BLOCK_SPRITE_ID = 1;

		}

	}

	if (fileExists("SAVES/progress.md"))
		PROGRESS = getFileValue("SAVES/progress.md", "progress");

	//Update volume to that loaded from options file
	updateVolume();
	
	//Game loop
	while (globalInstance->running) {

		//Poll for events
		while (SDL_PollEvent(&globalInstance->event)) {

			//User quit
            if (globalInstance->event.type == SDL_QUIT)
                globalInstance->running = false;

            if (globalInstance->event.type == SDL_WINDOWEVENT) {

				//Scale rendere if size of window changes
                if (globalInstance->event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
					scaleRenderer(); 

			}

			// Show the cursor if it is currently hidden and it moves
			if (globalInstance->event.type == SDL_MOUSEMOTION)
				if (SDL_ShowCursor(SDL_QUERY) == SDL_DISABLE)
					SDL_ShowCursor(SDL_ENABLE);

		}

		//Update 'controls' states
		updateControls();

		//Clear screen with black
        SDL_SetRenderDrawColor(globalInstance->renderer, 0, 0, 0, 255);
        SDL_RenderClear(globalInstance->renderer);

		//Calcualte frame_time and FPS-------------------------------------------
            //FPS and frame_time get updated every ~100 ms (10 times per second)
        if (!firstLoop) {

			//Increase frames_per_DS counter with each frame
            frames_per_DS++;

			//Now check how many milliseconds have passed since the last time we updated the FPS counter
            deltaTicks = SDL_GetTicks() - prevTicks;

			//If it has been more than 100ms, then we update it
            if (deltaTicks >= 100) {

				//Calculate FPS
				int FPS = frames_per_DS;
				FPS = (int)((FPS - ((float)deltaTicks - 100) * frames_per_DS / deltaTicks) * 10);
				globalInstance->FPS = FPS;
                
				//Reset frames_per_DS counter
				frames_per_DS = 0;

				//Set prevTicks to the current value of SDL_GetTicks so that we can begin tracking frames for the
				//next 100ms
                prevTicks = SDL_GetTicks();

            }

        }
        else {	//In the case that this is the first frame of the game   

			//Set prevTicks to an initial value
            prevTicks = SDL_GetTicks();

            //Scale renderer to fit window
            scaleRenderer();

            //It is no longer the first frame
            firstLoop = false;

        }

		//As long as the FPS is greater than 0, the frame_Time is just the inverse of the FPS
        if (globalInstance->FPS > 0)
            globalInstance->frame_time = (double)1 / (double)globalInstance->FPS;

        //-----------------------------------------------------------------------

		//Call mainLoop() at different intervals depending on if the player has the FPS limiter on or not
		if (LIMIT_FPS) {	//FPS limiter is on

			//Logic for limitting the FPS
				//Basically just sleeps every frame if not enough time passed between frames
			int deltaMS = SDL_GetTicks() - ticksLastFrame;
			if (deltaMS < targetFrameTime * 1000)
				SDL_Delay(targetFrameTime * 1000 - deltaMS);
			ticksLastFrame = SDL_GetTicks();

			mainLoop();

		}
		else
			mainLoop();

		//Render the current frame
		SDL_RenderPresent(globalInstance->renderer);

		//Update window in and out of fullscreen
			//For some reason this has to be done at the end of the frame or else it wont 
			//update correctly on launch
		if (UPDATE_FULLSCREEN_MODE) {

			setWindowMode(FULLSCREEN_MODE);

			//We are not loger updating the fullscreen mode
			UPDATE_FULLSCREEN_MODE = false;

		}

	}

	//Save window size and position to disk when the game closes. But only do this if it is not in fullscreen.
		//This is because we want to saved the minimized window settings, not the fullscreen window settings.
	if (!FULLSCREEN_MODE)
		saveWindowSettings();

	//Save option values to optionsFile when game closes
	saveToFile("SAVES/options.cfg", "FULLSCREEN", FULLSCREEN_MODE);
	saveToFile("SAVES/options.cfg", "VOLUME", VOLUME);
	saveToFile("SAVES/options.cfg", "LIMIT FPS", LIMIT_FPS);
	saveToFile("SAVES/options.cfg", "SHOW FPS", SHOW_FPS);
	saveToFile("SAVES/options.cfg", "CENTER DOT", CENTER_DOT);
	saveToFile("SAVES/options.cfg", "GRAPHICS", GRAPHICS);

	//Close SDL stuff 
	SDL_DestroyRenderer(globalInstance->renderer);
	SDL_DestroyWindow(globalInstance->window);
	SDL_Quit();

	//Return 0 upon successful run
	return 0;

}