#include "../../lib/libduck.h"
void main(void);

void main(void) {
    puts("Tasks linked list:\n");
    __asm__("int $0x82" :: "a" (0x05));
}