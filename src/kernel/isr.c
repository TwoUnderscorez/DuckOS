#include "isr.h"
#include "syscall.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../drivers/atapio.h"
#include "task.h"

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
		puts("Caught ");
		if(regs.cs == 0x08) puts("kmode ");
		puts("exception, ");
		switch(regs.int_no) {
			case 0x00:
				puts("Division or Modulo by Zero Error");
			break;
			case 0x01:
				puts("Debug");
			break;
			case 0x02:
				puts("Non-maskable Interrupt");
			break;
			case 0x03:
				puts("Breakpoint");
			break;
			case 0x04:
				puts("Overflow");
			break;
			case 0x05:
				puts("Bound Range Exceeded");
			break;
			case 0x06:
				puts("Invalid Opcode");
			break;
			case 0x07:
				puts("Device Not Available");
			break;
			case 0x0A:
				puts("Invalid TSS");
			break;
			case 0x0B:
				puts("Segment Not Present");
			break;
			case 0x0C:
				puts("Stack-Segment Fault");
			break;
			case 0x0D:
				puts("General Protection Fault");
			break;
			case 0x0E:
				puts("Page Fault");
			break;
			case 0x10:
				puts("x87 Floating-Point Exception");
			break;
			case 0x11:
				puts("Alignment Check");
			break;
			case 0x12:
				puts("Machine Check");
			break;
			case 0x13:
				puts("SIMD Floating-Point Exception");
			break;
			case 0x14:
				puts("Virtualization Exception");
			break;
			case 0x1E:
				puts("Security Exception");
			break;
			default:
				puts("0x");
				screen_print_int(regs.int_no, 16);			
		}
		puts(" with an error code of 0x");
		screen_print_int(regs.err_code, 16);
		puts("\n");
		if(regs.cs == 0x1B && regs.useresp > 0x60000) {
			remove_task(&regs);
			puts("Task terminated.\n");
		}
		else {
			puts("[KERNEL PANIC] System halted :(");
			getc();
		}
	}
}


void dump_regs(registers_t * regs) {
    puts("usresp: ");
    screen_print_int(regs->useresp, 16);
    puts(" isresp: ");
    screen_print_int(regs->kesp, 16);
    puts("\ncr3: ");
    screen_print_int(regs->cr3, 16);
    puts("\ncs: ");
    screen_print_int(regs->cs, 16);
    puts(" ss: ");
    screen_print_int(regs->ss, 16);
    puts(" ds: ");
    screen_print_int(regs->ds, 16);
    puts("\neax: ");
    screen_print_int(regs->ebp, 16);
    puts(" ebx: ");
    screen_print_int(regs->ebx, 16);
    puts(" ecx: ");
    screen_print_int(regs->edi, 16);
    puts(" edx: ");
    screen_print_int(regs->eax, 16);
    puts("\nebp: ");
    screen_print_int(regs->ecx, 16);
    puts(" edi: ");
    screen_print_int(regs->err_code, 16);
    puts(" esi: ");
    screen_print_int(regs->edx, 16);
    puts("\neflags: ");
    screen_print_int(regs->eflags, 16);
    puts(" eip: ");
    screen_print_int(regs->eip, 16);
    puts(" err: ");
    screen_print_int(regs->esi, 16);
    puts(" int: ");
    screen_print_int(regs->ss, 16);
    puts("\n");
}