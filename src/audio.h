#ifndef AUDIO_H_
#define AUDIO_H_

#include "MGF.h"

void _playSound(int id);
sound* loadSound(char* file);
void delSound(sound** Sound);
void setVolume(sound** Sound, unsigned short volume);
int getAudioLengthInMS(sound* Sound);

#endif