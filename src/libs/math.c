#include "math.h"

/**
 * @brief math celling operation
 * 
 * @param x 
 * @return int 
 */
int cell(double x)
{
    if ((int)x - x == 0)
    {
        return (int)x;
    }
    else
    {
        return (int)x + 1;
    }
}

/**
 * @brief math floor operation
 * 
 * @param x 
 * @return int 
 */
int floor(double x)
{
    return (int)x;
}

/**
 * @brief math round operation
 * 
 * @param x 
 * @return int 
 */
int round(double x)
{
    return (int)(x + .5);
}

int inline min(int a, int b)
{
    return (a > b) ? b : a;
}

int inline max(int a, int b)
{
    return (a > b) ? a : b;
}