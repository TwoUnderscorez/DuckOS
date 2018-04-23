#include "../../lib/libduck.h"
void main(int argc, char ** argv);

void main(int argc, char ** argv) {
    int * ptr = malloc(sizeof(int));
    *ptr = 35;
    puts("OK\n");
    getc();
    ptr = malloc(0x3000);
    *ptr = 35;
    puts("OK\n");
}