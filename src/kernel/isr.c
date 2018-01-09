#include "isr.h"
#include "../drivers/screen.h"

void isr_handler(registers_t regs)
{
	puts("Recieved interrupt: ");
	if(regs.int_no == 0x08){
		puts("0x08. double fault. system halted.");
		while(1);
	}
	screen_print_int(regs.int_no,16);
	puts("\n");
}
