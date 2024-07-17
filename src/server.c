#include "MGF.h"

// Function for starting a dedicated server
int startDedicatedServer(int argc, char* argv[])
{

    // Added just to get rid of unuused-parameter warning
    (void)argc;
    (void)argv;

    const int TICK_RATE = 128;

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

    IPaddress ip;
    bool portSpecified = false;

    bool restartAfterClose = false;

    // Check for launch arguments
	for (unsigned short i = 1; i < argc; i++)
    {

        // -p launch argument takes the next argument as the port
        if (SDL_strcmp(argv[i], "-p") == 0)
        {

            if (i < argc - 1)
            {

                portSpecified = true;

                if (SDLNet_ResolveHost(&ip, NULL, SDL_atoi(argv[i + 1])) == -1)
                {

                    printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
                    exit(1);

                }

            }

        }   // -r launch argument sets the server to restart after the game ends
        else if (SDL_strcmp(argv[i], "-r") == 0)
            restartAfterClose = true;

    }

    // Close server if no port was specified
    if (portSpecified == false)
    {

        printf("No port specified.\n");
        exit(1);

    }

    do
    {

        startServer(ip, TICK_RATE);

        if (restartAfterClose)
            printf("Restarting server...\n");

    }
    while (restartAfterClose);
    
    //Close SDL Stuff
    SDLNet_Quit();
    SDL_Quit();

    return 0;

}

