#ifndef ISR_H
#define ISR_H

// A struct that represents the data structure of what PUSHA pushes onto the stack and some extras...
struct registers
{
	// Pushed by isr_common_stub in asmsir.asm
	unsigned int cr2, cr3, ds;

	// Pushed by ISRs in asmisr.asm
	unsigned int edi, esi, ebp, kesp, ebx, edx, ecx, eax;
	unsigned int int_no, err_code;

	// Pushed by int opcode
	unsigned int eip, cs, eflags, useresp, ss;
} __attribute__((packed));

typedef struct registers registers_t;

void isr_handler(registers_t regs);
void dump_regs(registers_t *regs);

#endif