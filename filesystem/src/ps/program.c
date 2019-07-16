#include "../../lib/libduck.h"
int main(void);

int main(void)
{
    puts("Tasks linked list:\n");
    __asm__("int $0x82" ::"a"(0x05));
}