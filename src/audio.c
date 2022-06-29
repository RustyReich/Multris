#include "HEADERS/MGF.h"

//Play a sound
void _playSound(int id)
{

	sound* Sound = globalInstance->sounds[id];

	//If the sound being played is not the one already queued into the audio buffer
		//Technically we don't check if its the same one, we just check if its the same length
			//This still works since all audio files in the game are different lengths
	if (Sound->length != SDL_GetQueuedAudioSize(*globalInstance->audioDevice))
	{

		//Then we need to clear the audio buffer and queue the audio currently trying to be played
		SDL_ClearQueuedAudio(*globalInstance->audioDevice);
		SDL_QueueAudio(*globalInstance->audioDevice, Sound->buffer, 
			Sound->length);

	}

	//Play the audio currently queued
	SDL_PauseAudioDevice(*globalInstance->audioDevice, 0);

}

//Load a sound
sound* loadSound(char* file)
{

	//Allocate memory for the sound 
	sound* newSound;
	newSound = malloc(sizeof(*newSound));

	//Load the sound into the allocated memory
	if (newSound != NULL)
		SDL_LoadWAV(file, globalInstance->wavSpec, &newSound->buffer, 
			&newSound->length);

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

//Set volume of sound
void setVolume(sound** Sound, unsigned short volume)
{

	//Buffer that stores audio after volume adjustment
	Uint8* dstStream;
	dstStream = calloc((*Sound)->length, sizeof(*dstStream));

	//Mix audio with volume adjustment
	SDL_MixAudioFormat(dstStream, (*Sound)->buffer, 
		globalInstance->wavSpec->format, (*Sound)->length, 
		SDL_MIX_MAXVOLUME * (volume / 100.0));

	//Copy volume adjusted buffer back to Sound->buffer
	if (dstStream != NULL)
		for (int i = 0; i < (*Sound)->length; i++)
			(*Sound)->buffer[i] = dstStream[i];
	
	//Free dstStream
	free(dstStream);
	dstStream = NULL;
	
}
