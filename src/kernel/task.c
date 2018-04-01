#include "task.h"
#include "memory.h"
#include "isr.h"
#include "descriptors.h"
#include "../libs/string.h"
#include "../drivers/screen.h"
#include "../asm/asmio.h"

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

static void setup_testuserapp(registers_t * regs) {
    unsigned int t_pdpt = (unsigned int)create_pdpt();
    // Map user space (0x300000-0x301000)
    page_table_entry_t * data = malloc(sizeof(page_table_entry_t));
    memset((void *)data, '\0', sizeof(page_table_entry_t));
    data->present = 1;
    // data->physical_page_address = kalloc_frame()>>12;
    data->ro_rw = 1;
    data->kernel_user = 1;
    map_vaddr_to_pdpt(t_pdpt, data, 0x300000, 0x300001);
    disablePagingAsm();
    asmcli();
    memcpy((void *)(data->physical_page_address<<12), &usermain, 0x1000);
    enablePagingAsm();
    asmsti();
    /* Temporary loading of the user task to the appropriate location
     * in memory. will be removed. 
     */
    create_task(&otherTask, (void *)0x300000, mainTask.regs.eflags, t_pdpt,
                0x300100, regs->kesp);
    otherTask.next = &mainTask;
    runningTask = &mainTask;
}
 
void init_tasking(registers_t * regs) {
    // Create a task for the kernel
    memcpy(&mainTask.regs, regs, (int)sizeof(registers_t)); // Save kernel task regs
    mainTask.next = &otherTask;
    set_kernel_stack((unsigned int)regs->kesp);
    // Create the other task
    // setup_testuserapp(regs);
}
 
void create_task(task_t *task, void (*main)(), unsigned int flags, unsigned int pagedir,
                 unsigned int user_esp, unsigned int isr_esp) {
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
    task->regs.useresp = (unsigned int) user_esp; // Proccess' stack
    task->regs.kesp = (unsigned int)mainTask.regs.kesp; // ISR's stack
    task->next = 0;
}

void add_task(task_t * task) {
    runningTask->next = task;
}
 
void roundRobinNext(registers_t * regs) {
    task_t *last = runningTask;
    runningTask = runningTask->next;
    // Save last's state
    memcpy(&last->regs, regs, (int)sizeof(registers_t));
    // Load runningTask's state
    memcpy(regs, &runningTask->regs, (int)sizeof(registers_t));
    // Set ISR stack
    set_kernel_stack((unsigned int)runningTask->regs.kesp);
}