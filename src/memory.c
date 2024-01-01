#include "HEADERS/MGF.h"

//generate.c
void delPiece(piece** Piece);
void copyPiece(piece* piece1, piece* piece2);

//draw.c
SDL_Texture* createPieceTexture(piece Piece);

//layout.c
SDL_Texture* create_Score_Text();
SDL_Texture* create_Level_Text();
SDL_Texture* create_Lines_Text();
SDL_Texture* create_Pause_Text();
SDL_Texture* create_Foreground_Text();
SDL_Texture* create_Title_Text();
piece** getMovingPieces(piece** titlePieces);
piece** makeTitlePieces();
UI_list* create_Modes_List();
SDL_Texture* create_Cursor_Text();
void delete_UI_list(UI_list** list);
UI_list* create_Numerical_List();
UI_list* create_Custom_List();
UI_list* create_Options_List();
SDL_Texture* create_Values_Text();
SDL_Texture* create_volSlide_Text();
SDL_Texture* create_Controls_Text();
SDL_Texture* create_Custom_Text();

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

        if(!inVector((void**)ptr))
            pushAddress((void**)ptr, TEXTURE);

    }

}

//Declare the map data matrix
void declare_map_matrix(bool** ptr)
{

    if (*ptr == NULL)
    {

        *ptr = (bool*)(SDL_malloc(MAP_HEIGHT * MAP_WIDTH * sizeof(**ptr)));
        if (*ptr != NULL)
            for (unsigned short i = 0; i < MAP_HEIGHT; i++)
                for (unsigned short j = 0; j < MAP_WIDTH; j++)
                    *(*ptr + i * MAP_WIDTH + j) = false;

        if (!inVector((void**)ptr))
            pushAddress((void**)ptr, VARIABLE);

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