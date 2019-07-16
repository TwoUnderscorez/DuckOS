#include "../../lib/libduck.h"
int main(void);

int main(void)
{
    puts("Physical memory usage data:\n");
    __asm__("int $0x82" ::"a"(0x06));
}