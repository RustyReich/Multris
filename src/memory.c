#include "MGF.h"

//Function for pushing a variable or object onto a varVector structure 
//designed to make freeing variables at the end of a game_state easier
varVector** pushAddress(void** ptr, unsigned short type)
{

    //If this is the first address being pushed
    static varVector* vector;
    if (vector == NULL) {
        
        //Allocate memory for the varVector
        vector = SDL_calloc(1, sizeof(*vector));
        vector->count = 0;

    }

    //If we passed a NULL ptr, return the address of the varVector
    if(ptr == NULL)
        return &vector;
    else    //Otherwise, we want to push the pointer onto the varVector
    {

        //If this is the first pointer being pushed since the last time they were all freed
        if (vector->ptrs == NULL)
        {

            //Allocate memory for the ptrs array and the types array
            vector->ptrs = SDL_calloc(1, sizeof(void*));
            vector->types = SDL_calloc(1, sizeof(unsigned short));

        }
        else    //Otherwise, we need to reallocate ptrs and types to increase their size by 1
        {

            vector->ptrs = SDL_realloc(vector->ptrs, (vector->count + 1) * sizeof(void*));
            vector->types = SDL_realloc(vector->types, (vector->count + 1) * sizeof(unsigned short));

        }
        
        //Store the pointer to the variable and the type of variable into the arrays
        vector->ptrs[vector->count] = ptr;
        vector->types[vector->count] = type;

        //Increase the count of the varVector
        vector->count++;

        return NULL;

    }

}

//Function for checking if a ptr is already stored in the varVector
bool inVector(void** ptr)
{

    varVector** vector = pushAddress(NULL, TYPE_NA);

    for (int i = 0; i < (*vector)->count; i++)
        if (ptr == (*vector)->ptrs[i])
            return true;
    return false;

}

//Function for declaring a short on the varVector array
void declare_short(void** ptr, short value)
{

    if (*ptr == NULL)
    {

        *ptr = SDL_calloc(1, sizeof(short));
        **(short**)(ptr) = value;
        
        if (!inVector(ptr))
            pushAddress(ptr, VARIABLE);

    }

}

//Function for declaring an unsigned short on the varVector array
void declare_unsigned_short(void** ptr, unsigned short value)
{

    if (*ptr == NULL)
    {

        *ptr = SDL_calloc(1, sizeof(unsigned short));
        **(unsigned short**)(ptr) = value;
        
        if (!inVector(ptr))
            pushAddress(ptr, VARIABLE);

    }

}

// Function for declaring an unsigned long on the varVector array
void declare_unsigned_long(void** ptr, unsigned long value)
{

    if (*ptr == NULL)
    {

        *ptr = SDL_calloc(1, sizeof(unsigned long));
        **(unsigned long**)(ptr) = value;
        
        if (!inVector(ptr))
            pushAddress(ptr, VARIABLE);

    }

}

//Function for declaring a double on the varVector array
void declare_double(void** ptr, double value)
{

    if (*ptr == NULL)
    {

        *ptr = SDL_calloc(1, sizeof(double));
        **(double**)(ptr) = value;
        
        if (!inVector(ptr))
            pushAddress(ptr, VARIABLE);

    }

}

//Function for declaring an int on the varVector array
void declare_int(void** ptr, int value)
{

    if (*ptr == NULL)
    {

        *ptr = SDL_calloc(1, sizeof(int));
        **(int**)(ptr) = value;
        
        if (!inVector(ptr))
            pushAddress(ptr, VARIABLE);

    }

}

//Function for declaring a char on the varVector array
void declare_char(void** ptr, char value)
{

    if (*ptr == NULL)
    {

        *ptr = SDL_calloc(1, sizeof(char));
        **(char**)(ptr) = value;
        
        if (!inVector(ptr))
            pushAddress(ptr, VARIABLE);

    }

}

//Function for declaring a bool on the varVector array
void declare_bool(void** ptr, bool value)
{

    if (*ptr == NULL)
    {

        *ptr = SDL_calloc(1, sizeof(bool));
        **(bool**)(ptr) = value;
        
        if (!inVector(ptr))
            pushAddress(ptr, VARIABLE);

    }

}

//Function for declaring an unsigned int on the varVector array
void declare_unsigned_int(void** ptr, unsigned int value)
{

    if (*ptr == NULL)
    {

        *ptr = SDL_calloc(1, sizeof(unsigned int));
        **(unsigned int**)(ptr) = value;
        
        if (!inVector(ptr))
            pushAddress(ptr, VARIABLE);

    }

}

//Function for declaring a Piece on the varVector array
void declare_Piece(piece** ptr, piece* Piece)
{
    
    if (*ptr == NULL)
    {

        if (Piece != NULL)
        {

            *ptr = SDL_calloc(1, sizeof(piece));
            (*ptr)->blocks = SDL_calloc(Piece->numOfBlocks, sizeof(*(*ptr)->blocks));
            (*ptr)->centerBlock = SDL_calloc(1, sizeof(block));
            copyPiece(Piece, *ptr);

        }

        if (!inVector((void**)ptr))
            pushAddress((void**)ptr, PIECE);

    }

}

