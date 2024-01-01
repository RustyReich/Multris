#include "HEADERS/MGF.h"

//Play a sound
	//Only one sound can be played at a time.
	//Any sound currently being played when this function is called will be cutoff and the new sound will
	//start.
void _playSound(int id)
{

	sound* Sound = globalInstance->sounds[id];

	//Clear the audio buffer and queue the audio currently trying to be played
	SDL_ClearQueuedAudio(*globalInstance->audioDevice);
	SDL_QueueAudio(*globalInstance->audioDevice, Sound->buffer, Sound->length);

	//Play the audio currently queued
	SDL_PauseAudioDevice(*globalInstance->audioDevice, 0);

}

//Load a sound
sound* loadSound(char* file)
{

	//Allocate memory for the sound 
	sound* newSound;
	newSound = SDL_malloc(sizeof(*newSound));

	//Load the sound into the allocated memory
	if (newSound != NULL)
		SDL_LoadWAV(file, globalInstance->wavSpec, &newSound->buffer, &newSound->length);

	return newSound;

}

//Clear memory allocated by a sound
void delSound(sound** Sound)
{
	
	//Free memory taken by the sounds buffer
	SDL_FreeWAV((*Sound)->buffer);
	(*Sound)->buffer = NULL;
	
	//Now free memory taken by the actual sound struct itself
	SDL_free(*Sound);
	*Sound = NULL;

}

//Set volume of sound
void setVolume(sound** Sound, unsigned short volume)
{

	//Buffer that stores audio after volume adjustment
	Uint8* dstStream;
	dstStream = SDL_calloc((*Sound)->length, sizeof(*dstStream));

	//Mix audio with volume adjustment
	volume = SDL_MIX_MAXVOLUME * (volume / 100.0);
	SDL_MixAudioFormat(dstStream, (*Sound)->buffer, globalInstance->wavSpec->format, (*Sound)->length, volume);

	//Copy volume adjusted buffer back to Sound->buffer
	if (dstStream != NULL)
		for (unsigned int i = 0; i < (*Sound)->length; i++)
			(*Sound)->buffer[i] = dstStream[i];
	
	//Free dstStream
	SDL_free(dstStream);
	dstStream = NULL;
	
}

//Get the length of a sound struct in milliseconds
int getAudioLengthInMS(sound* Sound) {

	//Not really sure why we need to divide by 2 here
	double length_in_seconds = (double)(Sound->length) / (double)(globalInstance->wavSpec->freq) / (double)2;
	int length_in_ms = SDL_ceil(length_in_seconds * 1000);

	return length_in_ms;

}
