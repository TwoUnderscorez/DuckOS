#ifndef KERNEL_H
#define KERNEL_H

void free_write();
extern unsigned int endkernel;

enum STATUS_CODE
{
    // General
    GENERAL_SUCCESS,
    GENERAL_FAILURE
}

#endif