#include "task.h"
#include "memory.h"
#include "isr.h"
#include "descriptors.h"
#include "../libs/string.h"
#include "../drivers/screen.h"

// A pointer to the running task in the linked list of tasks
static task_t *runningTask;
// Represents the kernel task
static task_t mainTask;
static task_t otherTask; // temp

 
void other_main() {
    puts("Hello from userland!\n");
    puts("Trapping back to the kernel...\n");
    asm volatile("int $0x82");
}
 
void init_tasking(registers_t * regs) {
    // Create a task for the kernel
    memcpy(&mainTask.regs, regs, (int)sizeof(registers_t)); // Save kernel task regs
    mainTask.next = &otherTask;
    set_kernel_stack((unsigned int)regs->kesp);
    // Create the other task
    unsigned int t_pdpt = (unsigned int)create_pdpt();
    create_task(&otherTask, 0x300000, mainTask.regs.eflags, t_pdpt);
    otherTask.next = &mainTask;
    runningTask = &mainTask;
}
 
void create_task(task_t *task, void (*main)(), unsigned int flags, unsigned int pagedir) {
    task->regs.eax = 0;
    task->regs.ebx = 0;
    task->regs.ecx = 0;
    task->regs.edx = 0;
    task->regs.esi = 0;
    task->regs.edi = 0;
    task->regs.cs = 0x18 | 0x3; // Userland GDT selectors
    task->regs.ss = 0x20 | 0x3; // All tasks will run in userland
    task->regs.ds = 0x20 | 0x3;
    task->regs.eflags = flags;
    task->regs.eip = (unsigned int) main;
    task->regs.cr3 = (unsigned int) pagedir;
    task->regs.useresp = (unsigned int) 0x300100; // Proccess' stack
    task->regs.kesp = (unsigned int)mainTask.regs.kesp; // ISR's stack
    task->next = 0;
}
 
void roundRobinNext(registers_t * regs) {
    task_t *last = runningTask;
    runningTask = runningTask->next;
    // Save last's state
    memcpy(&last->regs, regs, (int)sizeof(registers_t));
    // Load runningTask's state
    memcpy(regs, &runningTask->regs, (int)sizeof(registers_t));
}