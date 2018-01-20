#include "task.h"
#include "memory.h"
#include "isr.h"
#include "../libs/string.h"
#include "../drivers/screen.h"

static task_t *runningTask;
static task_t mainTask;
static task_t otherTask;

 
void otherMain() {
    while(1); // temp
    puts("Hello from other task!\n");
    asm volatile("int $0x82");
}
 
void initTasking(registers_t * regs) {
    // Create a task for the kernel
    memcpy(&mainTask.regs, regs, (int)sizeof(registers_t)); // Save kernel task regs
    mainTask.next = &otherTask;
    // Create the other task
    createTask(&otherTask, otherMain, mainTask.regs.eflags, (unsigned int*)mainTask.regs.cr3);
    otherTask.next = &mainTask;
    runningTask = &mainTask;
}
 
void createTask(task_t *task, void (*main)(), unsigned int flags, unsigned int *pagedir) {
    task->regs.eax = 0;
    task->regs.ebx = 0;
    task->regs.ecx = 0;
    task->regs.edx = 0;
    task->regs.esi = 0;
    task->regs.edi = 0;
    task->regs.cs = 0x18 | 0x3;
    task->regs.ss = 0x20 | 0x3;
    task->regs.ds = 0x20 | 0x3;
    task->regs.eflags = flags;
    task->regs.eip = (unsigned int) main;
    task->regs.cr3 = (unsigned int) pagedir;
    task->regs.useresp = (unsigned int) 0x130000; // Proccess tack
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