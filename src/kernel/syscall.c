#include "syscall.h"
#include "task.h"
#include "isr.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"

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
		case 0x82: // Tasking                           USERLAND
            switch(regs->eax) {
                case 0x01: // Exit
                    remove_task(regs);
                    break;
                case 0x02: // Yield
                    roundRobinNext(regs);
                    break;
                case 0x03: // Get more heap
                    roundRobinNext(regs);
                    break;
                default:
                    return;
            }
            break;
        case 0x83: // Screen                            USERLAND
            switch(regs->eax) {
                case 0x01: // Put char
                    putc((char)regs->ebx);
                    break;
                case 0x02: // Put string
                    puts((char *)regs->ebx);
                    break;
                case 0x03: // Clear screen
                    screen_clear();
                    break;
                default:
                    return;   
            }
            break;
        default:
            return;
    }
}