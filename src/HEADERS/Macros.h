#define declareEnd(val) _Generic((val), \
                            int: declare_int, \
                            char: declare_char, \
                            double: declare_double, \
                            unsigned short: declare_unsigned_short, \
                            bool: declare_bool, \
                            unsigned int: declare_unsigned_int \
                            )
//Declare a variable on the varVector array
#define declare(var, val) _Generic((var) , \
                            int *: declareEnd(val), \
                            char *: declareEnd(val), \
                            double *: declareEnd(val), \
                            unsigned short *: declareEnd(val), \
                            bool *: declareEnd(val), \
                            unsigned int *: declareEnd(val) \
                            )(((void**)&var), (val))

//Macros for controls
#define onPress(var)    globalInstance->controls[var].onPress
#define onHold(var)     globalInstance->controls[var].onHold     

//Macros for variadic functions
#define create_list(...) _create_list(__VA_ARGS__, NULL)