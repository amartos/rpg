#include "macros.h"


Bool is_empty_string(char const string[])
{
    return !strcmp(string, "");
}

Bool is_same_double(double const a, double const b)
{
    Bool same = FALSE;
    double epsilon = 1/pow(10, DOUBLE_PRECISION);
    if (fabs(a - b) < epsilon)
        same = TRUE;
    return same;
}

