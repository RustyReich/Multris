#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include "HEADERS/MGF.h"

#define INITIAL_INTERNAL_WIDTH	384
#define INITIAL_INTERNAL_HEIGHT	504

//audio.c
sound* loadSound(char* file);

//file.c
void saveToFile(const char* file_path, const char* str, int value);

//Function for initializing an audio device
SDL_AudioDeviceID* prepareAudioDevice(SDL_AudioSpec** wavSpec)
{

    //Populate wavSpec structure
    *wavSpec = calloc(1, sizeof(**wavSpec));
    Uint32 wavLength;
    Uint8* wavBuffer;
    //All sound files have the same specs as specs.wav, so we can use it to load
    //the audio specs
    SDL_LoadWAV("AUDIO/specs.wav", (*wavSpec), &wavBuffer, &wavLength);

    //Open the Audio device
    SDL_AudioDeviceID* device;
    device = calloc(1, sizeof(*device));
    *device = SDL_OpenAudioDevice(NULL, 0, *wavSpec, NULL, 0);

    //Free the memory taken by loading "specs.wav"
    SDL_FreeWAV(wavBuffer);

    return device;

}

control* initControls()
{

    //Allocate memory for controls
    control* controls;
    controls = calloc(NUM_OF_CONTROLS, sizeof(*controls));

    //Set default controls
        //Control ID's are defined in Definitions.h
    controls[LEFT_BUTTON].button = SDL_SCANCODE_LEFT;
    controls[RIGHT_BUTTON].button = SDL_SCANCODE_RIGHT;
    controls[SOFT_DROP_BUTTON].button = SDL_SCANCODE_DOWN;
    controls[HARD_DROP_BUTTON].button = SDL_SCANCODE_UP;
    controls[HOLD_BUTTON].button = SDL_SCANCODE_SPACE;
    controls[ROTATE_CCW_BUTTON].button = SDL_SCANCODE_Z;
    controls[ROTATE_CW_BUTTON].button = SDL_SCANCODE_X;
    controls[MIRROR_BUTTON].button = SDL_SCANCODE_C;
    controls[SELECT_BUTTON].button = SDL_SCANCODE_RETURN;
    controls[EXIT_BUTTON].button = SDL_SCANCODE_ESCAPE;
    controls[DOWN_BUTTON].button = SDL_SCANCODE_DOWN;
    controls[UP_BUTTON].button = SDL_SCANCODE_UP;

    return controls;

}

sound** initSounds()
{

    //Allocate memory for sounds
    sound** sounds = calloc(NUM_OF_SOUNDS, sizeof(**sounds));
    for (unsigned short i = 0; i < NUM_OF_SOUNDS; i++)
        sounds[i] = calloc(1, sizeof(*sounds));

    //Load all sounds
    sounds[COMPLETE_SOUND] = loadSound("AUDIO/complete.wav");
    sounds[HOLD_SOUND] = loadSound("AUDIO/hold.wav");
    sounds[LAND_SOUND] = loadSound("AUDIO/land.wav");
    sounds[MOVE_SOUND] = loadSound("AUDIO/move.wav");
    sounds[OVER_SOUND] = loadSound("AUDIO/over.wav");
    sounds[PAUSE_SOUND] = loadSound("AUDIO/pause.wav");
    sounds[ROTATE_SOUND] = loadSound("AUDIO/rotate.wav");

    return sounds;

}

//Function for scaling the renderer to the current window size
void scaleRenderer()
{

    //Get window dimensions
    int windowWidth;
    int windowHeight;
    SDL_GetWindowSize(globalInstance->window, &windowWidth, &windowHeight);

    //Get renderer dimensions
    int renderWidth;
    int renderHeight;
    SDL_RenderGetLogicalSize(globalInstance->renderer, &renderWidth, 
        &renderHeight);

    SDL_Rect rect = { .x = 0, .y = 0, .w = renderWidth, .h = renderHeight};
    float scale = 1;

    //Scale renderer appropriately
    scale = (float)windowWidth / (float)renderWidth;
    if ((int)((float)renderWidth * scale) > windowWidth || 
        (int)((float)renderHeight * scale) > windowHeight)
            scale = (float)windowHeight / (float)renderHeight;

    //Adjust viewport
    if (windowWidth == round((float)renderWidth * scale))
    {

        rect.y = (int)((windowHeight * ((float)renderWidth / windowWidth) - 
            renderHeight) / 2);
        rect.x = 0;

    }
    else
    {

        rect.x = (int)((windowWidth * ((float)renderHeight / windowHeight) - 
            renderWidth) / 2);
        rect.y = 0;

    }

    SDL_RenderSetScale(globalInstance->renderer, scale, scale);

    SDL_RenderSetViewport(globalInstance->renderer, &rect);

}

