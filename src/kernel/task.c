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
#include "../libs/log.h"

// A pointer to the running task in the linked list of tasks
static task_t *runningTask;
// Represents the kernel task
static task_t mainTask;
unsigned int pid_counter = 0;

/**
 * @brief Initialize tasking capabilites
 * 
 * @param regs 
 */
void tasking_init(registers_t *regs)
{
    // Create a task for the kernel
    regs->useresp = regs->kesp;
    regs->ss = regs->ds;
    memcpy(&mainTask.regs, regs, (int)sizeof(registers_t)); // Save kernel task regs
    mainTask.next = &mainTask;
    set_kernel_stack((unsigned int)regs->kesp);
    // For now the the kernel is the main task (later will be set to terminal)
    runningTask = &mainTask;
}

/**
 * @brief Create a task
 * 
 * @param task ptr to task_t data struct
 * @param main the task's entry point
 * @param flags the eflags register
 * @param pagedir physical address of the task's paging directory pointer table
 * @param user_esp task's userland stack
 * @param isr_esp task's kmode stack
 */
void task_create(
    task_t *task,
    void (*main)(),
    unsigned int flags,
    unsigned int pagedir,
    unsigned int user_esp,
    unsigned int isr_esp)
{
    task->regs.eax = 0;
    task->regs.ebx = 0;
    task->regs.ecx = 0;
    task->regs.edx = 0;
    task->regs.esi = 0;
    task->regs.edi = 0;
    task->regs.cs = 0x18 | 0x3; // Userland GDT selectors
    task->regs.ss = 0x20 | 0x3; // All tasks will run in userland
    task->regs.ds = 0x20 | 0x3; // ORed with CPL 3 (userland)
    task->regs.eflags = flags;
    task->regs.eip = (unsigned int)main;
    task->regs.cr3 = (unsigned int)pagedir;
    task->regs.useresp = (unsigned int)user_esp; // Proccess' stack
    task->regs.kesp = (unsigned int)isr_esp;     // ISR's stack (Ignored by roundRobinNext, see note in asmisr.asm)
    task->next = 0;
    task->pid = pid_counter++;
}

/**
 * @brief Add a task to the linked list of running task 
 * 
 * @param task 
 */
void task_add(task_t *task)
{
    task->next = runningTask->next;
    runningTask->next = task;
}

/**
 * @brief Free a task's page frame
 * 
 * @param pdpt 
 */
void task_free_frames(page_directory_pointer_table_entry_t *pdpt)
{
    page_directory_table_entry_t *pdir;
    page_table_entry_t *ptab;
    int i, j, k;
    disablePagingAsm();
    for (i = 0; i < 4; i++)
    {
        if (pdpt[i].present)
        {
            pdir = (page_directory_table_entry_t *)(pdpt[i].page_directory_table_address << 12);
            for (j = 0; j < 512; j++)
            {
                if (pdir[j].present && !pdir[j].size)
                {
                    ptab = (page_table_entry_t *)(pdir[j].page_table_address << 12);
                    for (k = 0; k < 512; k++)
                    {
                        if (ptab[k].present)
                        {
                            kfree_frame(ptab[k].physical_page_address << 12);
                        }
                    }
                    kfree_frame(pdir[j].page_table_address << 12);
                }
            }
            kfree_frame(pdpt[i].page_directory_table_address << 12);
        }
    }
    kfree_frame((unsigned int)pdpt);
    enablePagingAsm();
}

/**
 * @brief Remove a task from the linked list of running tasks
 * 
 * @note calls task_free_frames
 * @param regs 
 */
void task_remove(registers_t *regs)
{
    task_t *prev = 0, *task_to_remove = 0;
    if (runningTask->next == runningTask)
    {
        runningTask->next = &mainTask;
        mainTask.next = &mainTask;
    }
    else
    {
        task_to_remove = prev = runningTask;
        while (prev->next != runningTask)
            prev = prev->next;
        prev->next = runningTask->next;
    }
    task_free_frames((page_directory_pointer_table_entry_t *)regs->cr3);
    task_roundRobinNext(regs);
    if (task_to_remove)
    {
        free(task_to_remove);
        task_to_remove = 0;
    }
}

/**
 * @brief Round robin next
 * 
 * @param regs 
 */
void task_roundRobinNext(registers_t *regs)
{
    task_t *last = runningTask;
    runningTask = runningTask->next;
    // Save last's state
    memcpy(&last->regs, regs, (int)sizeof(registers_t));
    // Load runningTask's state
    memcpy(regs, &runningTask->regs, (int)sizeof(registers_t));
    // Set ISR stack
    set_kernel_stack(mainTask.regs.kesp);
}

/**
 * @brief posix fork+execve, like CreateProcess in win32.
 * 
 * @param path Command line
 * @param argc Argument count
 * @param argv Argument values
 */
