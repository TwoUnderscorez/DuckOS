#include "../../lib/libduck.h"
int main(int argc, char **argv);

int main(int argc, char **argv)
{
    puts("Is the serial working?\n");
    serial_puts("Hello from userland!\n");
}