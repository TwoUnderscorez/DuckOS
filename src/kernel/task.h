#ifndef TASK_H
#define TASK_H

#include "isr.h"
#include "memory.h"
void other_main(void);
void tasking_init(registers_t *regs);

struct task
{
    registers_t regs;
    struct task *next;
    unsigned int pid;
    char name[30];
};

typedef struct task task_t;

struct context
{
    struct task *currentTask;
    struct context *nextContext;
};

typedef struct context context_t;

void task_create(task_t *task, void (*main)(), unsigned int flags, unsigned int pagedir, unsigned int user_esp, unsigned int isr_esp);
void task_roundRobinNext(registers_t *regs);
extern void usermain(void);
void task_add(task_t *task);
void task_free_frames(page_directory_pointer_table_entry_t *pdpt);
void task_remove(registers_t *regs);
void dump_regs(registers_t *regs);
void execve(char *path, int argc, char **argv);
void task_set_next_task_forever();
void task_print_task_linked_list();
void task_dump_all_task_memory_usage();
void task_disp_heap(unsigned int pid);
#endif