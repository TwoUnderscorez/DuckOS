#include "isr.h"
#include "screen.h"

void isr_handler(registers_t regs)
{
	screen_print("Recieved interrupt: ");
	screen_print_int(regs.int_no,16);
	screen_print("\n");
}
