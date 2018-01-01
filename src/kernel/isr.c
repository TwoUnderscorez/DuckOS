#include "isr.h"
#include "../drivers/screen.h"

void isr_handler(registers_t regs)
{
	puts("Recieved interrupt: ");
	screen_print_int(regs.int_no,16);
	puts("\n");
}
