#ifndef TASK_H
#define TASK_H

#include "isr.h"
void otherMain(void);
void initTasking();
 
struct task {
    registers_t regs;
    struct task *next;
};

typedef struct task task_t;
 
void initTasking();
void createTask(task_t*, void(*)(), unsigned int, unsigned int*);
void roundRobinNext(registers_t * regs);



#endif