#ifndef SERVER_H_
#define SERVER_H_

#include "MGF.h"

int startDedicatedServer(int argc, char* argv[]);
int startServer(IPaddress address, int tickRate);

#endif