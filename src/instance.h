#ifndef INSTANCE_H_
#define INSTANCE_H_

#include "MGF.h"

SDL_AudioDeviceID* prepareAudioDevice(SDL_AudioSpec** wavSpec);
control* initControls();
sound** initSounds();
void scaleRenderer();
void initInstance(gameInstance** instance);
void updateVolume();
void setWindowMode(unsigned short mode);
void findDisplayBounds(gameInstance* instance);
void MGF_srand(int seed);
int MGF_rand();

#endif