//Function for declaring a Piece Texture on the varVector array
void declare_Piece_Text(SDL_Texture** ptr, piece* Piece, bool drawCenterDot)
{

    if (*ptr == NULL)
    {

        if (Piece != NULL)
            *ptr = createPieceTexture(*Piece, drawCenterDot);

        if (!inVector((void**)ptr))
            pushAddress((void**)ptr, TEXTURE);

    }

}

//Function for declaring a HUD Texture on the varVector array
void declare_HUD_Text(SDL_Texture** ptr, int type)
{

    if (*ptr == NULL)
    {

        //Call a different function depending on the the type of HUD texture we are trying to make
        if (type == SCORE_TEXT)
            *ptr = create_Score_Text();
        else if (type == LEVEL_TEXT)
            *ptr = create_Level_Text();
        else if (type == LINES_TEXT)
            *ptr = create_Lines_Text();
        else if (type == PAUSED_TEXT)
            *ptr = create_Pause_Text();
        else if (type == FOREGROUND_TEXT)
            *ptr = create_Foreground_Text();
        else if (type == TITLE_TEXT)
            *ptr = create_Title_Text();
        else if (type == CURSOR_TEXT)
            *ptr = create_Cursor_Text();
        else if (type == VALUES_TEXT)
            *ptr = create_Values_Text();
        else if (type == VOLSLIDE_TEXT)
            *ptr = create_volSlide_Text();
        else if (type == CONTROLS_TEXT)
            *ptr = create_Controls_Text();
        else if (type == CUSTOM_TEXT)
            *ptr = create_Custom_Text();
        else if (type == SIZEBAG_TEXT)
            *ptr = create_SizeBag_Text();
        else if (type == CONNECTIONVALUES_TEXT)
            *ptr = create_ConnectionValues_Text();
        else if (type == CONNECTIONMESSAGE_TEXT)
            *ptr = create_ConnectionMessage_Text();
        else if (type == NAME_TEXT)
            *ptr = create_Name_Text();
        else if (type == HOSTINGVALUES_TEXT)
            *ptr = create_HostingValues_Text();
        else if (type == PAUSEDVALUES_TEXT)
            *ptr = create_PausedValues_Text();

        if(!inVector((void**)ptr))
            pushAddress((void**)ptr, TEXTURE);

    }

}

//Declare the map data matrix
void declare_map_matrix(int** ptr)
{

    if (*ptr == NULL)
    {

        *ptr = (int*)(SDL_malloc(MAP_HEIGHT * MAP_WIDTH * sizeof(**ptr)));
        if (*ptr != NULL)
            for (unsigned short i = 0; i < MAP_HEIGHT; i++)
                for (unsigned short j = 0; j < MAP_WIDTH; j++)
                    *(*ptr + i * MAP_WIDTH + j) = 0;

        if (!inVector((void**)ptr))
            pushAddress((void**)ptr, VARIABLE);

    }

}

// Declare the size bag for bag shuffling the sizes
void declare_size_bag(SizeBag** ptr, unsigned short mode, bool customMode)
{

    if (*ptr == NULL)
    {

        *ptr = (SizeBag*)(SDL_calloc(1, sizeof(SizeBag)));

        if (mode == 0)
        {

            if (*ptr != NULL)
            {

                // In MULTRIS mode, sizes 1 - MAX_PIECE_SIZE are in the bag
                (*ptr)->sizesInBag = SDL_calloc(MAX_PIECE_SIZE, sizeof(unsigned short));
                (*ptr)->size = MAX_PIECE_SIZE;

                for (unsigned short i = 1; i <= MAX_PIECE_SIZE; i++)
                    (*ptr)->sizesInBag[i - 1] = i;

            }

        }
        else if (mode != 0 && customMode == true)
        {

            if (*ptr != NULL)
            {

                // In CUSTOM mode, sizes 1 - MODE are in the bag
                (*ptr)->sizesInBag = SDL_calloc(mode, sizeof(unsigned short));
                (*ptr)->size = mode;

                for (unsigned short i = 1; i <= mode; i++)
                    (*ptr)->sizesInBag[i - 1] = i;

            }

        }
        else if (mode != 0 && customMode == false)
        {

            // In NUMERICAL mode, only size MODE is in the bag
            (*ptr)->sizesInBag = SDL_calloc(1, sizeof(unsigned short));
            (*ptr)->size = 1;

            (*ptr)->sizesInBag[0] = mode;

        }

        if (!inVector((void**)ptr))
            pushAddress((void**)ptr, SIZE_BAG);

    }

}

//Declare the array of moving pieces in the title
void declare_moving_title_pieces(piece*** ptr)
{

    if (*ptr == NULL)
    {

        *ptr = getMovingPieces(makeTitlePieces());

        if (!inVector((void**)ptr))
            pushAddress((void**)ptr, MOVING_TITLE_PIECES);

    }

}

