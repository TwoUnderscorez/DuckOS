#ifndef TASK_H
#define TASK_H

#include "isr.h"
void other_main(void);
void init_tasking(registers_t * regs);
 
struct task {
    registers_t regs;
    struct task *next;
    unsigned int pid;
    char name[30];
};

typedef struct task task_t;

struct context {
    struct task *currentTask;
    struct context *nextContext;
};

typedef struct context context_t;
 
void create_task(task_t *task, void (*main)(), unsigned int flags, unsigned int pagedir, unsigned int user_esp, unsigned int isr_esp);
void roundRobinNext(registers_t * regs);
extern void usermain(void);
void add_task(task_t * task);
void remove_task(registers_t * regs);
void dump_regs(registers_t * regs);
void execve(char * path, int argc, char ** argv);
void set_next_task_forever();
void print_task_linked_list();
void dump_all_task_memory_usage();
#endif