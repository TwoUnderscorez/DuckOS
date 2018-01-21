#ifndef TASK_H
#define TASK_H

#include "isr.h"
void other_main(void);
void init_tasking();
 
struct task {
    registers_t regs;
    struct task *next;
};

typedef struct task task_t;
 
void create_task(task_t*, void(*)(), unsigned int, unsigned int*);
void roundRobinNext(registers_t * regs);



#endif