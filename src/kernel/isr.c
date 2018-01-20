#include "isr.h"
#include "task.h"
#include "../drivers/screen.h"

void isr_handler(registers_t regs)
{
	switch (regs.int_no) {
		case 0x08:
			puts("int 0x08. double fault. system halted.");
			while(1);
		case 0x81:
			initTasking(&regs);
			break;
		case 0x82:
			roundRobinNext(&regs);
			break;
		default:
			puts("Recieved interrupt: ");
			screen_print_int(regs.int_no, 16);
			puts(" ");
			screen_print_int(regs.err_code, 16);
			puts("\n");
			getc();
	}
}
