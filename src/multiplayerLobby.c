#include "MGF.h"

unsigned short multiplayerLobby(piece** Piece, char* serverMessage)
{

    //Variables
    DECLARE_VARIABLE(int, nextText_Width, 0);
    DECLARE_VARIABLE(int, nextText_Height, 0);
    DECLARE_VARIABLE(bool, firstLoop, true);
    DECLARE_VARIABLE(double, textMulti, 0.80);
    DECLARE_VARIABLE(double, messageMulti, 1.0);
    DECLARE_VARIABLE(bool, error, false);
    DECLARE_VARIABLE(int, lastPulseTime, 0);
    DECLARE_VARIABLE(bool, justPasted, false);
    DECLARE_VARIABLE(bool, waitingForReady, false);
    DECLARE_VARIABLE(bool, tryingConnection, false);
    DECLARE_VARIABLE(bool, connectFunctionReturned, false);
    DECLARE_VARIABLE(int, timeConnectingStarted, 0);
    DECLARE_VARIABLE(bool, hostingGame, false);

    //Textures
    static SDL_Texture* Texture_Score; declare_HUD_Text(&Texture_Score, SCORE_TEXT);
    static SDL_Texture* Texture_Level; declare_HUD_Text(&Texture_Level, LEVEL_TEXT);
    static SDL_Texture* Texture_Lines; declare_HUD_Text(&Texture_Lines, LINES_TEXT);
    static SDL_Texture* Texture_Next; declare_Piece_Text(&Texture_Next, *Piece, false);
    static SDL_Texture* Texture_Cursor; declare_HUD_Text(&Texture_Cursor, CURSOR_TEXT);
    static SDL_Texture* Texture_ConnectionValues; declare_HUD_Text(&Texture_ConnectionValues, CONNECTIONVALUES_TEXT);
    static SDL_Texture* Texture_ConnectionMessage; declare_HUD_Text(&Texture_ConnectionMessage, CONNECTIONMESSAGE_TEXT);
    static SDL_Texture* Texture_HostingValues; declare_HUD_Text(&Texture_HostingValues, HOSTINGVALUES_TEXT);

    //UI elements
    static UI_list* connection; declare_UI_list(&connection, CONNECTION_LIST);
    static UI_list* hosting; declare_UI_list(&hosting, HOSTING_LIST);
    static UI_list* multiplayer; declare_UI_list(&multiplayer, MULTIPLAYER_LIST);

    //Arrays
    static char* ipString; declareStart(ipString, '\0');
    static char* portString; declareStart(portString, '\0');
    static char* nameString; declareStart(nameString, '\0');
    static char* currMessage; declareStart(currMessage, '\0');
    static char* hostGameData; declareStart(hostGameData, '\0');

    // Keep track of the currently active UI_list
    UI_list* active_list = NULL;

    if (*firstLoop == true)
    {

        SDL_QueryTexture(Texture_Next, NULL, NULL, nextText_Width, nextText_Height);
        *firstLoop = false;

        // If the player is being returned back to the lobby after being disconnected from a multiplayer game
        if (SDL_strlen(serverMessage) > 0)
        {

            multiplayer->ui->currentlyInteracting = false;
            
            // If we just got out of a game that we were hosting, we should be exited into the HOST menu. Otherwise, the CONNECT menu.
            if (globalInstance->hosting == true)
                hosting->ui->currentlyInteracting = true;
            else
                connection->ui->currentlyInteracting = true;

            // Display serverMessage on the screen
            currMessage = SDL_realloc(currMessage, sizeof(char) * (SDL_strlen(serverMessage) + 1));
            SDL_strlcpy(currMessage, serverMessage, (SDL_strlen(serverMessage) + 1));

            updateConnectionMessageText(&Texture_ConnectionMessage, currMessage);
            *error = true;

            // Erase serverMessage since now we are already displaying it on the screen
            SDL_strlcpy(serverMessage, "\0", 1);

            // Load values needed depending on the screen we are in
            if (connection->ui->currentlyInteracting == true)
                loadIPandPortForConnectScreen(ipString, portString);
            else if (hosting->ui->currentlyInteracting == true)
                loadPortForHostScreen(portString);

        }

        // Load the players name from the name file if it is there
        char* loadedName = loadName();
        if (loadedName != NULL)
        {

            if (SDL_strlen(loadedName) > 0)
            {

                nameString = SDL_realloc(nameString, (SDL_strlen(loadedName) + 1) * sizeof(char));
                SDL_strlcpy(nameString, loadedName, SDL_strlen(loadedName) + 1);

                // Update the ConnectionValues and HostingValues textures with the name that was loaded
                updateConnectionValuesText(Texture_ConnectionValues, ipString, portString, nameString);
                updateHostingValuesText(Texture_HostingValues, portString, nameString);

            }

            SDL_free(loadedName);

        }

        // Update the ConnectionValues and HostingValues textures now that some data may have been loaded from disk
        updateConnectionValuesText(Texture_ConnectionValues, ipString, portString, nameString);
        updateHostingValuesText(Texture_HostingValues, portString, nameString);

        // We are no longer hosting a game since we're in the multiplayer lobby
        globalInstance->hosting = false;

    }

    // Keep track of the currently active list and the currently selected option in that list
    const char* selected_option = NULL;
    if (connection->ui->currentlyInteracting)
    {

        selected_option = getListSelectedString(connection);
        active_list = connection;

    }
    else if (multiplayer->ui->currentlyInteracting)
    {

        selected_option = getListSelectedString(multiplayer);
        active_list = multiplayer;

    }
    else if (hosting->ui->currentlyInteracting)
    {

        selected_option = getListSelectedString(hosting);
        active_list = hosting;

    }

    // If connected to a server
    if (MULTIPLAYER)
    {

        // Check if data is ready to be received from the server
        if (SDLNet_CheckSockets(globalInstance->serverSocketSet, 0))
        {

			bool disconnect = false;
            bool startGame = false;

			// Capture data from server in 1024-byte chunks and combine them together into data pointer
			char* data = NULL;
			int dataLen = 0;
			while(SDLNet_CheckSockets(globalInstance->serverSocketSet, 0))
			{

				char currentData[1024];
				int currentLen = SDLNet_TCP_Recv(globalInstance->serverSocket, currentData, 1024);

				// If data is length 0, the server closed. So disconnect from the server.
				if (currentLen == 0)
				{

					disconnect = true;
					break;

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

			// Split data into "packets" separated by "\0"
			int numPackets = 0;
			char** packets = extractStringsFromDelimitedBytes(data, dataLen, &numPackets, '\0');

			// Now go through each packet and process it
			for (unsigned short packetIndex = 0; packetIndex < numPackets; packetIndex++)
			{

                // If the packet does not have a "=" then it is a server message
                if (SDL_strstr(packets[packetIndex], "=") == NULL)
                {

                    // If received a message containing "START", exit multiplayer lobby and move into PLAYMODE.
                    if (SDL_strstr(packets[packetIndex], "START") != NULL)
                    {

                        startGame = true;
                        break;

                    }
                    else
                    {

                        // Print the messsage received from the server
                        currMessage = SDL_realloc(currMessage, sizeof(char) * SDL_strlen(packets[packetIndex]) + 1);
                        SDL_strlcpy(currMessage, packets[packetIndex], SDL_strlen(packets[packetIndex]) + 1);
                        updateConnectionMessageText(&Texture_ConnectionMessage, currMessage);

                        if (SDL_strstr(currMessage, "Press SELECT when ready") != NULL)
                            *waitingForReady = true;

                    }

                }
				else
                {

                    // If the packet received has an "=", then it is an important game value that needs to be transferred into playMode
                        // So store it in serverMessage, separating packets with "|"
                    SDL_strlcat(serverMessage, packets[packetIndex], SERVERMESSAGE_BUFFER_SIZE);
                    SDL_strlcat(serverMessage, "|", SERVERMESSAGE_BUFFER_SIZE);

                }

            }

			// Free our data and packets to avoid memory leaks
			for (int i = 0; i < numPackets; i++)
				SDL_free(packets[i]);
			SDL_free(packets);
			SDL_free(data);

			// If disconnect flag has been set
			if (disconnect == true)
			{

                // So disconnect from the server and set MULTIPLAYER to false
                disconnectFromServer();
                MULTIPLAYER = false;

                // If the last message we had was "Waiting for players to join"
                if (SDL_strstr(currMessage, "Waiting for players to join") != NULL)
                {

                    // Then that means the server closed, so display that
                    currMessage = SDL_realloc(currMessage, sizeof(char) * SDL_strlen("Server closed") + 1);
                    SDL_strlcpy(currMessage, "Server closed", SDL_strlen("Server closed") + 1);
                    updateConnectionMessageText(&Texture_ConnectionMessage, currMessage);

                }

			}   // If the startGame flag has been set
            else if (startGame == true)
            {

                // Save the players name once the game starts
                saveName(nameString);

                // Go to playMode
                freeVars();
                return PLAY_SCREEN;

            }

        }

        // If the server has told you to ready up and you press SELECT_BUTTON
        if (onPress(SELECT_BUTTON) && *waitingForReady == true)
        {   

            // Tell the server you are ready and print that to the screen
            currMessage = SDL_realloc(currMessage, sizeof(char) * SDL_strlen("READY") + 1);
            SDL_strlcpy(currMessage, "READY", SDL_strlen("READY") + 1);
            updateConnectionMessageText(&Texture_ConnectionMessage, currMessage);

            char message[] = "READY";
            SDLNet_TCP_Send(globalInstance->serverSocket, message, SDL_strlen(message) + 1);  

        }

        // Send a "PULSE" to the server every MULTIPLAYER_PULSE_DELAY_SECONDS so the server knows that we are still connected.
        if ((SDL_GetTicks() - *lastPulseTime) / 1000 > MULTIPLAYER_PULSE_DELAY_SECONDS)
        {

            char message[] = "PULSE";
            SDLNet_TCP_Send(globalInstance->serverSocket, message, SDL_strlen(message) + 1);
            
            *lastPulseTime = SDL_GetTicks();

        }

    }
    else
    {
        
        // If the player is trying to connect to a server
        if (*tryingConnection == true)
        {
            
            // Check if the thread attempting the connection has either already returned or timed out
            if (*connectFunctionReturned || (SDL_GetTicks() - *timeConnectingStarted) / 1000 > CONNECTION_TIMEOUT_SECONDS)
            {
                
                // If it timed out
                if (*connectFunctionReturned == false)
                {

                    // Cancel the thread. This is OS-specific
                    #ifdef _WIN32
                    TerminateThread(OpenThread(THREAD_TERMINATE, false, globalInstance->connectionThreadID), 0);
                    #else
                    pthread_cancel(globalInstance->connectionThreadID);
                    #endif

                    // And display that connection timed out
                    currMessage = SDL_realloc(currMessage, sizeof(char) * SDL_strlen("Connection timed out") + 1);
                    SDL_strlcpy(currMessage, "Connection timed out", SDL_strlen("Connection timed out") + 1);
                    
                    updateConnectionMessageText(&Texture_ConnectionMessage, currMessage);
                    *error = true;

                }   // If it didn't time out, check if a successful connection was established
                else if (!globalInstance->serverSocket)
                {
                    
                    // Display error message to screen
                    const char* temp = SDLNet_GetError();
                    currMessage = SDL_realloc(currMessage, sizeof(char) * SDL_strlen(temp) + 1);
                    SDL_strlcpy(currMessage, temp, SDL_strlen(temp) + 1);

                    updateConnectionMessageText(&Texture_ConnectionMessage, currMessage);
                    *error = true;

                }

                // If there were no errors connecting to the server
                if (*error == false)
                {

                    // Allocate a connection socket and add the connection socket to our socket set
                    // and set MULTIPLAYER to true
                    globalInstance->serverSocketSet = SDLNet_AllocSocketSet(1);
                    SDLNet_TCP_AddSocket(globalInstance->serverSocketSet, globalInstance->serverSocket);

                    // Kepe track of the last time we sent data to the server
                    *lastPulseTime = SDL_GetTicks();

                    MULTIPLAYER = true;
                    
                    // Send name to server
                    char* namePacket;
                    int len = SDL_strlen("NAME=") + SDL_strlen(nameString) + 1;
                    namePacket = SDL_calloc(len, sizeof(char));
                    SDL_strlcpy(namePacket, "NAME=", len);
                    SDL_strlcat(namePacket, nameString, len);
                    SDLNet_TCP_Send(globalInstance->serverSocket, namePacket, len);
                    SDL_free(namePacket);

                    // Set global hosting value so that we know when we are the ones hosting the game that we are in
                    if (*hostingGame)
                        globalInstance->hosting = true;

                }

                *tryingConnection = false;
                *connectFunctionReturned = false;

            }   // Update "Attempting Connection..." counter
            else if (currMessage[SDL_strlen(currMessage) - 1] != '0' + (char)((SDL_GetTicks() - *timeConnectingStarted) / 1000))
            {

                currMessage[SDL_strlen(currMessage) - 1] = '0' + ((SDL_GetTicks() - *timeConnectingStarted) / 1000);
                updateConnectionMessageText(&Texture_ConnectionMessage, currMessage);                

            }

        }
        else if (*hostingGame == true)
        {

            // If the hostingGameData is no longer holding the portString, that means we got some kind of message back from the hosting
            // thread.
            if (SDL_strcmp(hostGameData, portString) != 0)
            {

                // So display that message
                currMessage = SDL_realloc(currMessage, sizeof(char) * SDL_strlen(hostGameData) + 1);
                SDL_strlcpy(currMessage, hostGameData, SDL_strlen(hostGameData) + 1);
                updateConnectionMessageText(&Texture_ConnectionMessage, currMessage);

            }
            

        }

        // Control Logic ----------------------------------------------------
        
        // If you press DOWN
        if (onPress(DOWN_BUTTON))
        {

            // Move cursor down and play MOVE_SOUND if not already at the bottom.
            if (active_list->selected_entry < active_list->num_entries - 1)
            {

                playSound(MOVE_SOUND);
                active_list->selected_entry++;

            }
            
        }

        // If you press UP
        if (onPress(UP_BUTTON))
        {

            // Move cursor up and play MOVE_SOUND if not already at the top
            if (active_list->selected_entry > 0)
            {

                playSound(MOVE_SOUND);
                active_list->selected_entry--;

            }

        }

        if (onPress(SELECT_BUTTON))
        {

            if (connection->ui->currentlyInteracting)
            {

                // When you press SELECT_BUTTON on the CONNECT option
                if (SDL_strcmp(selected_option, "CONNECT") == 0)
                {

                    playSound(ROTATE_SOUND);

                    // Reset error to false whenever we press the CONNECT button
                    *error = false;

                    // Don't allow IP address that doesn't have at least one period
                    if (SDL_strchr(ipString, '.') == NULL)
                    {

                        currMessage = SDL_realloc(currMessage, sizeof(char) * SDL_strlen("Malformed IP Address") + 1);
                        SDL_strlcpy(currMessage, "Malformed IP Address", SDL_strlen("Malformed IP Address") + 1);

                        updateConnectionMessageText(&Texture_ConnectionMessage, currMessage);
                        *error = true;

                    }
                    else
                    {

                        // Resolve the host
                        if (SDLNet_ResolveHost(&(globalInstance->serverIP), ipString, SDL_atoi(portString)) == -1)
                        {

                            // Display error message to screen
                            const char* temp = SDLNet_GetError();
                            currMessage = SDL_realloc(currMessage, sizeof(char) * SDL_strlen(temp) + 1);
                            SDL_strlcpy(currMessage, temp, SDL_strlen(temp) + 1);
        
                            updateConnectionMessageText(&Texture_ConnectionMessage, currMessage);
                            *error = true;

                        }
                        
                        // Attempt connection to server in a separate thread
                        *tryingConnection = true;
                        SDL_CreateThread(openConnection, "openConnection", connectFunctionReturned);
                        *timeConnectingStarted = SDL_GetTicks();

                        // Display that we are attempting to connect to the server
                        currMessage = SDL_realloc(currMessage, sizeof(char) * SDL_strlen("Attempting connection...0") + 1);
                        SDL_strlcpy(currMessage, "Attempting connection...0", SDL_strlen("Attempting connection...0") + 1);
                        updateConnectionMessageText(&Texture_ConnectionMessage, currMessage);

                    }

                }

            }   // Enter the appropriate sub-menu
            else if (multiplayer->ui->currentlyInteracting)
            {

                playSound(ROTATE_SOUND);

                multiplayer->ui->currentlyInteracting = false;

                if (SDL_strcmp(selected_option, "CONNECT") == 0)
                {

                    connection->ui->currentlyInteracting = true;
                    
                    // Load the connect IP and PORT from disk when entering the connection screen and update the ConnectionValues texture
                    loadIPandPortForConnectScreen(ipString, portString);

                    updateConnectionValuesText(Texture_ConnectionValues, ipString, portString, nameString);

                }
                else if (SDL_strcmp(selected_option, "HOST") == 0)
                {

                    hosting->ui->currentlyInteracting = true;

                    // Load the hostPort from disk when entering the hosting screen and update the HostingValues texture
                    loadPortForHostScreen(portString);

                    updateHostingValuesText(Texture_HostingValues, portString, nameString);

                }

            }
            else if (hosting->ui->currentlyInteracting)
            {

                if (SDL_strcmp(selected_option, "HOST") == 0)
                {

                    playSound(ROTATE_SOUND);

                    // Reset error to false whenever we press the HOST button
                    *error = false;

                    // hostGameData is used to transmit data between the game thread and the server thread. Initially, we use it to pass
                    // the port to the server thread
                    hostGameData = SDL_realloc(hostGameData, sizeof(char) * (SDL_strlen(portString) + 1));
                    SDL_strlcpy(hostGameData, portString, SDL_strlen(portString) + 1);

                    // Host server in separate thread, passing the port to host it on in hostGameData
                    SDL_CreateThread(hostGame, "hostGame", (void*)hostGameData);

                    // Now, we attempt to connect to our own server

                    // First, attempt to resolve the host
                    if (SDLNet_ResolveHost(&(globalInstance->serverIP), "127.0.0.1", SDL_atoi(portString)) == -1)
                    {

                        // Display error message to screen
                        const char* temp = SDLNet_GetError();
                        currMessage = SDL_realloc(currMessage, sizeof(char) * SDL_strlen(temp) + 1);
                        SDL_strlcpy(currMessage, temp, SDL_strlen(temp) + 1);
    
                        updateConnectionMessageText(&Texture_ConnectionMessage, currMessage);
                        *error = true;

                    }
                    
                    // Attempt connection to server in a separate thread
                    *tryingConnection = true;
                    SDL_CreateThread(openConnection, "openConnection", connectFunctionReturned);
                    *timeConnectingStarted = SDL_GetTicks();

                    // Display that we are attempting to connect to the server
                    currMessage = SDL_realloc(currMessage, sizeof(char) * SDL_strlen("Attempting connection...0") + 1);
                    SDL_strlcpy(currMessage, "Attempting connection...0", SDL_strlen("Attempting connection...0") + 1);
                    updateConnectionMessageText(&Texture_ConnectionMessage, currMessage);

                    *hostingGame = true;

                }

            }

        }

        // Keep track of the string currently being modified and the maxLength for that string
        char* strBeingModified = NULL;
        int maxLength = 0;

        // Check if left or right control button are being held
        bool controlHeld = (globalInstance->onKeyHold[SDL_SCANCODE_LCTRL] || globalInstance->onKeyHold[SDL_SCANCODE_RCTRL]);

        // Check which option is selected and thus which string is being modified
        if (SDL_strcmp(selected_option, "IP") == 0 || SDL_strcmp(selected_option, "PORT") == 0)
        {

            // Check which one it is, IP or PORT
            strBeingModified = ipString;
            if (SDL_strcmp(selected_option, "PORT") == 0)
                strBeingModified = portString;

        }
        else if (SDL_strcmp(selected_option, "NAME") == 0)
            strBeingModified = nameString;

        // Set the maxLength depending on the string being modified
        if (strBeingModified == ipString)
            maxLength = 15;
        else if (strBeingModified == portString)
            maxLength = 5;
        else if (strBeingModified == nameString)
            maxLength = MAX_NAME_LENGTH;

        // Don't allow typing while holding the control button
        if (controlHeld == false)
        {

            // If the selected connection option is IP or PORT
            if (strBeingModified == ipString || strBeingModified == portString)
            {

                // Define the list of keys which are valid inputs while inputting an IP or PORT
                int validKeys[] = { SDL_SCANCODE_0, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4, SDL_SCANCODE_5,
                                    SDL_SCANCODE_6, SDL_SCANCODE_7, SDL_SCANCODE_8, SDL_SCANCODE_9, SDL_SCANCODE_PERIOD, SDL_SCANCODE_BACKSPACE,
                                    SDL_SCANCODE_KP_0, SDL_SCANCODE_KP_1, SDL_SCANCODE_KP_2, SDL_SCANCODE_KP_3, SDL_SCANCODE_KP_4,
                                    SDL_SCANCODE_KP_5, SDL_SCANCODE_KP_6, SDL_SCANCODE_KP_7, SDL_SCANCODE_KP_8, SDL_SCANCODE_KP_9 };

                // Go through all validKeys
                for (unsigned short i = 0; i < sizeof(validKeys) / sizeof(validKeys[0]); i++)
                {

                    // Check if this key was just pressed this frame. Skip it if it wasn't
                    int pressedKey = validKeys[i];
                    if (globalInstance->onKeyPress[pressedKey] == false)
                        continue;

                    // Only allow digits and periods
                    if (globalInstance->onKeyPress[SDL_SCANCODE_PERIOD] || (pressedKey >= SDL_SCANCODE_1 && pressedKey <= SDL_SCANCODE_0))
                    {

                        // Convert the SDL_SCANCODE to an ascii value
                        char asciiValue;
                        if (pressedKey == SDL_SCANCODE_PERIOD)
                            asciiValue = '.';
                        else if (pressedKey == SDL_SCANCODE_0 || pressedKey == SDL_SCANCODE_KP_0)
                            asciiValue = '0';
                        else if (pressedKey >= SDL_SCANCODE_1 && pressedKey <= SDL_SCANCODE_9)
                            asciiValue = '1' + (pressedKey - SDL_SCANCODE_1);
                        else if (pressedKey >= SDL_SCANCODE_KP_1 && pressedKey <= SDL_SCANCODE_KP_9)
                            asciiValue = '1' + (pressedKey - SDL_SCANCODE_KP_1);

                        // Check if the key that was pressed was a period 
                        bool pressedPeriod = (pressedKey == SDL_SCANCODE_PERIOD);

                        // Don't allow periods in the PORT and dont allow input past maxLength
                        if ((int)SDL_strlen(strBeingModified) < maxLength && !(pressedPeriod && strBeingModified == portString))
                        {

                            // Expand the currently selected string and concatenate the pressedkey
                            int currLength = SDL_strlen(strBeingModified) + 1;
                            strBeingModified = SDL_realloc(strBeingModified, sizeof(char) * (currLength + 1));
                            SDL_strlcat(strBeingModified, &asciiValue, currLength + 1);

                            // Update the values texture depending on what sub-menu we are in
                            if (active_list == connection)
                                updateConnectionValuesText(Texture_ConnectionValues, ipString, portString, nameString);
                            else if (active_list == hosting)
                                updateHostingValuesText(Texture_HostingValues, portString, nameString);

                        }
                        
                    }

                }

            }
            else if (SDL_strcmp(selected_option, "NAME") == 0)  /// If editing the name
            {

                // Allow letters and numbers
                for (unsigned short i = SDL_SCANCODE_A; i <= SDL_SCANCODE_0; i++)
                {

                    // Check if the key was just pressed
                    int pressedKey = i;
                    if (globalInstance->onKeyPress[pressedKey] == false)
                        continue;

                    // Convert the key to ascii
                    char asciiValue;
                    if (i < SDL_SCANCODE_1)
                        asciiValue = 'A' + (i - SDL_SCANCODE_A);
                    else if (i == SDL_SCANCODE_0)
                        asciiValue = '0';
                    else
                        asciiValue = '1' + (i - SDL_SCANCODE_1);

                    // Append asciiValue if name isn't too long
                    if ((int)SDL_strlen(nameString) < maxLength)
                    {

                        int currLength = SDL_strlen(nameString) + 1;
                        nameString = SDL_realloc(nameString, sizeof(char) * (currLength + 1));
                        SDL_strlcat(nameString, &asciiValue, currLength + 1);

                        // Update the values texture depending on what sub-menu we are in
                        if (active_list == connection)
                            updateConnectionValuesText(Texture_ConnectionValues, ipString, portString, nameString);
                        else if (active_list == hosting)
                            updateHostingValuesText(Texture_HostingValues, portString, nameString);

                    }

                }
                
            }

            if (globalInstance->onKeyPress[SDL_SCANCODE_BACKSPACE])  // Erase if user presses backspace
            {

                // Don't allow erase if length of string is already zero
                if ((int)SDL_strlen(strBeingModified) > 0)
                {

                    // Remove the last character from string.
                    int currLength = SDL_strlen(strBeingModified) + 1;
                    strBeingModified = SDL_realloc(strBeingModified, sizeof(char) * (currLength - 1));
                    SDL_strlcpy(strBeingModified, strBeingModified, currLength - 1);

                    // Update the values texture depending on what sub-menu we are in
                    if (active_list == connection)
                        updateConnectionValuesText(Texture_ConnectionValues, ipString, portString, nameString);
                    else if (active_list == hosting)
                        updateHostingValuesText(Texture_HostingValues, portString, nameString);

                }

            }

        }   // If control+V is pressed
        else if (controlHeld && globalInstance->onKeyHold[SDL_SCANCODE_V] && *justPasted == false)
        {

            // Get the text currently on the clipboard
            char* clipboard = SDL_GetClipboardText();

            // Remove characters from clipboard that aren't allow in the string being modified
            for (unsigned short i = 0; i < SDL_strlen(clipboard); i++)
            {

                // Only allow digits and periods in IP and PORT
                if (strBeingModified == ipString || strBeingModified == portString)
                {

                    if (SDL_isdigit(clipboard[i]) == false && clipboard[i] != '.')
                    {

                        SDL_strlcpy(&clipboard[i], &clipboard[i + 1], SDL_strlen(clipboard));
                        i--;

                    }

                }   // Only allow digits and letters in NAME
                else if (strBeingModified == nameString)
                {

                    if (SDL_isdigit(clipboard[i]) == false && SDL_isalpha(clipboard[i]) == false)
                    {

                        SDL_strlcpy(&clipboard[i], &clipboard[i + 1], SDL_strlen(clipboard));
                        i--;

                    }

                }

            }

            // Don't let the user paste more text than can fit in the current value
            int currLength = SDL_strlen(strBeingModified) + 1;
            int newLength = SDL_min(maxLength + 1, (int)SDL_strlen(clipboard) + currLength);
            strBeingModified = SDL_realloc(strBeingModified, sizeof(char) * (newLength + 1));
            
            // Append text from the clipboard to the end of the strBeingModified
            SDL_strlcat(strBeingModified, clipboard, newLength);

            // Update the values texture depending on what sub-menu we are in
            if (active_list == connection)
                updateConnectionValuesText(Texture_ConnectionValues, ipString, portString, nameString);
            else if (active_list == hosting)
                updateHostingValuesText(Texture_HostingValues, portString, nameString);

            // Free memory taken up by copied clipboard text
            SDL_free(clipboard);

            // Don't paste continuously while holding down Control+V
            *justPasted = true;

        }   // Once player lets go of Control or V, they can paste again
        else if (!controlHeld || !globalInstance->onKeyHold[SDL_SCANCODE_V])
            *justPasted = false;

        // Exit to the main menu if EXIT_BUTTON is pressed
        if (onPress(EXIT_BUTTON))
        {

            playSound(LAND_SOUND);

            // If we were connected to a server, we need to disconnect
            if (MULTIPLAYER)
                disconnectFromServer();

            // Exit back to main menu if not in CONNECT or HOST lists
            if (multiplayer->ui->currentlyInteracting)
            {

                // Save the players name if they leave the lobby
                saveName(nameString);

                freeVars();
                return RESET;

            }   // If in not in MULTIPLAYER list, exit back to MULTIPLAYER list
            else if (multiplayer->ui->currentlyInteracting == false)
            {

                // Save the hostPort when exiting the hosting menu
                if (hosting->ui->currentlyInteracting == true)
                    saveHostPort(portString);
                else if (connection->ui->currentlyInteracting == true)
                {

                    // Save the connectPort and connectIP when exiting the connection menu
                    saveIP(ipString);
                    saveConnectPort(portString);

                }

                connection->ui->currentlyInteracting = false;
                hosting->ui->currentlyInteracting = false;
                multiplayer->ui->currentlyInteracting = true;

                // When we leave a sub-menu, reset the portString to an empty string. This is because both the CONNECT and HOST screens
                // have a port option
                portString = SDL_realloc(portString, sizeof(char) * 1);
                SDL_strlcpy(portString, "\0", 1);

                updateConnectionMessageText(&Texture_ConnectionMessage, "\0");

            }

        }

    }

    // Rendering --------------------------------------------------------
    
	drawTexture(Texture_Score, getScoreDrawX(MODE), getScoreDrawY(MODE), 1.0);
    drawTexture(Texture_Level, 312, 115, 1.0);
    drawTexture(Texture_Lines, 312, 189, 1.0);
    drawTexture(Texture_Next, 318 - (*nextText_Width / 2), 282 - (*nextText_Height / 2), 1.0);

    // Draw the currently active list
    int listX = active_list->ui->x;
    int listY = active_list->ui->y;
    drawTexture(active_list->ui->texture, listX, listY, *textMulti);
    int currsorX = listX - 14;
    int cursorY = listY + ((getListSelectedEntryY(active_list) - listY) * *textMulti);
    drawTexture(Texture_Cursor, currsorX, cursorY, *textMulti);

    // Display the values texture of the currently active sub-menu
    if (connection->ui->currentlyInteracting)
        drawTexture(Texture_ConnectionValues, listX + getStringLength("PORT", *textMulti), listY, *textMulti);
    else if (hosting->ui->currentlyInteracting)
        drawTexture(Texture_HostingValues, listX + getStringLength("PORT", *textMulti), listY, *textMulti);

    // Display error message if longer than 0 characters and not in MULTIPLAYER list
    if (SDL_strlen(currMessage) > 0 && multiplayer->ui->currentlyInteracting == false)
    {

        // Give a square of padding on either side of the currMessage
        int width = (SDL_round(BASE_PLAYFIELD_WIDTH * MAX_PIECE_SIZE) - 2) * SPRITE_WIDTH;
        int stringLength = getStringLength(currMessage, 1.0);
        *messageMulti = (double)width / (double)stringLength;
        int x = getConnectionMessageX(currMessage, *messageMulti);
        int y = getConnectionMessageY(*messageMulti);
        drawTexture(Texture_ConnectionMessage, x, y, *messageMulti);

    }

    // ------------------------------------------------------------------

    return MULTIPLAYERLOBBY_SCREEN;

}

// Function for loading the IP and PORT from disk for the CONNECT screen
void loadIPandPortForConnectScreen(char* ipString, char* portString)
{

    char* loadedIP = loadIP();
    if (loadedIP != NULL)
    {

        if (SDL_strlen(loadedIP) > 0)
        {

            ipString = SDL_realloc(ipString, (SDL_strlen(loadedIP) + 1) * sizeof(char));
            SDL_strlcpy(ipString, loadedIP, SDL_strlen(loadedIP) + 1);

        }

        SDL_free(loadedIP);

    }

    char* loadedPort = loadConnectPort();
    if (loadedPort != NULL)
    {

        if (SDL_strlen(loadedPort) > 0)
        {

            portString = SDL_realloc(portString, (SDL_strlen(loadedPort) + 1) * sizeof(char));
            SDL_strlcpy(portString, loadedPort, SDL_strlen(loadedPort) + 1);

        }

        SDL_free(loadedPort);

    }

}

// Function for loading the PORT from disk for the HOST screen
void loadPortForHostScreen(char* portString)
{

    char* loadedPort = loadHostPort();
    if (loadedPort != NULL)
    {

        if (SDL_strlen(loadedPort) > 0)
        {

            portString = SDL_realloc(portString, (SDL_strlen(loadedPort) + 1) * sizeof(char));
            SDL_strlcpy(portString, loadedPort, SDL_strlen(loadedPort) + 1);

        }

        SDL_free(loadedPort);

    }    

}