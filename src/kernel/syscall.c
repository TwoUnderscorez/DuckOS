#include "syscall.h"
#include "task.h"
#include "isr.h"
#include "../drivers/screen.h"

void initialise_syscalls() {
    ;
}

void handle_syscall(registers_t * regs) {
    switch(regs->int_no) {
        case 0x80: // Interrupt test                    KMODE
            puts("Recieved interrupt: 0x80. Interrupts seem to be functioning properly.\n");
            break;
        case 0x81: // Init tasking                      KMODE
			init_tasking(regs);
			break;
		case 0x82: // Context Switch                    USERLAND
			roundRobinNext(regs); 
			break;
        case 0x83: // puts                              USERLAND
            puts((char *)regs->eax);
            break;
        default:
            return;
    }
}