//Declare a UI_list
void declare_UI_list(UI_list** ptr, int type)
{

    if (*ptr == NULL)
    {

        //Call a different function depending on which list we are trying to make
        if (type == MODES_LIST)
            *ptr = create_Modes_List();
        else if (type == NUMERICAL_LIST)
            *ptr = create_Numerical_List();
        else if (type == CUSTOM_LIST)
            *ptr = create_Custom_List();
        else if (type == OPTIONS_LIST)
            *ptr = create_Options_List();
        else if (type == CONNECTION_LIST)
            *ptr = create_Connect_List();
        else if (type == MULTIPLAYER_LIST)
            *ptr = create_Multiplayer_List();
        else if (type == HOSTING_LIST)
            *ptr = create_Hosting_List();
        else if (type == PAUSE_LIST)
            *ptr = create_Pause_list();

        if (!inVector((void**)ptr))
            pushAddress((void**)ptr, UI_LIST);

    }

}

//Function for freeing memory of all variables declared during the current game_state
void freeVars()
{
    
    //By passing NULL to pushAddress(), we get the address of the varVector
    varVector** vector = pushAddress(NULL, TYPE_NA);

    for (int i = 0; i < (*vector)->count; i++)
    {

        //Check if memory is actually allocated at *ptr[i]
        if (*(void**)((*vector)->ptrs[i]) != NULL)
        {

            //If it is, then free it
                //We call different freeing methods depending on the type of variable or
                //object
            if ((*vector)->types[i] == VARIABLE)
                SDL_free(*(void**)((*vector)->ptrs[i]));
            else if ((*vector)->types[i] == PIECE)
                delPiece((piece**)&*(void**)((*vector)->ptrs[i]));
            else if ((*vector)->types[i] == TEXTURE)
                SDL_DestroyTexture(*(SDL_Texture**)((*vector)->ptrs[i]));
            else if ((*vector)->types[i] == MOVING_TITLE_PIECES)
            {

                for (unsigned short j = 0; j < NUM_MOVING_TITLE_PIECES; j++)
                    delPiece(&((piece**)*(void**)(*vector)->ptrs[i])[j]);
                SDL_free((piece**)*(void**)(*vector)->ptrs[i]);

            }
            else if ((*vector)->types[i] == UI_LIST)
                delete_UI_list((UI_list**)&*(void**)((*vector)->ptrs[i]));
            else if ((*vector)->types[i] == SIZE_BAG)
            {

                SDL_free(((SizeBag*)*(void**)(*vector)->ptrs[i])->sizesInBag);
                SDL_free(((SizeBag*)*(void**)(*vector)->ptrs[i]));

            } 

        }

        //Reset the ptr to NULL
        *(void**)((*vector)->ptrs[i]) = NULL;

    }   

    //Free ptrs and types arrays
    SDL_free((*vector)->ptrs);
    SDL_free((*vector)->types);

    //Free the memory taken by the varVector
    SDL_free(*vector);
    *vector = NULL;

}

// Function for extracting all of the values from a string of bytes separated by a delimiter character
    // The function returns a dynamically allocated 2D array that should be freed later with SDL_free().
    // Ensure to free each element as well as the array itself.
char** extractStringsFromDelimitedBytes(char* data, int dataLength, int* numValues, char delim)
{

    // values is an array of strings of bytes
    char** values = NULL;
    *numValues = 0;
    char* currentStringValue = NULL;

    // Go through the entire string of bytes
    for (int dataIndex = 0; dataIndex < dataLength; dataIndex++)
    {

        // If the current byte is not a delimter.
        if (data[dataIndex] != delim)
        {

            // Then we are currently reading a value. So build the current value by appending the bytes 1-by-1 to
            // currentStringValue
            if (currentStringValue == NULL)
            {

                currentStringValue = SDL_calloc(2, sizeof(char));
                currentStringValue[0] = data[dataIndex];
                currentStringValue[1] = '\0';

            }
            else
            {

                int newLen  = SDL_strlen(currentStringValue) + 1 + 1;
                currentStringValue = SDL_realloc(currentStringValue, newLen * sizeof(char));
                SDL_strlcat(currentStringValue, &(data[dataIndex]), newLen);

            }

        }   // If the current byte is a delimiter, we have finished reading the current value.
        else if (currentStringValue != NULL)    // Skip if current value is of length zero
        {

            // So append the current value to the list of values
            if (*numValues == 0)
            {

                values = SDL_calloc(1, sizeof(char*));
                *numValues = *numValues + 1;

            }
            else
            {

                values = SDL_realloc(values, (*numValues + 1) * sizeof(char*));
                *numValues = *numValues + 1;

            }

            values[*numValues - 1] = SDL_calloc(SDL_strlen(currentStringValue) + 1, sizeof(char));
            SDL_strlcpy(values[*numValues - 1], currentStringValue, SDL_strlen(currentStringValue) + 1);

            // Free the currentStringValue to avoid memory leaks
            SDL_free(currentStringValue);
            currentStringValue = NULL;

        }

    }

    // Ensure to free currentStringValue before leaving the function.
    if (currentStringValue != NULL)
    {

        SDL_free(currentStringValue);
        currentStringValue = NULL;

    }

    return values;

}