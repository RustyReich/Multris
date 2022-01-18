#include "HEADERS/MGF.h"

//Function for pushing a variable into a vector of varVector structures designed to make
//freeing variables at the end of a game_state easier
varVector** pushAddress_VAR(void* addr, void** ptr)
{

    static varVector* vector;
    if (vector == NULL) {
        
        vector = calloc(1, sizeof(*vector));
        vector->count = 0;

    }

    if(addr == NULL || ptr == NULL)
        return &vector;
    else
    {
        
        if (vector->adrs == NULL || vector->ptrs == NULL)
        {

            vector->adrs = calloc(1, sizeof(void*));
            vector->ptrs = calloc(1, sizeof(void*));

        }
        else
        {

            void** tempAdrs = calloc(vector->count, sizeof(void*));
            void** tempPtrs = calloc(vector->count, sizeof(void*));

            for (int i = 0; i < vector->count; i++)
            {

                tempAdrs[i] = vector->adrs[i];
                tempPtrs[i] = vector->ptrs[i];

            }

            free(vector->adrs);
            free(vector->ptrs);

            vector->adrs = calloc(vector->count + 1, sizeof(void*));
            vector->ptrs = calloc(vector->count + 1, sizeof(void*));

            for(int i = 0; i < vector->count + 1; i++)
            {

                vector->adrs[i] = tempAdrs[i];
                vector->ptrs[i] = tempPtrs[i];

            }

            free(tempAdrs);
            free(tempPtrs);

        }
        
        vector->adrs[vector->count] = addr;
        vector->ptrs[vector->count] = ptr;

        vector->count++;

        return NULL;

    }

}

//Function for declaring an unsigned short on the varVector array
void declare_unsigned_short(void** ptr, unsigned short value)
{

    if (*ptr == NULL)
    {

        *ptr = calloc(1, sizeof(unsigned short));
        **(unsigned short**)(ptr) = value;
        pushAddress_VAR(*ptr, ptr);

    }

}

//Function for declaring a double on the varVector array
void declare_double(void** ptr, double value)
{

    if (*ptr == NULL)
    {

        *ptr = calloc(1, sizeof(double));
        **(double**)(ptr) = value;
        pushAddress_VAR(*ptr, ptr);

    }

}

//Function for declaring an int on the varVector array
void declare_int(void** ptr, int value)
{

    if (*ptr == NULL)
    {

        *ptr = calloc(1, sizeof(int));
        **(int**)(ptr) = value;
        pushAddress_VAR(*ptr, ptr);

    }

}

//Function for declaring a char on the varVector array
void declare_char(void** ptr, char value)
{

    if (*ptr == NULL)
    {

        *ptr = calloc(1, sizeof(char));
        **(char**)(ptr) = value;
        pushAddress_VAR(*ptr, ptr);

    }

}

//Function for declaring a bool on the varVector array
void declare_bool(void** ptr, bool value)
{

    if (*ptr == NULL)
    {

        *ptr = calloc(1, sizeof(bool));
        **(bool**)(ptr) = value;
        pushAddress_VAR(*ptr, ptr);

    }

}

//Function for declaring an unsigned int on the varVector array
void declare_unsigned_int(void** ptr, unsigned int value)
{

    if (*ptr == NULL)
    {

        *ptr = calloc(1, sizeof(unsigned int));
        **(unsigned int**)(ptr) = value;
        pushAddress_VAR(*ptr, ptr);

    }

}

//Function for freeing memory of all variables declared during the current game_state
void freeVars(varVector** vector)
{

    for (int i = 0; i < (*vector)->count; i++)
    {

        free((*vector)->adrs[i]);
        (*vector)->adrs[i] = NULL;
        *(void**)((*vector)->ptrs[i]) = NULL;

    }   

    free(*vector);
    *vector = NULL;

}