void execve(char *path, int argc, char **argv)
{
    int inode_num = 0;
    void *file = 0;
    EXT2_INODE_t *inode = 0;

    inode_num = ext2_path_to_inode(path);

    inode = malloc(sizeof(EXT2_INODE_t));
    if (!inode)
        goto _cleanup_inode;

    ext2_load_inode(inode_num, inode);

    file = malloc(inode->size_low);
    if (!file)
        goto _cleanup_file;

    ext2_load_file(inode, 0, 0, file);

    if (elf_check_supported((Elf32_Ehdr_t *)file))
    {
        elf_load_file(file, argc, argv);
    }

_cleanup_file:
    if (file)
    {
        free(file);
        file = 0;
    }
_cleanup_inode:
    if (inode)
    {
        free(inode);
        inode = 0;
    }
}

void task_set_next_task_forever()
{
    runningTask->next->next = runningTask->next;
}

/**
 * @brief Dump a running tasks list
 * 
 */
void task_print_task_linked_list()
{
    task_t *task_ptr = runningTask;
    do
    {
        puts((char *)&task_ptr->name);
        putc('[');
        screen_print_int(task_ptr->pid, 10);
        puts("] -> ");
        task_ptr = task_ptr->next;
    } while (task_ptr != runningTask);
    puts("\b\b\b");
    task_ptr = runningTask;
    do
    {
        putc('\n');
        puts((char *)&task_ptr->name);
        putc('[');
        screen_print_int(task_ptr->pid, 10);
        puts("]:\n");
        task_disp_heap(task_ptr->pid);
        getc();
        task_ptr = task_ptr->next;
    } while (task_ptr != runningTask);
}

/**
 * @brief Print the page frames in use by running tasks
 * 
 */
void task_dump_all_task_memory_usage()
{
    puts("Loaded tasks: \n");
    task_t *ptr = runningTask;
    page_directory_pointer_table_entry_t *pdpt;
    page_directory_table_entry_t *pdir;
    page_table_entry_t *ptab;
    int i, j, k;
    disablePagingAsm();
    do
    {
        pdpt = (page_directory_pointer_table_entry_t *)ptr->regs.cr3;
        puts((char *)&ptr->name);
        putc('[');
        screen_print_int(ptr->pid, 10);
        puts("]: ");
        for (i = 0; i < 4; i++)
        {
            if (pdpt[i].present)
            {
                pdir = (page_directory_table_entry_t *)(pdpt[i].page_directory_table_address << 12);
                for (j = 0; j < 512; j++)
                {
                    if (pdir[j].present && !pdir[j].size)
                    {
                        ptab = (page_table_entry_t *)(pdir[j].page_table_address << 12);
                        for (k = 0; k < 512; k++)
                        {
                            if (ptab[k].present)
                            {
                                screen_print_int((ptab[k].physical_page_address << 12) / 0x1000, 10);
                                putc(',');
                            }
                        }
                        screen_print_int((pdir[j].page_table_address << 12) / 0x1000, 10);
                        putc(',');
                    }
                }
                screen_print_int((pdpt[i].page_directory_table_address << 12) / 0x1000, 10);
                putc(',');
            }
        }
        screen_print_int(((unsigned int)pdpt) / 0x1000, 10);
        putc('\n');
        ptr = ptr->next;
    } while (ptr != runningTask);
    enablePagingAsm();
}

/**
 * @brief Dump a task's heap
 * 
 * @param pid 
 */
void task_disp_heap(unsigned int pid)
{
    task_t *ptr = runningTask;
    memory_block_header_t *heap_ptr = (memory_block_header_t *)0x700000;
    unsigned int pdpt_bk, i;
    do
    {
        ptr = ptr->next;
    } while ((ptr->pid != pid) && (ptr != runningTask));
    pdpt_bk = swapPageDirectoryAsm((unsigned int *)ptr->regs.cr3);
    if (!heap_ptr->used)
        return;
    unsigned char c;
    do
    {
        screen_set_bgfg(0x70);
        puts("Length: ");
        screen_print_int(heap_ptr->length, 10);
        puts("B; used: ");
        if (heap_ptr->used)
            puts("yes");
        else
            puts("no");
        screen_set_bgfg(0x07);
        for (i = 0; i < heap_ptr->length; i++)
        {
            screen_set_bgfg(0x07);
            c = *((unsigned char *)((unsigned int)heap_ptr + sizeof(memory_block_header_t) + i));
            screen_print_int(c, 16);
            i++;
            screen_set_bgfg(0x0F);
            c = *((unsigned char *)((unsigned int)heap_ptr + sizeof(memory_block_header_t) + i));
            screen_print_int(c, 16);
        }
        heap_ptr = heap_ptr->next;
    } while (heap_ptr->length > 0);
    swapPageDirectoryAsm((unsigned int *)pdpt_bk);
    screen_set_bgfg(0x07);
}