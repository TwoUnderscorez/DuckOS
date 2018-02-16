#include "math.h"

int cell(double x) {
    if ((int)x - x == 0) {
        return (int)x;
    }
    else {
        return (int)x + 1;
    }
}

int floor(double x) {
    return (int)x;
}

int round(double x) {
    return (int)(x+.5);
}