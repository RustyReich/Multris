#ifndef SERVER_H_
#define SERVER_H_

#include "MGF.h"

int startDedicatedServer(int argc, char* argv[]);
void startServer(IPaddress address, int tickRate);

#endif