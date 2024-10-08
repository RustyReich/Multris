#include "MGF.h"

// Function for disconnecting from a server
void disconnectFromServer()
{

	// Close the socket
    SDLNet_TCP_Close(globalInstance->serverSocket);
	// Free the socket set
    SDLNet_FreeSocketSet(globalInstance->serverSocketSet);
	// And set MULTIPLAYER to false
    MULTIPLAYER = false;

}

// Function for sending garbage data to the server
void sendGarbageToServer(int amount, int* lastPulseTime)
{

	// Convert the amount into a string
	char* amountString = SDL_calloc(getIntLength(amount) + 1, sizeof(char));
	SDL_itoa(amount, amountString, 10);
	amountString[getIntLength(amount)] = '\0';

	// Prepend the "GARBAGE=" header to the data string
	int len = SDL_strlen("GARBAGE=") + SDL_strlen(amountString) + 1;
	char* data = SDL_calloc(len, sizeof(char));
	SDL_strlcpy(data, "GARBAGE=", len);
	SDL_strlcat(data, amountString, len);

	// Send data to server
	SDLNet_TCP_Send(globalInstance->serverSocket, data, len);

	// Keep track of when last communication with server was
	*lastPulseTime = SDL_GetTicks();

	// Free strings to avoid memory leaks
	SDL_free(amountString);
	SDL_free(data);	

}

// Function for sending a list of rows to remove to the server
	// Data is sent to server in the format "REMOVE=[row #]|[row #]|...|[row #]|"
void sendRemovalToServer(int* rows, int numRows, int *lastPulseTime)
{

	// Add header to packet
	int len = SDL_strlen("REMOVE=") + 1;
	char* data = SDL_calloc(len, sizeof(char));
	SDL_strlcpy(data, "REMOVE=", len);

	// Go through each row in the array of rows and append them to our data string separated by "|"
	for (unsigned short i = 0; i < numRows; i++)
	{

		char* rowString = SDL_calloc(getIntLength(rows[i]), sizeof(char));
		SDL_itoa(rows[i], rowString, 10);
		rowString[getIntLength(rows[i])] = '\0';

		len = SDL_strlen(data) + SDL_strlen(rowString) + SDL_strlen("|") + 1;
		data = SDL_realloc(data, len * sizeof(char));
		SDL_strlcat(data, rowString, len);
		SDL_strlcat(data, "|", len);

		SDL_free(rowString);

	}

	// Send the data to the server
	SDLNet_TCP_Send(globalInstance->serverSocket, data, len);

	// Keep track of the last time we communicated with the server
	*lastPulseTime = SDL_GetTicks();

	// Free the data to avoid memory leaks
	SDL_free(data);

}

// Function for sending current sizeBag to the server
	// Sent in the format "SIZEBAG=|size_1|size_2|size_3|...|"
void sendSizeBagToServer(SizeBag* sizeBag, int* lastPuleTime)
{

	// Add header to packet
	int len = SDL_strlen("SIZEBAG=") + 1;
	char* data = SDL_calloc(len, sizeof(char));
	SDL_strlcpy(data, "SIZEBAG=", len);

	// Go through each size in the bag and append it as a string to our data
	for (unsigned short i = 0; i < sizeBag->size; i++)
	{

		char* sizeString = SDL_calloc(getIntLength(sizeBag->sizesInBag[i]), sizeof(char));
		SDL_itoa(sizeBag->sizesInBag[i], sizeString, 10);
		sizeString[getIntLength(sizeBag->sizesInBag[i])] = '\0';

		len = SDL_strlen(data) + SDL_strlen(sizeString) + SDL_strlen("|") + 1;
		data = SDL_realloc(data, len * sizeof(char));
		SDL_strlcat(data, sizeString, len);
		SDL_strlcat(data, "|", len);

		// Free the sizeString to avoid memory leaks
		SDL_free(sizeString);

	}

	// Send the data to the server
	SDLNet_TCP_Send(globalInstance->serverSocket, data, len);

	// Keep track of the last time we communicated with the server
	*lastPuleTime = SDL_GetTicks();

	// Free the data to avoid memory leaks
	SDL_free(data);

}

