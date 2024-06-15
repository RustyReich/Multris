#include "MGF.h"

unsigned short multiplayerLobby(piece** Piece, bool* justDisconnected)
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
    static char* currMessage; declareStart(currMessage, '\0');

    if (*firstLoop == true)
    {

        SDL_QueryTexture(Texture_Next, NULL, NULL, nextText_Width, nextText_Height);
        *firstLoop = false;

        // If the player is being returned back to the lobby after being disconnected from a multiplayer game
        if (*justDisconnected)
        {

            // Display "Disconnected from server" on the screen
            currMessage = SDL_realloc(currMessage, sizeof(char) * SDL_strlen("Disconnected from server") + 1);
            SDL_strlcpy(currMessage, "Disconnected from server", SDL_strlen("Disconnected from server") + 1);

            updateConnectionMessageText(&Texture_ConnectionMessage, currMessage);
            *error = true;

            // And the user is now no longer "justDisconnected"
            *justDisconnected = false;

        }

    }

    // If connected to a server
    if (MULTIPLAYER)
    {

        // Check if data is ready to be received from the server
        if (SDLNet_CheckSockets(globalInstance->serverSocketSet, 0))
        {

            // Receive the data if there is any
            char data[1024];
            int len = SDLNet_TCP_Recv(globalInstance->serverSocket, data, 1024);
            data[len] = '\0';

            // If the length of the data is zero, the server closed
            if (len == 0)
            {

                // So disconnect from the server and set MULTIPLAYER to false
                disconnectFromServer();
                MULTIPLAYER = false;

                // If the last message we had was "Waiting for players to join"
                if (SDL_strstr(currMessage, "Waiting for players to join") != NULL)
                {

                    // Then that means we lost connection to the server, so display that
                    currMessage = SDL_realloc(currMessage, sizeof(char) * SDL_strlen("Lost connection to server") + 1);
                    SDL_strlcpy(currMessage, "Lost connection to server", SDL_strlen("Lost connection to server") + 1);
                    updateConnectionMessageText(&Texture_ConnectionMessage, currMessage);

                }

            }
            else    // If the length of the data was not zero
            {

                // Print the messsage received from the server
                currMessage = SDL_realloc(currMessage, sizeof(char) * SDL_strlen(data) + 1);
                SDL_strlcpy(currMessage, data, SDL_strlen(data) + 1);
                updateConnectionMessageText(&Texture_ConnectionMessage, currMessage);

                // If received a messages containing "All players joined", exit multiplayer lobby and move into PLAYMODE.
                if (SDL_strstr(currMessage, "All players joined") != NULL)
                {

                    freeVars();
                    return PLAY_SCREEN;

                }

            }

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
                            
                        }

                    }

                }

            }

        }

        // If the selected connection option is IP or PORT
        if (SDL_strcmp(selected_option, "IP") == 0 || SDL_strcmp(selected_option, "PORT") == 0)
        {

            // Check which one it is, IP or PORT
            char* strBeingModified = ipString;
            if (SDL_strcmp(selected_option, "PORT") == 0)
                strBeingModified = portString;

            // Max length is diffent depending on if the user is editing the IP or the PORT
            int maxLength = 15;
            if (strBeingModified == portString)
                maxLength = 5;

            // Define the list of keys which are valid inputs while inputting an IP and PORT
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
                        updateConnectionValuesText(Texture_ConnectionValues, ipString, portString);

                    }
                    
                }
                else if (pressedKey == SDL_SCANCODE_BACKSPACE)  // Erase if user presses backspace
                {

                    // Don't allow erase if length of string is already zero
                    if ((int)SDL_strlen(strBeingModified) > 0)
                    {

                        // Remove the last character from string.
                        int currLength = SDL_strlen(strBeingModified) + 1;
                        strBeingModified = SDL_realloc(strBeingModified, sizeof(char) * (currLength - 1));
                        SDL_strlcpy(strBeingModified, strBeingModified, currLength - 1);

                        // And then update the ConnectionValues texture
                        updateConnectionValuesText(Texture_ConnectionValues, ipString, portString);

                    }

                }

            }

            // Check if left or right control button are being held
            bool controlHeld = (globalInstance->onKeyHold[SDL_SCANCODE_LCTRL] || globalInstance->onKeyHold[SDL_SCANCODE_RCTRL]);

            // If Control+V are being held
            if (controlHeld && globalInstance->onKeyHold[SDL_SCANCODE_V] && *justPasted == false)
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
                updateConnectionValuesText(Texture_ConnectionValues, ipString, portString);

                // Free memory taken up by copied clipboard text
                SDL_free(clipboard);

                // Don't paste continuously while holding down Control+V
                *justPasted = true;

            }   // Once player lets go of Control or V, they can paste again
            else if (!controlHeld || !globalInstance->onKeyHold[SDL_SCANCODE_V])
                *justPasted = false;

        }

        // Exit to the main menu if EXIT_BUTTON is pressed
        if (onPress(EXIT_BUTTON))
        {

            // If we were connected to a server, we need to disconnect
            if (MULTIPLAYER)
                disconnectFromServer();

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