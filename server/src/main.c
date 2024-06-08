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

    // Game is started once all players connect
    bool gameStarted = false;

    // Variables used for checking for timedout players
    int *lastPulse = NULL;
    Uint32 TIMEOUT_SECONDS = 10;

    while (running == true)
    {

        // If game hasn't started yet
        if (gameStarted == false)
        {

            // Check for a connection from a client and accept it
            clients[numConnectedPlayers] = SDLNet_TCP_Accept(server);

            // If a client connects
            if (clients[numConnectedPlayers])
            {

                // Get their IP address
                IPaddress *remoteip;
                remoteip = SDLNet_TCP_GetPeerAddress(clients[numConnectedPlayers]);

                // Print error if error getting the IP address of the connected client
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

                // Keep track of the last time we received data from the client
                if (numConnectedPlayers == 1)
                {

                    lastPulse = SDL_calloc(1, sizeof(int));
                    lastPulse[0] = SDL_GetTicks();

                }
                else if (lastPulse != NULL && numConnectedPlayers > 1)
                {

                    lastPulse = SDL_realloc(lastPulse, numConnectedPlayers * sizeof(int));
                    lastPulse[numConnectedPlayers - 1] = SDL_GetTicks();

                }

                // If still waiting for more players to join
                if (numConnectedPlayers < maxPlayers)
                {

                    // Let user know we are still waiting on additional players
                    printf("Waiting for player %d to join...\n", numConnectedPlayers + 1);

                    // Also let the connected player know that we are waiting for players to join
                    char message[] = "Waiting for players to join...";
                    SDLNet_TCP_Send(clients[numConnectedPlayers - 1], message, SDL_strlen(message));

                }
                else
                {

                    // Send message to all connected players once all players join
                    for (int i = 0; i < maxPlayers; i++)
                    {

                        char message[] = "All players joined...";
                        SDLNet_TCP_Send(clients[i], message, SDL_strlen(message));

                    }

                    // Once all players have joined, the game starts
                    gameStarted = true;

                }

            }

        }

        // If at least one player has connected
        if (numConnectedPlayers > 0)
        {

            // Check all connected players
            for (int i = 0; i < numConnectedPlayers; i++)
            {

                // Check to see if the player has sent any data
                if (SDLNet_CheckSockets(socketSets[i], 0))
                {

                    int playerID = i + 1;

                    // Keep track of the last time we have receieved data from that client
                    lastPulse[i] = SDL_GetTicks();

                    // Process the data if they sent any
                    char data[1024];
                    int len = SDLNet_TCP_Recv(clients[i], data, 1024);
                    data[len] = '\0';

                    // If the data is of length 0, that means the client closed the connection
                    if (len == 0)
                    {

                        // If a player disconnected, close the server.
                        printf("Player %d disconnected. Closing server.\n", playerID);
                        running = false;
                        continue;
                        
                    }

                    // If the data is MAP data
                    if (SDL_strstr(data, "MAP") != NULL)
                    {

                        printf("Received MAP from player %d.\n", playerID);

                        // Send the map data to every other player
                        for (int j = 0; j < maxPlayers; j++)
                        {

                            if (j != i)
                            {

                                printf("Sending MAP from player %d to player %d...\n", playerID, j + 1);
                                SDLNet_TCP_Send(clients[j], data, len);

                            }

                        }

                    }   // if the data is SCORE data   
                    else if (SDL_strstr(data, "SCORE") != NULL)
                    {

                        printf("Received SCORE from player %d.\n", playerID);

                        // Send the score data to every other player
                        for (int j = 0; j < maxPlayers; j++)
                        {

                            if (j != i)
                            {

                                printf("Sending SCORE from player %d to player %d...\n", playerID, j + 1);
                                SDLNet_TCP_Send(clients[j], data, len);

                            }

                        }

                    }
                    else if (SDL_strstr(data, "NEXT") != NULL)
                    {

                        printf("Received NEXT from player %d.\n", playerID);

                        // Send the NEXT data to every other player
                        for (int j = 0; j < maxPlayers; j++)
                        {

                            if (j != i)
                            {

                                printf("Sending NEXT from player %d to player %d...\n", playerID, j + 1);
                                SDLNet_TCP_Send(clients[j], data, len);

                            }

                        }

                    }

                }
                else
                {

                    // If we haven't received any data in TIMEOUT_SECONDS seconds, assume a timeout
                    if ((SDL_GetTicks() - lastPulse[i]) / 1000 > TIMEOUT_SECONDS)
                    {

                        // Since the client disconnected, close the server.
                        printf("Received no packets from player %d in %d seconds. Closing server.\n", i + 1, TIMEOUT_SECONDS);
                        running = false;
                        continue;                        

                    }

                }

            }

        }

    }

    //Close SDL Stuff
    SDLNet_Quit();
    SDL_Quit();

    return 0;

}