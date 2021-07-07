#include <SDL.h>

#include "Structures.h"

SDL_AudioDeviceID* prepareAudioDevice(SDL_AudioSpec** wavSpec)
{

	//Populate wavSpec structure
	*wavSpec = malloc(sizeof(**wavSpec));
	Uint32 wavLength;
	Uint8* wavBuffer;
	//All sound files have the same specs as move.wav, so we can use it to load the audio specs
	SDL_LoadWAV("move.wav", (*wavSpec), &wavBuffer, &wavLength);

	//Open the Audio device
	SDL_AudioDeviceID* device;
	device = malloc(sizeof(*device));
	if (device != NULL)
		*device = SDL_OpenAudioDevice(NULL, 0, *wavSpec, NULL, 0);

	//Free the memory taken by loading "move.wav"
	SDL_FreeWAV(wavBuffer);

	return device;

}

void playSound(sound* Sound, SDL_AudioDeviceID* audioDevice, SDL_AudioSpec* wavSpec)
{

	//If the sound being played is not the one already queued into the audio buffer
		//Technically we don't check if its the same one, we just check if its the same length
			//This still works since all audio files in the game are different lengths
	if (Sound->length != SDL_GetQueuedAudioSize(*audioDevice))
	{

		//Then we need to clear the audio buffer and queue the audio currently trying to be played
		SDL_ClearQueuedAudio(*audioDevice);
		SDL_QueueAudio(*audioDevice, Sound->buffer, Sound->length);

	}

	//Play the audio currently queued
	SDL_PauseAudioDevice(*audioDevice, 0);

}

sound* loadSound(char* file, SDL_AudioSpec* wavSpec)
{

	//Allocate memory for the sound 
	sound* newSound;
	newSound = malloc(sizeof(*newSound));

	//Load the sound into the allocated memory
	if (newSound != NULL)
		SDL_LoadWAV(file, wavSpec, &newSound->buffer, &newSound->length);

	return newSound;

}

//Clear memory allocated by a sound
void delSound(sound** Sound)
{
	
	//Free memory taken by the sounds buffer
	SDL_FreeWAV((*Sound)->buffer);
	(*Sound)->buffer = NULL;
	
	//Now free memory taken by the actual sound struct itself
	free(*Sound);
	*Sound = NULL;

}
