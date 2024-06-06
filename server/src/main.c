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

    // Start server provided in command line argument if present
    if (argc > 1)
    {

        if (SDLNet_ResolveHost(&ip, NULL, SDL_atoi(argv[1])) == -1)
        {

            printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
            exit(1);

        }

    }
    else
    {

        // Open server on random port. Try up to 10 random ports to find an open one
        while (SDLNet_ResolveHost(&ip, NULL, rand() % SDL_MAX_UINT16) == -1)
        {

            int maxPortTries = 10;
            int currPortTries = 0;

            currPortTries++;
            if (currPortTries < maxPortTries)
                continue;

            printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
            exit(1);

        }

    }

    // Open TCP socket
    server = SDLNet_TCP_Open(&ip);

    if (!server)
    {

        printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        exit(2);

    }
    else
        printf("Opened server on port %d.\n", SDL_SwapBE16(ip.port));

    bool running = true;

    int maxPlayers = 2;
    int numConnectedPlayers = 0;

    // Array of connected clients
    TCPsocket* clients = SDL_calloc(maxPlayers, sizeof(TCPsocket));

    // Array of socket sets. Each connected client has its own socket set.
    SDLNet_SocketSet* socketSets = SDL_calloc(maxPlayers, sizeof(SDLNet_SocketSet));
    
    printf("Waiting for player %d to join...\n", numConnectedPlayers + 1);

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

            // Add socket for current player to their own socket set
            socketSets[numConnectedPlayers] = SDLNet_AllocSocketSet(1);
            SDLNet_TCP_AddSocket(socketSets[numConnectedPlayers], clients[numConnectedPlayers]);

            // Keep track of number of connected players
            int id = numConnectedPlayers + 1;

            // Print their IP to the console
            Uint32 ipaddr = SDL_SwapBE32(remoteip->host);
            int ipParts[] = { ipaddr >> 24, (ipaddr >> 16) & 0xff, (ipaddr >> 8) & 0xff, ipaddr & 0xff };
            Uint16 port = SDL_SwapBE16(remoteip->port);
            printf("Player %d connected from %d.%d.%d.%d:%hu.\n", id, ipParts[0], ipParts[1], ipParts[2], ipParts[3], port);

            numConnectedPlayers++;

            if (numConnectedPlayers < maxPlayers)
            {

                // Let user know we are still waiting on additional players
                printf("Waiting for player %d to join...\n", numConnectedPlayers + 1);

                // Also let the connected player know that we are waiting for players to join
                char message[] = "Waiting for players to join...";
                SDLNet_TCP_Send(clients[numConnectedPlayers - 1], message, SDL_strlen(message));

            }

        }
        else
            SDL_Delay(100);

    }

    // Send message to all connected players once all players join
    for (int i = 0; i < maxPlayers; i++)
    {

        char message[] = "All players joined...";
        SDLNet_TCP_Send(clients[i], message, SDL_strlen(message));

    }

    while (running == true)
    {

        // Check all connected players
        for (int i = 0; i < maxPlayers; i++)
        {

            // Check to see if the player has sent any data
            if (SDLNet_CheckSockets(socketSets[i], 0))
            {

                printf("Receiving data from player %d...\n", i + 1);

                // Process the data if they sent any
                char data[1024];
                int len = SDLNet_TCP_Recv(clients[i], data, 1024);
                data[len - 1] = '\0';

                // If the data is MAP data
                if (SDL_strstr(data, "MAP") != NULL )
                {

                    printf("Received MAP from player %d.\n", i + 1);

                    // Send the map data to every other player
                    for (int j = 0; j < maxPlayers; j++)
                    {

                        if (j != i)
                        {

                            printf("Sending MAP from player %d to player %d...\n", i + 1, j + 1);
                            SDLNet_TCP_Send(clients[j], data, len);

                        }

                    }

                }

            }

        }

    }

    return 0;

}