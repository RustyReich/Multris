#include "../../src/MGF.h"

int main (int argc, char* argv[])
{

    // Added just to get rid of unuused-parameter warning
    (void)argc;
    (void)argv;

    const int TICK_RATE = 128;
    const double TARGET_FRAME_TIME = (double)1 / (double)TICK_RATE;

    Uint32 ticksLastFrame = SDL_GetTicks(); 

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
            for (int currentPlayerIndex = 0; currentPlayerIndex < numConnectedPlayers; currentPlayerIndex++)
            {

                // Check to see if the player has sent any data
                if (SDLNet_CheckSockets(socketSets[currentPlayerIndex], 0))
                {

                    int playerID = currentPlayerIndex + 1;

                    // Keep track of the last time we have receieved data from that client
                    lastPulse[currentPlayerIndex] = SDL_GetTicks();

                    // Capture data from player in 1024-byte chunks and combine them together into data pointer
                    char* data = NULL;
                    int dataLen = 0;
                    while (SDLNet_CheckSockets(socketSets[currentPlayerIndex], 0))
                    {

                        char currentData[1024];
                        int currentLen = SDLNet_TCP_Recv(clients[currentPlayerIndex], currentData, 1024);

                        // If the data is of length 0, that means the client closed the connection
                        if (currentLen == 0)
                        {

                            // If a player disconnected, close the server.
                            printf("Player %d disconnected. Closing server.\n", playerID);
                            running = false;
                            goto Close;
                            
                        }

                        if (data == NULL)
                        {

                            data = SDL_calloc(currentLen, sizeof(char));
                            SDL_memcpy(data, currentData, currentLen);
                            dataLen = currentLen;

                        }
                        else
                        {

                            data = SDL_realloc(data, dataLen + currentLen);
                            SDL_memcpy(&data[dataLen], currentData, currentLen);
                            dataLen += currentLen;

                        }
                        
                    }

                    // Split data into "packets". A packet is basically a set of data for a specific variable.
                        // For example,"MAP=...NEXT=...SCORE=..." could all be received at the same time, and be stored
                        // in the data pointer at the same time, and here we would split it into three separate packets.
                    char** packets = NULL;
                    int numPackets = 0;
                    char* currentStringValue = NULL;
                    for (int dataIndex = 0; dataIndex < dataLen; dataIndex++)
                    {

                        if (data[dataIndex] != '\0')
                        {

                            if (currentStringValue == NULL)
                            {

                                currentStringValue = SDL_calloc(2, sizeof(char));
                                currentStringValue[0] = data[dataIndex];
                                currentStringValue[1] = '\0';

                            }
                            else
                            {

                                int newLen = SDL_strlen(currentStringValue) + 1 + 1;
                                currentStringValue = SDL_realloc(currentStringValue, newLen * sizeof(char));
                                SDL_strlcat(currentStringValue, &(data[dataIndex]), newLen);

                            }

                        }
                        else
                        {

                            if (numPackets == 0)
                            {

                                packets = SDL_calloc(1, sizeof(char*));
                                numPackets++;

                            }
                            else
                            {

                                packets = SDL_realloc(packets, (numPackets + 1) * sizeof(char*));
                                numPackets++;                                

                            }

                            packets[numPackets - 1] = SDL_calloc(SDL_strlen(currentStringValue) + 1, sizeof(char));
                            SDL_strlcpy(packets[numPackets - 1], currentStringValue, SDL_strlen(currentStringValue) + 1);

                            SDL_free(currentStringValue);
                            currentStringValue = NULL;

                        }

                    }

                    if (currentStringValue != NULL)
                    {

                        SDL_free(currentStringValue);
                        currentStringValue = NULL;

                    }

                    // Now go through each packet and process it
                    for (unsigned short packetIndex = 0; packetIndex < numPackets; packetIndex++)
                    {

                        // If the data is MAP data
                        if (SDL_strstr(packets[packetIndex], "MAP") != NULL)
                        {

                            printf("Received MAP from player %d.\n", playerID);

                            // Send the MAP data to every other player
                            for (int otherPlayerIndex = 0; otherPlayerIndex < maxPlayers; otherPlayerIndex++)
                            {

                                if (otherPlayerIndex != currentPlayerIndex)
                                {

                                    printf("Sending MAP from player %d to player %d...\n", playerID, otherPlayerIndex + 1);
                                    int len = SDL_strlen(packets[packetIndex]) + 1;
                                    SDLNet_TCP_Send(clients[otherPlayerIndex], packets[packetIndex], len);

                                }

                            }

                        }   // if the data is SCORE data   
                        else if (SDL_strstr(packets[packetIndex], "SCORE") != NULL)
                        {

                            printf("Received SCORE from player %d.\n", playerID);

                            // Send the SCORE data to every other player
                            for (int otherPlayerIndex = 0; otherPlayerIndex < maxPlayers; otherPlayerIndex++)
                            {

                                if (otherPlayerIndex != currentPlayerIndex)
                                {

                                    printf("Sending SCORE from player %d to player %d...\n", playerID, otherPlayerIndex + 1);
                                    int len = SDL_strlen(packets[packetIndex]) + 1;
                                    SDLNet_TCP_Send(clients[otherPlayerIndex], packets[packetIndex], len);

                                }

                            }

                        }
                        else if (SDL_strstr(packets[packetIndex], "NEXT") != NULL)
                        {

                            printf("Received NEXT from player %d.\n", playerID);

                            // Send the NEXT data to every other player
                            for (int otherPlayerIndex = 0; otherPlayerIndex < maxPlayers; otherPlayerIndex++)
                            {

                                if (otherPlayerIndex != currentPlayerIndex)
                                {

                                    printf("Sending NEXT from player %d to player %d...\n", playerID, otherPlayerIndex + 1);
                                    int len = SDL_strlen(packets[packetIndex]) + 1;
                                    SDLNet_TCP_Send(clients[otherPlayerIndex], packets[packetIndex], len);

                                }

                            }

                        }
                        else if (SDL_strstr(packets[packetIndex], "LEVEL") != NULL)
                        {

                            printf("Received LEVEL from player %d.\n", playerID);

                            // Send the LEVEL data to every other player
                            for (int otherPlayerIndex = 0; otherPlayerIndex < maxPlayers; otherPlayerIndex++)
                            {

                                if (otherPlayerIndex != currentPlayerIndex)
                                {
            
                                    printf("Sending LEVEL from player %d to player %d...\n", playerID, otherPlayerIndex + 1);
                                    int len = SDL_strlen(packets[packetIndex]) + 1;
                                    SDLNet_TCP_Send(clients[otherPlayerIndex], packets[packetIndex], len);

                                }

                            }

                        }
                        else if (SDL_strstr(packets[packetIndex], "LINES") != NULL)
                        {

                            printf("Received LINES from player %d.\n", playerID);

                            // Send the LINES data to every other player
                            for (int otherPlayerIndex = 0; otherPlayerIndex < maxPlayers; otherPlayerIndex++)
                            {

                                if (otherPlayerIndex != currentPlayerIndex)
                                {
            
                                    printf("Sending LINES from player %d to player %d...\n", playerID, otherPlayerIndex + 1);
                                    int len = SDL_strlen(packets[packetIndex]) + 1;
                                    SDLNet_TCP_Send(clients[otherPlayerIndex], packets[packetIndex], len);

                                }

                            }

                        }
                        else if (SDL_strstr(packets[packetIndex], "CURRENT") != NULL)
                        {

                            printf("Received CURRENT from player %d.\n", playerID);

                            for (int otherPlayerIndex = 0; otherPlayerIndex < maxPlayers; otherPlayerIndex++)
                            {

                                if (otherPlayerIndex != currentPlayerIndex)
                                {

                                    // Send CURRENT piece data to all other players
                                    printf("Sending CURRENT from player %d to player %d...\n", playerID, otherPlayerIndex + 1);
                                    int len = SDL_strlen(packets[packetIndex]) + 1;
                                    SDLNet_TCP_Send(clients[otherPlayerIndex], packets[packetIndex], len);

                                }

                            }

                        }
                        else if (SDL_strstr(packets[packetIndex], "HOLD") != NULL)
                        {

                            printf("Received HOLD from player %d.\n", playerID);

                            for (int otherPlayerIndex = 0; otherPlayerIndex < maxPlayers; otherPlayerIndex++)
                            {

                                if (otherPlayerIndex != currentPlayerIndex)
                                {

                                    // Send HOLD piece data to all other players
                                    printf("Sending HOLD from player %d to player %d...\n", playerID, otherPlayerIndex + 1);
                                    int len = SDL_strlen(packets[packetIndex]) + 1;
                                    SDLNet_TCP_Send(clients[otherPlayerIndex], packets[packetIndex], len);

                                }

                            }

                        }
                        else if (SDL_strstr(packets[packetIndex], "POSITION") != NULL)
                        {

                            printf("Received POSITION from player %d.\n", playerID);

                            for (int otherPlayerIndex = 0; otherPlayerIndex < maxPlayers; otherPlayerIndex++)
                            {

                                if (otherPlayerIndex != currentPlayerIndex)
                                {

                                    // Send POSITION data to all other players
                                    printf("Sending POSITION from player %d to player %d...\n", playerID, otherPlayerIndex + 1);
                                    int len = SDL_strlen(packets[packetIndex]) + 1;
                                    SDLNet_TCP_Send(clients[otherPlayerIndex], packets[packetIndex], len);

                                }

                            }

                        }

                    }

                    // Free our data and packets to avoid memory leaks
                    for (int i = 0; i < numPackets; i++)
                        SDL_free(packets[i]);
                    SDL_free(packets);
                    SDL_free(data);

                }
                else
                {

                    // If we haven't received any data in TIMEOUT_SECONDS seconds, assume a timeout
                    if ((SDL_GetTicks() - lastPulse[currentPlayerIndex]) / 1000 > TIMEOUT_SECONDS)
                    {

                        // Since the client disconnected, close the server.
                        printf("Received no packets from player %d in %d seconds. Closing server.\n", currentPlayerIndex + 1, TIMEOUT_SECONDS);
                        running = false;
                        continue;                        

                    }

                }

            }

        }

        //Logic for limitting the FPS
            //Basically just sleeps every frame if not enough time passed between frames
        int deltaMS = SDL_GetTicks() - ticksLastFrame;
        if (deltaMS < TARGET_FRAME_TIME * 1000)
            SDL_Delay(TARGET_FRAME_TIME * 1000 - deltaMS);
        ticksLastFrame = SDL_GetTicks();

    }

    Close:;
    
    //Close SDL Stuff
    SDLNet_Quit();
    SDL_Quit();

    return 0;

}