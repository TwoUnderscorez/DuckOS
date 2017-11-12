#ifndef ISR_H
#define ISR_H

// A struct that represents the data structure of what PUSHA pushes onto the stack
struct registers
{
	unsigned int ds;
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
	unsigned int int_no, err_code;
	unsigned int eip, cs, eflags, useresp, ss;
};

typedef struct registers registers_t;

#endif
