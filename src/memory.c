#include "HEADERS/MGF.h"

//generate.c
void delPiece(piece** Piece);
void copyPiece(piece* piece1, piece* piece2);

//draw.c
SDL_Texture* createPieceTexture(piece Piece);

//layouc.c
SDL_Texture* create_Score_Text();
SDL_Texture* create_Level_Text();
SDL_Texture* create_Lines_Text();
SDL_Texture* create_Pause_Text();
SDL_Texture* create_Foreground_Text();
SDL_Texture* create_Title_Text();
piece** getMovingPieces(piece** titlePieces);
piece** makeTitlePieces();

//Function for pushing a variable or object onto an array of varVector structures 
//designed to make freeing variables at the end of a game_state easier
varVector** pushAddress(void** ptr, unsigned short type)
{

    static varVector* vector;
    if (vector == NULL) {
        
        vector = calloc(1, sizeof(*vector));
        vector->count = 0;

    }

    if(ptr == NULL)
        return &vector;
    else
    {

        if (vector->ptrs == NULL)
        {

            vector->ptrs = calloc(1, sizeof(void*));
            vector->types = calloc(1, sizeof(unsigned short));

        }
        else
        {

            void** tempPtrs = calloc(vector->count, sizeof(void*));
            unsigned short* tempTypes = calloc(vector->count, sizeof(unsigned short));

            for (int i = 0; i < vector->count; i++)
            {

                tempPtrs[i] = vector->ptrs[i];
                tempTypes[i] = vector->types[i];

            }

            free(vector->ptrs);
            free(vector->types);

            vector->ptrs = calloc(vector->count + 1, sizeof(void*));
            vector->types = calloc(vector->count + 1, sizeof(unsigned short));

            for(int i = 0; i < vector->count + 1; i++)
            {

                vector->ptrs[i] = tempPtrs[i];
                vector->types[i] = tempTypes[i];

            }

            free(tempPtrs);
            free(tempTypes);

        }
        
        vector->ptrs[vector->count] = ptr;
        vector->types[vector->count] = type;

        vector->count++;

        return NULL;

    }

}

//Function for checking if a ptr is already stored in the varVector
bool inVector(void** ptr)
{

    varVector** vector = pushAddress(NULL, TYPE_NA);

    for (unsigned int i = 0; i < (*vector)->count; i++)
        if (ptr == (*vector)->ptrs[i])
            return true;
    return false;

}

//Function for declaring an unsigned short on the varVector array
void declare_unsigned_short(void** ptr, unsigned short value)
{

    if (*ptr == NULL)
    {

        *ptr = calloc(1, sizeof(unsigned short));
        **(unsigned short**)(ptr) = value;

        if (!inVector(ptr))
            pushAddress(ptr, VARIABLE);

    }

}

//Function for declaring a double on the varVector array
void declare_double(void** ptr, double value)
{

    if (*ptr == NULL)
    {

        *ptr = calloc(1, sizeof(double));
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

        *ptr = calloc(1, sizeof(int));
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

        *ptr = calloc(1, sizeof(char));
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

        *ptr = calloc(1, sizeof(bool));
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

        *ptr = calloc(1, sizeof(unsigned int));
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

            *ptr = calloc(1, sizeof(piece));
            (*ptr)->blocks = calloc(Piece->numOfBlocks, sizeof(*(*ptr)->blocks));
            copyPiece(Piece, *ptr);

        }

        if (!inVector((void**)ptr))
            pushAddress((void**)ptr, PIECE);

    }

}

//Function for declaring a Piece Texture on the varVector array
void declare_Piece_Text(SDL_Texture** ptr, piece* Piece)
{

    if (*ptr == NULL)
    {

        if (Piece != NULL)
            *ptr = createPieceTexture(*Piece);

        if (!inVector((void**)ptr))
            pushAddress((void**)ptr, TEXTURE);

    }

}

//Function for declaring a HUD Texture on the varVector array
void declare_HUD_Text(SDL_Texture** ptr, int type)
{

    if (*ptr == NULL)
    {

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

        if(!inVector((void**)ptr))
            pushAddress((void**)ptr, TEXTURE);

    }

}

//Declare the map data matrix
void declare_map_matrix(bool** ptr)
{

    if (*ptr == NULL)
    {

        *ptr = (bool*)(malloc(MAP_HEIGHT * MAP_WIDTH * sizeof(**ptr)));
        if (*ptr != NULL)
            for (unsigned short i = 0; i < MAP_HEIGHT; i++)
                for (unsigned short j = 0; j < MAP_WIDTH; j++)
                    *(*ptr + i * MAP_WIDTH + j) = false;

        if (!inVector((void**)ptr))
            pushAddress((void**)ptr, VARIABLE);

    }

}

void declare_moving_title_pieces(piece*** ptr)
{

    if (*ptr == NULL)
    {

        *ptr = getMovingPieces(makeTitlePieces());

        if (!inVector((void**)ptr))
            pushAddress((void**)ptr, MOVING_TITLE_PIECES);

    }

}

//Function for freeing memory of all variables declared during the current game_state
void freeVars()
{
    
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
                free(*(void**)((*vector)->ptrs[i]));
            else if ((*vector)->types[i] == PIECE)
                delPiece((piece**)&*(void**)((*vector)->ptrs[i]));
            else if ((*vector)->types[i] == TEXTURE)
                SDL_DestroyTexture(*(SDL_Texture**)((*vector)->ptrs[i]));
            else if ((*vector)->types[i] == MOVING_TITLE_PIECES)
            {

                for (unsigned short j = 0; j < NUM_MOVING_TITLE_PIECES; j++)
                    delPiece(&((piece**)*(void**)(*vector)->ptrs[i])[j]);
                free((piece**)*(void**)(*vector)->ptrs[i]);

            }
            
        }

        //Reset the ptr to NULL
        *(void**)((*vector)->ptrs[i]) = NULL;

    }   

    //Free the memory taken by the varVector
    free(*vector);
    *vector = NULL;

}