//Initialize game instance
void initInstance(gameInstance** instance)
{

    //Allocate memory for instance
    *instance = calloc(1, sizeof(**instance));

    (*instance)->running = true;

    //Get screen information
    SDL_GetCurrentDisplayMode(0, &(*instance)->DM);

    //Initialize window
        //Initialize in top left of screen
        //Window will be half the width and half the height of the screen
    (*instance)->window = SDL_CreateWindow("Multris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                            (*instance)->DM.w / 2, (*instance)->DM.h / 2, 
                                            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_FOCUS);

    //Save window size
    SDL_GetWindowSize((*instance)->window, &(*instance)->minimizedWindow_W, &(*instance)->minimizedWindow_H);

    //Initialize renderer
    (*instance)->renderer = SDL_CreateRenderer((*instance)->window, -1, SDL_RENDERER_ACCELERATED);
    //Set renderer size
    SDL_RenderSetLogicalSize(globalInstance->renderer, INITIAL_INTERNAL_WIDTH, INITIAL_INTERNAL_HEIGHT);

    //Initialize audioDevice and wavSpec
    (*instance)->audioDevice = prepareAudioDevice(&globalInstance->wavSpec);

    //Load all game sounds
    (*instance)->masterSounds = initSounds();
    (*instance)->sounds = initSounds();

    //Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {

        fprintf(stderr, "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        exit(-1);
            
    }
    else
        (*instance)->windowSurface = SDL_GetWindowSurface((*instance)->window);

    //Load string sprites
    (*instance)->stringSS = IMG_LoadTexture((*instance)->renderer, "SPRITES/string_ss.png");
    //Query string spritesheet for width and height
    SDL_QueryTexture((*instance)->stringSS, NULL, NULL, &(*instance)->stringSS_W, &(*instance)->stringSS_H);

    //Load game sprites
    (*instance)->gameSS = IMG_LoadTexture((*instance)->renderer, "SPRITES/game_ss.png");
    //Query game spritesheet for width and height
    SDL_QueryTexture((*instance)->gameSS, NULL, NULL, &(*instance)->gameSS_W, &(*instance)->gameSS_H);

    //Get keyboard state
	(*instance)->keys = (Uint8*)SDL_GetKeyboardState(NULL);

    //Initialize controls with the default controls
    (*instance)->controls = initControls();

    //Initialize FPS and frame_time to 0
    (*instance)->FPS = 0;
    (*instance)->frame_time = 0;

}

//Function for updating the volume of the game sounds
void updateVolume()
{

    for (unsigned short i = 0; i < NUM_OF_SOUNDS; i++)
    {

        //Buffer that stores audio after volume adjustment
        Uint8* dstStream = calloc(globalInstance->masterSounds[i]->length, 
                                    sizeof(*dstStream));

        //Mix audio with volume adjustment
        SDL_MixAudioFormat(dstStream, 
                            globalInstance->masterSounds[i]->buffer, 
                            globalInstance->wavSpec->format,
                            globalInstance->masterSounds[i]->length,
                            SDL_MIX_MAXVOLUME * (VOLUME / 100.0));

        //Copy volume adjusted buffer to sounds[i]->buffer
        for (int j = 0 ; j < globalInstance->masterSounds[i]->length; j++)
            globalInstance->sounds[i]->buffer[j] = dstStream[j];

        //Free dstStream memory
        free(dstStream);

    }

}

//Function for updating window mode between FULLSCREEN and not FULLSCREEN
void setWindowMode(unsigned short mode)
{

    if (mode == 0)
    {

        SDL_SetWindowFullscreen(globalInstance->window, 0);

        //Reset window size
        SDL_SetWindowSize(globalInstance->window, globalInstance->minimizedWindow_W,
                                                    globalInstance->minimizedWindow_H);
        //Reset window position
        SDL_SetWindowPosition(globalInstance->window, globalInstance->minimizedWindow_X,
                                                        globalInstance->minimizedWindow_Y);

	    //Reset scale on renderer
		SDL_RenderSetScale(globalInstance->renderer, 1, 1);
		SDL_RenderSetViewport(globalInstance->renderer, NULL);

        //Scale renderer accordingly
	    scaleRenderer();

    }
    else if (mode == 1)
    {

        //Save window size
        SDL_GetWindowSize(globalInstance->window, &globalInstance->minimizedWindow_W, 
                                                    &globalInstance->minimizedWindow_H);
        //Save window position
        SDL_GetWindowPosition(globalInstance->window, &globalInstance->minimizedWindow_X,
                                                        &globalInstance->minimizedWindow_Y);

        //Set window size to display size
        SDL_SetWindowSize(globalInstance->window, globalInstance->DM.w,
            globalInstance->DM.h);
        //Set window position to 0,0
        SDL_SetWindowPosition(globalInstance->window, 0, 0);

        //Scale renderer accordingly
	    scaleRenderer();

        SDL_SetWindowFullscreen(globalInstance->window, 
            SDL_WINDOW_FULLSCREEN);

    }

}