// Function for starting a server (dedicated or from in game)
void startServer(IPaddress address, int tickRate)
{

    const double TARGET_FRAME_TIME = (double)1 / (double)tickRate;
    Uint32 ticksLastFrame = SDL_GetTicks();

    // Open TCP socket
    TCPsocket socket;
    socket = SDLNet_TCP_Open(&address);

    if (!socket)
    {

        printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        exit(2);

    }
    else
        printf("Opened server on port %d.\n", SDL_SwapBE16(address.port));

    bool running = true;

    int maxPlayers = 2;
    int numConnectedPlayers = 0;

    // Array of connected clients
    TCPsocket* clients = SDL_calloc(maxPlayers, sizeof(TCPsocket));

    // Array of socket sets. Each connected client has its own socket set.
    SDLNet_SocketSet* socketSets = SDL_calloc(maxPlayers, sizeof(SDLNet_SocketSet));
    
    printf("Waiting for player %d to join...\n", numConnectedPlayers + 1);

    // Variables used for checking for timedout players
    int *lastPulse = NULL;
    Uint32 TIMEOUT_SECONDS = 10;

    // Store the names of the players
    char** names = NULL;
    int numNames = 0;
    bool sentNames = false;

    // Variables for the countdown
    int playersReady = 0;
    bool countdownGoing = false;
    int currentCountdownValue = 4;
    Uint32 timeAllPlayersConnected = 0;
    bool gameStarted = false;

    LoopStart:;
    while (running == true)
    {

        // Accept a connection
        TCPsocket newSocket = SDLNet_TCP_Accept(socket);
        
        // If a client connected
        if (newSocket != NULL)
        {

            // Allow the connection as long as the server is not full
            if (numConnectedPlayers < maxPlayers)
            {

                clients[numConnectedPlayers] = newSocket;
                
                // Get their IP address
                IPaddress* remoteip;
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
                    char message[] = "Waiting for players to join";
                    SDLNet_TCP_Send(clients[numConnectedPlayers - 1], message, SDL_strlen(message) + 1);

                }
                else
                {

                    // Once all players connect, let all players know to press SELECT when ready
                    for (int i = 0; i < maxPlayers; i++)
                    {

                        char message[] = "Press SELECT when ready";
                        SDLNet_TCP_Send(clients[i], message, SDL_strlen(message) + 1);

                    }

                }      

            }
            else    // If the server is full, disconnect the client and let them know the server is full
            {

                char message[] = "Server is full";
                SDLNet_TCP_Send(newSocket, message, SDL_strlen(message) + 1);
                
                SDLNet_TCP_Close(newSocket);

            }

        }

        // If at least one player has connected
        if (numConnectedPlayers > 0)
        {

            // If all players are ready and we have synced all the names and we haven't started the countdown yet
            if (playersReady == maxPlayers && sentNames == true && countdownGoing == false)
            {

                // Send the RNG seed to all players
                    // Seed is just the current time
                int startSeed = (int)time(NULL);
                for (int playerIndex = 0; playerIndex < maxPlayers; playerIndex++)
                {

                    char* seedAsString = SDL_calloc(getIntLength(startSeed), sizeof(char));
                    SDL_itoa(startSeed, seedAsString, 10);

                    int len = SDL_strlen("SEED=") + getIntLength(startSeed) + 1;
                    char* seedPacket = SDL_calloc(len, sizeof(char));
                    SDL_strlcpy(seedPacket, "SEED=", len);
                    SDL_strlcat(seedPacket, seedAsString, len);

                    SDLNet_TCP_Send(clients[playerIndex], seedPacket, len);

                    SDL_free(seedAsString);
                    SDL_free(seedPacket);  

                }

                // Start the countdown at 3 seconds
                countdownGoing = true;

                timeAllPlayersConnected = SDL_GetTicks();
                printf("All players ready.\n");
                printf("Starting game in 3...\n");

                for (int playerIndex = 0; playerIndex < maxPlayers; playerIndex++)
                {

                    char message[] = "Game starting in 3";
                    SDLNet_TCP_Send(clients[playerIndex], message, SDL_strlen(message) + 1);  

                }

            }   // Countdown until the game has started
            else if (countdownGoing == true && gameStarted == false)
            {

                if ((SDL_GetTicks() - timeAllPlayersConnected) / 1000 >= 3)
                {

                    // Send "START" message to all players once countdown reaches zero
                    printf("Starting game!\n");

                    for (int playerIndex = 0; playerIndex < maxPlayers; playerIndex++)
                    {

                        char message[] = "START";
                        SDLNet_TCP_Send(clients[playerIndex], message, SDL_strlen(message) + 1);  

                    }

                    gameStarted = true;

                }
                else if ((SDL_GetTicks() - timeAllPlayersConnected) / 1000 >= 2 && currentCountdownValue > 1)
                {

                    printf("Starting game in 1...\n");

                    for (int playerIndex = 0; playerIndex < maxPlayers; playerIndex++)
                    {

                        char message[] = "Game starting in 1";
                        SDLNet_TCP_Send(clients[playerIndex], message, SDL_strlen(message) + 1);  

                    }

                    currentCountdownValue = 1;

                }
                else if ((SDL_GetTicks() - timeAllPlayersConnected) / 1000 >= 1 && currentCountdownValue > 2)
                {

                    printf("Starting game in 2...\n");

                    for (int playerIndex = 0; playerIndex < maxPlayers; playerIndex++)
                    {

                        char message[] = "Game starting in 2";
                        SDLNet_TCP_Send(clients[playerIndex], message, SDL_strlen(message) + 1);  

                    }

                    currentCountdownValue = 2;

                }

            }

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

                            // Tell all other clients a player disconnected
                            char message[] = "Player disconnected.";
                            for (int otherPlayerIndex = 0; otherPlayerIndex < numConnectedPlayers; otherPlayerIndex++)
                                if (otherPlayerIndex != currentPlayerIndex)
                                    SDLNet_TCP_Send(clients[otherPlayerIndex], message, SDL_strlen(message) + 1);
                            
                            running = false;
                            goto LoopStart;
                            
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

                        // Get the packet header (everything before the '=')
                        int endHeaderIndex = 0;
                        for (unsigned short i = 0; i < SDL_strlen(packets[packetIndex]); i++)
                            if (packets[packetIndex][endHeaderIndex] != '=')
                                endHeaderIndex++;
                            else
                                break;
                        char* packetHeader = SDL_calloc(endHeaderIndex + 1, sizeof(char));
                        SDL_strlcpy(packetHeader, packets[packetIndex], endHeaderIndex + 1);

                        // If the received packet is a NAME
                        if (SDL_strstr(packets[packetIndex], "NAME") != NULL)
                        {

                            // Find the start of the name value
                            int nameStartIndex = SDL_strlen("NAME=");
                            int nameLength = SDL_strlen(&(packets[packetIndex][nameStartIndex])) + 1;

                            // Allocate space for the name
                            if (numNames == 0)
                                names = SDL_calloc(1, sizeof(char*));
                            else
                                names = SDL_realloc(names, sizeof(char*) * (numNames + 1));
                            names[currentPlayerIndex] = SDL_calloc(nameLength, sizeof(char));
                            numNames++;

                            // And store the name
                            SDL_strlcpy(names[currentPlayerIndex], &(packets[packetIndex][nameStartIndex]), nameLength);

                            printf("Received name %s from player %d.\n", names[currentPlayerIndex], playerID);

                            // If the NAME we just recieved is the last name, send names to all
                            // players
                            if (numNames == maxPlayers)
                            {

                                for (unsigned short i = 0; i < numConnectedPlayers; i++)
                                {

                                    char* currentName = names[i];

                                    for (unsigned short j = 0; j < numConnectedPlayers; j++)
                                    {

                                        if (j != i)
                                        {

                                            // Send the players names to each other
                                            printf("Sending name %s from player %d to player %d.\n", currentName, i + 1, j + 1);
                                            int len = SDL_strlen("NAME=") + SDL_strlen(currentName) + 1;
                                            char* namePacket = SDL_calloc(len, sizeof(char));
                                            SDL_strlcpy(namePacket, "NAME=", len);
                                            SDL_strlcat(namePacket, currentName, len);
                                            SDLNet_TCP_Send(clients[j], namePacket, len);
                                            SDL_free(namePacket);

                                        }

                                    }

                                }

                                sentNames = true;

                            }

                        }   // Keep track of when each player is ready
                        else if (SDL_strstr(packets[packetIndex], "READY") != NULL)
                        {

                            printf("Player %d is ready.\n", playerID);

                            playersReady++;

                        }
                           // If the packet is not a NAME or PULSE
                        else if (SDL_strstr(packets[packetIndex], "PULSE") == NULL)
                        {

                            printf("Received %s from player %d.\n", packetHeader, playerID);

                            // Forward the packet to all other connected players
                            for (int otherPlayerIndex = 0; otherPlayerIndex < maxPlayers; otherPlayerIndex++)
                            {

                                if (otherPlayerIndex != currentPlayerIndex)
                                {

                                    printf("Sending %s from player %d to player %d.\n", packetHeader, playerID, otherPlayerIndex + 1);
                                    int len = SDL_strlen(packets[packetIndex]) + 1;
                                    SDLNet_TCP_Send(clients[otherPlayerIndex], packets[packetIndex], len);

                                }

                            }


                        }
                        else
                            printf("Received PULSE from player %d.\n", playerID);

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
                        
                        // Tell all other clients a player disconnected
                        char message[] = "Player disconnected.";
                        for (int otherPlayerIndex = 0; otherPlayerIndex < numConnectedPlayers; otherPlayerIndex++)
                            if (otherPlayerIndex != currentPlayerIndex)
                                SDLNet_TCP_Send(clients[otherPlayerIndex], message, SDL_strlen(message) + 1);
                        
                        running = false;
                        goto LoopStart;                        

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

    // Free memory and close connection to each client
    for (unsigned short i = 0; i < numConnectedPlayers; i++)
    {

        if (names != NULL)
            SDL_free(names[i]);

        SDLNet_TCP_Close(clients[i]);
        SDLNet_FreeSocketSet(socketSets[i]);

    }
    if (names != NULL)
        SDL_free(names);
    SDL_free(socketSets);
    SDLNet_TCP_Close(socket);
    SDL_free(clients);

    printf("Server closed.\n");

}