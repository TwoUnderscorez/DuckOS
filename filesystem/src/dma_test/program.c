#include "../../lib/libduck.h"
void main(int argc, char ** argv);

void main(int argc, char ** argv) {
    int * ptr = malloc(4);
    puts("mOK\n");
    *ptr = 35;
    puts("OK\n");
    getc();
    // ptr = malloc(0x1500);
    ptr = malloc(0x1000); // Initial heap start + 0x500
    *ptr = 35;
    puts("OK\n");
}