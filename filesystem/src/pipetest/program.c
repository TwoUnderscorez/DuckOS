#include "../../lib/libduck.h"
void main(int argc, char ** argv);

void main(int argc, char ** argv) {
    puts("My argv:\n");
    for (int i = 0; i < argc; i ++){
        puts(argv[i]);
        puts("\n");
    }
}