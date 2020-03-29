#include "macros.h"


Bool is_empty_string(char const string[])
{
    return !strcmp(string, "");
}

Bool is_same_double(double const a, double const b)
{
    double epsilon = 1/pow(10, DOUBLE_PRECISION);
    if (fabs(a - b) < epsilon)
        return TRUE;
    return FALSE;
}

Bool convert_to_bool(int const number)
{
    return number == 0 ? FALSE : TRUE;
}

double degrees(double const radians)
{
    return radians*180/M_PI;
}

double radians(double const degrees)
{
    return degrees*M_PI/180;
}
