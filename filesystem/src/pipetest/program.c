#include "../../lib/libduck.h"
int main(int argc, char **argv);

int main(int argc, char **argv)
{
    // Prints everything in argv which demonstrates the pipe functionallity
    puts("My argv:\n");
    for (int i = 0; i < argc; i++)
    {
        puts(argv[i]);
        puts("\n");
    }
}