// Function for sending current position to the server
void sendPositionToServer(int X, int Y, int* lastPulseTime)
{

	// Convert X value into a string
	char* xString = SDL_calloc(getIntLength(X) + 1, sizeof(char));
	SDL_itoa(X, xString, 10);
	xString[getIntLength(X)] = '\0';

	// Conver Y value into a string
	char* yString = SDL_calloc(getIntLength(Y) + 1, sizeof(char));
	SDL_itoa(Y, yString, 10);
	yString[getIntLength(Y)] = '\0';

	// Send the position to the server in the format "POSITION=X|Y|"
	int len = SDL_strlen("POSITION=") + SDL_strlen(xString) + SDL_strlen("|") + SDL_strlen(yString) + SDL_strlen("|") + 1;
	char* data = SDL_calloc(len, sizeof(char));
	SDL_strlcpy(data, "POSITION=", len);
	SDL_strlcat(data, xString, len);
	SDL_strlcat(data, "|", len);
	SDL_strlcat(data, yString, len);
	SDL_strlcat(data, "|", len);

	SDL_free(xString);
	SDL_free(yString);

	SDLNet_TCP_Send(globalInstance->serverSocket, data, len);

	*lastPulseTime = SDL_GetTicks();

	SDL_free(data);

}

// Function for sending level data to the server
void sendLevelToServer(int level, int* lastPulseTime)
{

	// Convert the level into a string
	char* levelString = SDL_calloc(getIntLength(level) + 1, sizeof(char));
	SDL_itoa(level, levelString, 10);
	levelString[getIntLength(level)] = '\0';

	// Prepend the "LEVEL=" header to the data string
	int len = SDL_strlen("LEVEL=") + SDL_strlen(levelString) + 1;
	char* data = SDL_calloc(len, sizeof(char));
	SDL_strlcpy(data, "LEVEL=", len);
	SDL_strlcat(data, levelString, len);

	// Ensure data ends in null-byte
	data[len] = '\0';

	// Send data to server
	SDLNet_TCP_Send(globalInstance->serverSocket, data, len);

	// Keep track of when last communication with server was
	*lastPulseTime = SDL_GetTicks();

	// Free the levelString to avoid memory leaks
	SDL_free(levelString);

	SDL_free(data);

}

// Function for sending lines data to the server
void sendLinesToServer(int lines, int* lastPulseTime)
{

	// Convert the lines value into a string
	char* linesString = SDL_calloc(getIntLength(lines) + 1, sizeof(char));
	SDL_itoa(lines, linesString, 10);
	linesString[getIntLength(lines)] = '\0';

	// Prepend the "LINES=" header to the data string
	int len = SDL_strlen("LINES=") + SDL_strlen(linesString) + 1;
	char* data = SDL_calloc(len, sizeof(char));
	SDL_strlcpy(data, "LINES=", len);
	SDL_strlcat(data, linesString, len);

	// Ensure data ends in null-byte
	data[len] = '\0';

	// Send data to server
	SDLNet_TCP_Send(globalInstance->serverSocket, data, len);

	// Keep track of when last communication with server was
	*lastPulseTime = SDL_GetTicks();

	// Free the linesString to avoid memory leaks
	SDL_free(linesString);

	SDL_free(data);

}

// Send the current CURRENT piece to the server
void sendCurrentPieceToServer(piece* currentPiece, int *lastPulseTime)
{

	// Convert the piece to a string
	char* currentPieceAsString = convertPieceToString(currentPiece);

	// Pre-pend "CURRENT=" to the string
	int len = SDL_strlen("CURRENT=") + SDL_strlen(currentPieceAsString) + 1;
	char* data = SDL_calloc(len, sizeof(char));
	SDL_strlcpy(data, "CURRENT=", len);
	SDL_strlcat(data, currentPieceAsString, len);

	// Send the string to the server
	SDLNet_TCP_Send(globalInstance->serverSocket, data, len);

	// Keep track of when last communication with server was
	*lastPulseTime = SDL_GetTicks();

	// Free the strings to avoid memory leaks
	SDL_free(currentPieceAsString);
	SDL_free(data);

}

// Function for sending a NEXT piece to the server
void sendNextPieceToServer(piece* nextPiece, int* lastPulseTime)
{

	// Conver the piece to a string
	char* nextPieceAsString = convertPieceToString(nextPiece);

	// Pre-pend "NEXT=" to the string
	int len = SDL_strlen("NEXT=") + SDL_strlen(nextPieceAsString) + 1;
	char* data = SDL_calloc(len, sizeof(char));
	SDL_strlcpy(data, "NEXT=", len);
	SDL_strlcat(data, nextPieceAsString, len);

	// Send the string to the server
	SDLNet_TCP_Send(globalInstance->serverSocket, data, len);

	// Keep track of the last time data was sent to the server
	*lastPulseTime = SDL_GetTicks();

	// Free the strings to avoid memory leaks
	SDL_free(nextPieceAsString);
	SDL_free(data);

}

