#ifndef RPG_MACROS
#define RPG_MACROS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


// Define a macro for memory allocation repeated code
#define MALLOC(p, s, e, m) \
    TRY{p = malloc(s); if (p == NULL) {THROW(e);}} \
    CATCH_HANDLE(e, m) ETRY;

// Bool is defined as an enum to be used as a type returned by functions
typedef enum Bool Bool;
enum Bool
{
    FALSE = 0, // ensure FALSE is 0
    TRUE = 1
};

/* Returns FALSE for 0, TRUE for else. This function ensures that any number is
 * converted to TRUE (1), instead of the number. */
Bool convert_to_bool(int const number);

// Returns TRUE if string is empty
Bool is_empty_string(char const string[]);

/* Define how many numbers after floating points are considered to compare
 * doubles. */
#define DOUBLE_PRECISION 2

/* Returns TRUE if double a == double b up to 10^-DOUBLE_PRECISION
 * This function is needed to circumvent floating numbers comparison issue.
 * see here for an explanation:
 * https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/ */
Bool is_same_double(double const a, double const b);

// Convert degrees to radians
double degrees(double radians);

// Convert radians to degrees
double radians(double degrees);

#endif // define RPG_MACROS
