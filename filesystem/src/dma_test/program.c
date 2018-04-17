#include "../../lib/libduck.h"
void main(int argc, char ** argv);

void main(int argc, char ** argv) {
    int * ptr = malloc(sizeof(int));
    *ptr = 35;
    puts("OK\n");
    ptr = malloc(0x1500);
    *ptr = 35;
    puts("OK\n");
}