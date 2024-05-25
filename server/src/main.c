#include "../../src/MGF.h"

int main (int argc, char* argv[])
{

    // Added just to get rid of unuused-parameter warning
    (void)argc;
    (void)argv;

    printf("Starting server...\n");

    // Initialize SDL Stuff
    if (SDL_Init(0) == -1)
    {

        printf("SDL_Init: %s\n", SDL_GetError());
        exit(1);

    }
    if (SDLNet_Init() == -1)
    {

        printf("SDLNet_Init: %s\n", SDLNet_GetError());
        exit(2);

    }

    // Set seed for random number generator
    srand((int)time(NULL));

    TCPsocket server;
    IPaddress ip;
    int maxPortTries = 10;
    int currPortTries = 0;

    // Open server on random port. Try up to 10 random ports to find an open one
    while (SDLNet_ResolveHost(&ip, NULL, rand() % SDL_MAX_UINT16) == -1)
    {

        currPortTries++;
        if (currPortTries < maxPortTries)
            continue;

        printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        exit(1);

    }

    // Open TCP socket
    server = SDLNet_TCP_Open(&ip);

    if (!server)
    {

        printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        exit(2);

    }
    else
        printf("Opened server on port %d\n", SDL_SwapBE16(ip.port));

    bool running = true;

    int maxPlayers = 2;
    int numConnectedPlayers = 0;

    // Array of connected clients
    TCPsocket* clients = SDL_calloc(maxPlayers, sizeof(TCPsocket));

    printf("Waiting for player %d to join\n", numConnectedPlayers + 1);

    // Wait until maxPLayers number of players connects to the server
    while (numConnectedPlayers < maxPlayers)
    {

        // Accept connection from a client
        clients[numConnectedPlayers] = SDLNet_TCP_Accept(server);

        // If a client connects
        if (clients[numConnectedPlayers])
        {

            // Get their IP address
            IPaddress *remoteip;
            remoteip = SDLNet_TCP_GetPeerAddress(clients[numConnectedPlayers]);

            if (!remoteip)
            {

                printf("SDLNet_TCP_GetPeerAddress: %s\n", SDLNet_GetError());
                continue;

            }

            // Keep track of number of connected players
            int id = numConnectedPlayers + 1;

            // Print their IP to the console
            Uint32 ipaddr = SDL_SwapBE32(remoteip->host);
            int ipParts[] = { ipaddr >> 24, (ipaddr >> 16) & 0xff, (ipaddr >> 8) & 0xff, ipaddr & 0xff };
            Uint16 port = SDL_SwapBE16(remoteip->port);
            printf("Player %d connected from %d.%d.%d.%d:%hu.\n", id, ipParts[0], ipParts[1], ipParts[2], ipParts[3], port);

            numConnectedPlayers++;

            // Let user know we are still waiting on additional players
            if (numConnectedPlayers < maxPlayers)
                printf("Waiting for player %d to join\n", numConnectedPlayers + 1);

        }
        else
            SDL_Delay(100);

    }

    while (running == true)
    {



    }

    return 0;

}