// Function for sending a HOLD piece to the server
void sendHoldPieceToServer(piece* holdPiece, int* lastPulseTime)
{

	// Conver the piece to a string
	char* holdPieceAsString = convertPieceToString(holdPiece);

	// Pre-pend "HOLD=" to the string
	int len = SDL_strlen("HOLD=") + SDL_strlen(holdPieceAsString) + 1;
	char* data = SDL_calloc(len, sizeof(char));
	SDL_strlcpy(data, "HOLD=", len);
	SDL_strlcat(data, holdPieceAsString, len);

	// Send the string to the server
	SDLNet_TCP_Send(globalInstance->serverSocket, data, len);

	// Keep track of the last time data was sent to the server
	*lastPulseTime = SDL_GetTicks();

	// Free the strings to avoid memory leaks
	SDL_free(holdPieceAsString);
	SDL_free(data);

}

// Function for sending score data to the server
void sendScoretoServer(int score, int* lastPulseTime)
{

	// Convert the score into a string
	char* scoreString = SDL_calloc(getIntLength(score) + 1, sizeof(char));
	SDL_itoa(score, scoreString, 10);
	scoreString[getIntLength(score)] = '\0';

	// Prepend the "SCORE=" header to the data string
	int len = SDL_strlen("SCORE=") + SDL_strlen(scoreString) + 1;
	char* data = SDL_calloc(len, sizeof(char));
	SDL_strlcpy(data, "SCORE=", len);
	SDL_strlcat(data, scoreString, len);

	// Ensure data ends in null-byte
	data[len] = '\0';

	// Send data to server
	SDLNet_TCP_Send(globalInstance->serverSocket, data, len);

	// Keep track of when last communication with server was
	*lastPulseTime = SDL_GetTicks();

	// Free the scoreString to avoid memory leaks
	SDL_free(scoreString);

	SDL_free(data);

}

// Function for sending mapData to the server
void sendMapToServer(int* mapData, int* lastPuleTime)
{

	int len = MAP_WIDTH * MAP_HEIGHT + SDL_strlen("MAP=") + 1;
	char* data = SDL_calloc(len, sizeof(char));
	SDL_strlcpy(data, "MAP=", len);
	
	int index = SDL_strlen("MAP=");

	for (unsigned short i = 0; i < MAP_HEIGHT; i++)
	{

		for (unsigned short j = 0; j < MAP_WIDTH; j++)
		{

			// MAP data is sent as a string of digits
			data[index] = '0' + (char)*(mapData + i * MAP_WIDTH + j);

			index++;

		}

	}

	// MAP data ends with a null-terminator
	data[len] = '\0';

	// Send the data
	SDLNet_TCP_Send(globalInstance->serverSocket, data, len);

	// Keep track of the last time we sent data to the server.
	*lastPuleTime = SDL_GetTicks();

	// Then free the data to avoid memory leaks
	SDL_free(data);

}

// Function for connecting to a server in a separate thread
int openConnection(void* data)
{

	// Keep track of the ID for this tread so it can be canceled if it timesout
	globalInstance->connectionThreadID = SDL_GetThreadID(NULL);
	
	// Attempt the connection
	globalInstance->serverSocket = SDLNet_TCP_Open(&(globalInstance->serverIP));
	
	// If failed to open connection, return the error message to the main thread via the data variable
	if (globalInstance->serverSocket == NULL)
	{

		const char* temp = SDLNet_GetError();
		SDL_strlcpy(data, temp, HOST_GAME_DATA_MAX_LENGTH);

		return 0;

	}

	// Keep track of if the SDLNet_TCP_Open function returned
	SDL_strlcpy(data, "true", HOST_GAME_DATA_MAX_LENGTH);

	return 0;

}

// Function for starting a server in a separate thread. The data variable is used to transmit information between the server thread and the
// game thread
int hostGame(void* data)
{

	IPaddress localIP;

	// If we can't resolve the host
	if (SDLNet_ResolveHost(&localIP, NULL, SDL_atoi((char*)data)) == -1)
	{

		// Send the error back to the game thread via the data variable
		const char* temp = SDLNet_GetError();
		SDL_strlcpy(data, temp, HOST_GAME_DATA_MAX_LENGTH);

		return 0;                   

	}

	// If the server returns with a value other than 0
	if (startServer(localIP, SERVER_TICK_RATE) != 0)
	{

		// Send the error back to the game thread via the data variable
		const char* temp = SDLNet_GetError();
		SDL_strlcpy(data, temp, HOST_GAME_DATA_MAX_LENGTH);

	}

	return 0;

}