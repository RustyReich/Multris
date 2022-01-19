#include "HEADERS/MGF.h"

//generate.c
void delPiece(piece** Piece);
void copyPiece(piece* piece1, piece* piece2);

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

//Function for freeing memory of all variables declared during the current game_state
void freeVars(varVector** vector)
{

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
            
        }

        //Reset the ptr to NULL
        *(void**)((*vector)->ptrs[i]) = NULL;

    }   

    //Free the memory taken by the varVector
    free(*vector);
    *vector = NULL;

}