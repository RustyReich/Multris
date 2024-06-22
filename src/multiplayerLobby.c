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

    //Textures
    static SDL_Texture* Texture_Score; declare_HUD_Text(&Texture_Score, SCORE_TEXT);
    static SDL_Texture* Texture_Level; declare_HUD_Text(&Texture_Level, LEVEL_TEXT);
    static SDL_Texture* Texture_Lines; declare_HUD_Text(&Texture_Lines, LINES_TEXT);
    static SDL_Texture* Texture_Next; declare_Piece_Text(&Texture_Next, *Piece, false);
    static SDL_Texture* Texture_Cursor; declare_HUD_Text(&Texture_Cursor, CURSOR_TEXT);
    static SDL_Texture* Texture_ConnectionValues; declare_HUD_Text(&Texture_ConnectionValues, CONNECTIONVALUES_TEXT);
    static SDL_Texture* Texture_ConnectionMessage; declare_HUD_Text(&Texture_ConnectionMessage, CONNECTIONMESSAGE_TEXT);

    //UI elements
    static UI_list* connection; declare_UI_list(&connection, CONNECTION_LIST);

    //Arrays
    static char* ipString; declareStart(ipString, '\0');
    static char* portString; declareStart(portString, '\0');
    static char* nameString; declareStart(nameString, '\0');
    static char* currMessage; declareStart(currMessage, '\0');

    if (*firstLoop == true)
    {

        SDL_QueryTexture(Texture_Next, NULL, NULL, nextText_Width, nextText_Height);
        *firstLoop = false;

        // If the player is being returned back to the lobby after being disconnected from a multiplayer game
        if (SDL_strlen(serverMessage) > 0)
        {

            // Display serverMessage on the screen
            currMessage = SDL_realloc(currMessage, sizeof(char) * (SDL_strlen(serverMessage) + 1));
            SDL_strlcpy(currMessage, serverMessage, (SDL_strlen(serverMessage) + 1));

            updateConnectionMessageText(&Texture_ConnectionMessage, currMessage);
            *error = true;

            // Erase serverMessage since now we are already displaying it on the screen
            SDL_strlcpy(serverMessage, "\0", 1);

        }

        // Load the players name from the name file if it is there
        char* loadedName = loadName();
        if (loadedName != NULL)
        {

            if (SDL_strlen(loadedName) > 0)
            {

                nameString = SDL_realloc(nameString, (SDL_strlen(loadedName) + 1) * sizeof(char));
                SDL_strlcpy(nameString, loadedName, SDL_strlen(loadedName) + 1);

                updateConnectionValuesText(Texture_ConnectionValues, ipString, portString, nameString);

            }

            SDL_free(loadedName);

        }

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
                        currMessage = SDL_realloc(currMessage, sizeof(char) * SDL_strlen(data) + 1);
                        SDL_strlcpy(currMessage, data, SDL_strlen(data) + 1);
                        updateConnectionMessageText(&Texture_ConnectionMessage, currMessage);

                        if (SDL_strstr(currMessage, "Press SELECT when ready") != NULL)
                            *waitingForReady = true;

                    }

                }   // If the received packet is the name of the opponent, store it in serverMessage
				else if (SDL_strstr(packets[packetIndex], "NAME=") != NULL)
                    SDL_strlcat(serverMessage, packets[packetIndex], SERVERMESSAGE_BUFFER_SIZE);

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

			}   // If the starGame flag has been set
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

        // Control Logic ----------------------------------------------------

        // Keep track of the current selected connection option
        const char* selected_option = getListSelectedString(connection);

        // If you press DOWN
        if (onPress(DOWN_BUTTON))
        {

            if (connection->ui->currentlyInteracting)
            {

                // Move cursor down and play MOVE_SOUND if not already at the bottom.
                if (connection->selected_entry < connection->num_entries - 1)
                {

                    playSound(MOVE_SOUND);
                    connection->selected_entry++;

                }

            }
            
        }

        // If you press UP
        if (onPress(UP_BUTTON))
        {

            if (connection->ui->currentlyInteracting)
            {

                // Move cursor up and play MOVE_SOUND if not already at the top
                if (connection->selected_entry > 0)
                {

                    playSound(MOVE_SOUND);
                    connection->selected_entry--;

                }

            }

        }

        if (onPress(SELECT_BUTTON))
        {

            if (connection->ui->currentlyInteracting)
            {

                // When you press SELECT_BUTTON on the CONNECT option
                if (SDL_strcmp(selected_option, "CONNECT") == 0)
                {

                    // Reset error to false whenever we press the CONNECT button
                    *error = false;

                    // Don'y allow IP address that doesn't have at least one period
                    if (SDL_strchr(ipString, '.') == NULL)
                    {

                        currMessage = SDL_realloc(currMessage, sizeof(char) * SDL_strlen("Malformed IP Address") + 1);
                        SDL_strlcpy(currMessage, "Malformed IP Address", SDL_strlen("Malformed IP Address") + 1);

                        updateConnectionMessageText(&Texture_ConnectionMessage, currMessage);
                        *error = true;

                    }
                    else
                    {

                        // Open connection to server as specified by ipString and portString
                        if (SDLNet_ResolveHost(&(globalInstance->serverIP), ipString, SDL_atoi(portString)) == -1)
                        {

                            // Display error message to screen
                            const char* temp = SDLNet_GetError();
                            currMessage = SDL_realloc(currMessage, sizeof(char) * SDL_strlen(temp) + 1);
                            SDL_strlcpy(currMessage, temp, SDL_strlen(temp) + 1);
        
                            updateConnectionMessageText(&Texture_ConnectionMessage, currMessage);
                            *error = true;

                        }

                        globalInstance->serverSocket = SDLNet_TCP_Open(&(globalInstance->serverIP));
                        if (!globalInstance->serverSocket)
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

                            // Allocate a connection socket and add the connection socket to our socket set and set 
                            // MULTIPLAYER to true
                            globalInstance->serverSocketSet = SDLNet_AllocSocketSet(1);
                            SDLNet_TCP_AddSocket(globalInstance->serverSocketSet, globalInstance->serverSocket);
                            
                            // Keep track of the last time we sent data to the server
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

                        }

                    }

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
                                    SDL_SCANCODE_6, SDL_SCANCODE_7, SDL_SCANCODE_8, SDL_SCANCODE_9, SDL_SCANCODE_PERIOD, SDL_SCANCODE_BACKSPACE };

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
                        else if (pressedKey == SDL_SCANCODE_0)
                            asciiValue = '0';
                        else
                            asciiValue = '1' + (pressedKey - SDL_SCANCODE_1);

                        // Check if the key that was pressed was a period 
                        bool pressedPeriod = (pressedKey == SDL_SCANCODE_PERIOD);

                        // Don't allow periods in the PORT and dont allow input past maxLength
                        if ((int)SDL_strlen(strBeingModified) < maxLength && !(pressedPeriod && strBeingModified == portString))
                        {

                            // Expand the currently selected string and concatenate the pressedkey
                            int currLength = SDL_strlen(strBeingModified) + 1;
                            strBeingModified = SDL_realloc(strBeingModified, sizeof(char) * (currLength + 1));
                            SDL_strlcat(strBeingModified, &asciiValue, currLength + 1);

                            // Then update the Connectionvalues texture
                            updateConnectionValuesText(Texture_ConnectionValues, ipString, portString, nameString);

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

                        updateConnectionValuesText(Texture_ConnectionValues, ipString, portString, nameString);

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

                    // And then update the ConnectionValues texture
                    updateConnectionValuesText(Texture_ConnectionValues, ipString, portString, nameString);

                }

            }

        }   // If control+V is pressed
        else if (controlHeld && globalInstance->onKeyHold[SDL_SCANCODE_V] && *justPasted == false)
        {

            // Get the text currently on the clipboard
            char* clipboard = SDL_GetClipboardText();

            // Don't let the user paste more text than can fit in the current value
            int currLength = SDL_strlen(strBeingModified) + 1;
            int newLength = SDL_min(maxLength + 1, SDL_strlen(clipboard) + currLength);
            strBeingModified = SDL_realloc(strBeingModified, sizeof(char) * (newLength + 1));
            
            // Append text from the clipboard to the end of the strBeingModified
            SDL_strlcat(strBeingModified, clipboard, newLength);

            // And then update the ConnectionValues texture
            updateConnectionValuesText(Texture_ConnectionValues, ipString, portString, nameString);

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

            // If we were connected to a server, we need to disconnect
            if (MULTIPLAYER)
                disconnectFromServer();

            // Save the players name if they leave the lobby
            saveName(nameString);

            playSound(LAND_SOUND);
            freeVars();
            return RESET;

        }

    }

    // Rendering --------------------------------------------------------
    
	drawTexture(Texture_Score, getScoreDrawX(MODE), getScoreDrawY(MODE), 1.0);
    drawTexture(Texture_Level, 312, 115, 1.0);
    drawTexture(Texture_Lines, 312, 189, 1.0);
    drawTexture(Texture_Next, 318 - (*nextText_Width / 2), 282 - (*nextText_Height / 2), 1.0);

    int connectionX = connection->ui->x;
    int connectionY = connection->ui->y;
    drawTexture(connection->ui->texture, connectionX, connectionY, *textMulti);
    int currsorX = connectionX - 14;
    int cursorY = connectionY + ((getListSelectedEntryY(connection) - connectionY) * *textMulti);
    drawTexture(Texture_Cursor, currsorX, cursorY, *textMulti);

    drawTexture(Texture_ConnectionValues, connectionX + getStringLength("PORT", *textMulti), connectionY, *textMulti);
    
    // Display error message if longer than 0 characters
    if (SDL_strlen(currMessage) > 0)
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