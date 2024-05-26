#include "MGF.h"

unsigned short multiplayerLobby(piece** Piece)
{

    //Variables
    DECLARE_VARIABLE(int, nextText_Width, 0);
    DECLARE_VARIABLE(int, nextText_Height, 0);
    DECLARE_VARIABLE(bool, firstLoop, true);
    DECLARE_VARIABLE(bool, justPressedButton, false);
    DECLARE_VARIABLE(double, textMulti, 0.80);
    DECLARE_VARIABLE(double, messageMulti, 1.0);

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

    }

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

                // Don'y allow IP address that doesn't have at least one period
                if (SDL_strchr(ipString, '.') == NULL)
                {

                    currMessage = SDL_realloc(currMessage, sizeof(char) * SDL_strlen("Malformed IP Address") + 1);
                    SDL_strlcpy(currMessage, "Malformed IP Address", SDL_strlen("Malformed IP Address") + 1);

                    updateConnectionMessageText(&Texture_ConnectionMessage, currMessage);

                }
                else
                {

                    IPaddress ip;
                    TCPsocket tcpsock;

                    // Open connection to server as specified by ipString and portString
                    if (SDLNet_ResolveHost(&ip, ipString, SDL_atoi(portString)) == -1)
                    {

                        // Display error message to screen
                        const char* temp = SDLNet_GetError();
                        currMessage = SDL_realloc(currMessage, sizeof(char) * SDL_strlen(temp) + 1);
                        SDL_strlcpy(currMessage, temp, SDL_strlen(temp) + 1);
    
                        updateConnectionMessageText(&Texture_ConnectionMessage, currMessage);

                    }

                    tcpsock = SDLNet_TCP_Open(&ip);
                    if (!tcpsock)
                    {

                        // Display error message to screen
                        const char* temp = SDLNet_GetError();
                        currMessage = SDL_realloc(currMessage, sizeof(char) * SDL_strlen(temp) + 1);
                        SDL_strlcpy(currMessage, temp, SDL_strlen(temp) + 1);

                        updateConnectionMessageText(&Texture_ConnectionMessage, currMessage);


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

        // Get the currently pressed key
        int pressedKey = getPressedKey();

        // Only allow one key press at a time. So justPressedButton is false if no buttons are being pressed, to allow
        // a new key press
        if (getPressedKey() == -1)
            *justPressedButton = false;

        // As long the the user didnt just press a key already
        if (*justPressedButton == false)
        {

            // Only allow digits and periods
            if (pressedKey == SDL_SCANCODE_PERIOD || (pressedKey >= SDL_SCANCODE_1 && pressedKey <= SDL_SCANCODE_0))
            {
                
                // Convert the SDL_SCANCODE to an ascii value
                char asciiValue;
                if (pressedKey == SDL_SCANCODE_PERIOD)
                    asciiValue = '.';
                else if (pressedKey == SDL_SCANCODE_0)
                    asciiValue = '0';
                else
                    asciiValue = '1' + (pressedKey - SDL_SCANCODE_1);

                // Max length is diffent depending on if the user is editing the IP or the PORT
                int maxLength = 15;
                if (strBeingModified == portString)
                    maxLength = 5;

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

                    // And don't allow multiple inputs from a single key press
                    *justPressedButton = true;

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

                    // And don't allow multiple inputs from a single key press
                    *justPressedButton = true;

                }

            }

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

        int stringLength = getStringLength(currMessage, 1.0);
        *messageMulti = SDL_round(BASE_PLAYFIELD_WIDTH * MAX_PIECE_SIZE) * SPRITE_WIDTH / stringLength;
        int x = getConnectionMessageX(currMessage, *messageMulti);
        int y = getConnectionMessageY(*messageMulti);
        drawTexture(Texture_ConnectionMessage, x, y, *messageMulti);

    }

    // ------------------------------------------------------------------

    return MULTIPLAYERLOBBY_SCREEN;

}