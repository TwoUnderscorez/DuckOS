#include "syscall.h"
#include "task.h"
#include "isr.h"

void initialise_syscalls() {
    ;
}

void handle_syscall(registers_t * regs) {
    switch(regs->int_no) {
        case 0x81:
			init_tasking(regs);
			break;
		case 0x82:
			roundRobinNext(regs);
			break;
        default:
            return;
    }
}