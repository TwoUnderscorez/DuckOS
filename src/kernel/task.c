#include "task.h"
#include "memory.h"
#include "isr.h"
#include "descriptors.h"
#include "heap.h"
#include "elf.h"
#include "../drivers/ext2.h"
#include "../libs/string.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../asm/asmio.h"

// A pointer to the running task in the linked list of tasks
static task_t *runningTask;
// Represents the kernel task
static task_t mainTask;
 
void init_tasking(registers_t * regs) {
    // Create a task for the kernel
    regs->useresp = regs->kesp;
    regs->ss = regs->ds;
    memcpy(&mainTask.regs, regs, (int)sizeof(registers_t)); // Save kernel task regs
    mainTask.next = &mainTask;
    set_kernel_stack((unsigned int)regs->kesp);
    // For now the the kernel is the main task (later will be set to terminal)
    runningTask = &mainTask;
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
    task->regs.kesp = (unsigned int)isr_esp; // ISR's stack (Ignored by roundRobinNext, see note in asmisr.asm)
    task->next = 0;
}

void add_task(task_t * task) {
    task->next = runningTask->next;
    runningTask->next = task;
}

void free_task_frames(page_directory_pointer_table_entry_t * pdpt) {
    page_directory_table_entry_t * pdir;
    page_table_entry_t * ptab;
    int i, j, k;
    disablePagingAsm();
    for(i = 0; i<4; i++) {
        if(pdpt[i].present) {
            pdir = (page_directory_table_entry_t *)(pdpt[i].page_directory_table_address<<12);
            for(j = 0; j < 512; j++) {
                if(pdir[j].present && !pdir[j].size) {
                    ptab = (page_table_entry_t *)(pdir[j].page_table_address<<12);
                    for(k = 0; k < 512; k++) {
                        if(ptab[k].present) {
                            kfree_frame(ptab[k].physical_page_address<<12);
                        }
                    }
                    kfree_frame(pdir[j].page_table_address<<12);
                }
            }
            kfree_frame(pdpt[i].page_directory_table_address<<12);
        }
    }
    kfree_frame((unsigned int)pdpt);
    enablePagingAsm();
}

void remove_task(registers_t * regs) {
    task_t * prev = runningTask;
    while(prev->next!=runningTask) prev = prev->next;
    prev->next = runningTask->next;
    free_task_frames((page_directory_pointer_table_entry_t *)regs->cr3);
    roundRobinNext(regs);
}
 
void roundRobinNext(registers_t * regs) {
    task_t *last = runningTask;
    runningTask = runningTask->next;
    // Save last's state
    memcpy(&last->regs, regs, (int)sizeof(registers_t));
    // Load runningTask's state
    memcpy(regs, &runningTask->regs, (int)sizeof(registers_t));
    // Set ISR stack
    // set_kernel_stack((unsigned int)runningTask->regs.kesp);
}

void dump_regs(registers_t * regs) {
    puts("usresp: ");
    screen_print_int(regs->useresp, 16);
    puts(" isresp: ");
    screen_print_int(regs->kesp, 16);
    puts(" cr3: ");
    screen_print_int(regs->cr3, 16);
    puts(" cs: ");
    screen_print_int(regs->cs, 16);
    puts(" ss: ");
    screen_print_int(regs->ss, 16);
    puts(" ds: ");
    screen_print_int(regs->ds, 16);
    puts(" eax: ");
    screen_print_int(regs->eax, 16);
    puts(" ebp: ");
    screen_print_int(regs->ebp, 16);
    puts(" ebx: ");
    screen_print_int(regs->ebx, 16);
    puts(" ecx: ");
    screen_print_int(regs->ecx, 16);
    puts(" edi: ");
    screen_print_int(regs->edi, 16);
    puts(" edx: ");
    screen_print_int(regs->edx, 16);
    puts(" eflags: ");
    screen_print_int(regs->eflags, 16);
    puts(" eip: ");
    screen_print_int(regs->eip, 16);
    puts(" err: ");
    screen_print_int(regs->err_code, 16);
    puts(" esi: ");
    screen_print_int(regs->esi, 16);
    puts(" int: ");
    screen_print_int(regs->int_no, 16);
    puts(" ss: ");
    screen_print_int(regs->ss, 16);
    puts("\n");
}

void execve(char * path, int argc, char ** argv) {
    int inode_num = path_to_inode(path);
    EXT2_INODE_t * inode = malloc(sizeof(EXT2_INODE_t));
    load_inode(inode_num, inode);
    char * file = malloc((1 + inode->sector_usage) * 512);
    load_file(inode_num, 0, 0, file);
    if(elf_check_supported((Elf32_Ehdr_t *)file)) {
		elf_load_file(file, argc, argv);
	}
    free((void *)inode);
    free((void *)file);
}

void set_next_task_forever() {
    runningTask->next->next = runningTask->next;
}