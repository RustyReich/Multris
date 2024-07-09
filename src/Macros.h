#ifndef MACROS_H_
#define MACROS_H_

#include "MGF.h"

//Declare a variable on the varVector array
#define declareEnd(val) _Generic((val),                             \
                            int: declare_int,                       \
                            char: declare_char,                     \
                            double: declare_double,                 \
                            unsigned short: declare_unsigned_short, \
                            bool: declare_bool,                     \
                            unsigned int: declare_unsigned_int,     \
                            short: declare_short,                   \
                            unsigned long: declare_unsigned_long    \
                            )
#define declareStart(var, val) _Generic((var) ,                 \
                            int *: declareEnd(val),             \
                            char *: declareEnd(val),            \
                            double *: declareEnd(val),          \
                            unsigned short *: declareEnd(val),  \
                            bool *: declareEnd(val),            \
                            unsigned int *: declareEnd(val),    \
                            short*: declareEnd(val),            \
                            unsigned long*: declareEnd(val)      \
                            )(((void**)&var), (val))
#define DECLARE_VARIABLE(type, varName, value) static type* varName; declareStart(varName, value)

//Macros for instance
#define onPress(var)    globalInstance->controls[var].onPress
#define onHold(var)     globalInstance->controls[var].onHold    
#define playSound(var)  _playSound(var)

//Macros for variadic functions
#define create_list(...) _create_list(__VA_ARGS__, NULL)

//In-line function macros
#define swap(a, b, t) { t tmp = a; a = b; b = tmp; } 

#endif