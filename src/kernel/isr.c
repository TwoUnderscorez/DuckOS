#include "isr.h"
#include "syscall.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../drivers/atapio.h"

void isr_handler(registers_t regs)
{
	if(regs.int_no == 0x08) {
		puts("[KERNEL PANIC] int 0x08. Double fault. System halted. ):");
		while(1);
	}
	else if(regs.int_no == ATA_IRQ_LINE){
		ata_irq_handler();
	}
	else if(regs.int_no >= 0x70) {
		handle_syscall(&regs);
	}
	else {
		puts("Recieved interrupt: ");
		screen_print_int(regs.int_no, 16);
		puts(" ");
		screen_print_int(regs.err_code, 2);
		puts("\n");
		getc();
	}
}
