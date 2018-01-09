#include "kernel.h"
#include "descriptors.h"
#include "memory.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../asm/asmio.h"

int kmain(multiboot_information_t * mbd, unsigned int magic){
	screen_clear();
	if (magic!=0x2BADB002){
		puts("Invalid multiboot header.");
		return -1;
	}
	puts("Welcome to DuckOS!!!\n");
	puts("Setting up the GDT...\n");
	gdt_setup();
	puts("GDT set.\n");
	puts("Setting up the IDT...\n");
	idt_setup();
	puts("IDT set.\n");
	puts("Sending interrupt.\n");
	__asm__("int $0x00");
	__asm__("int $0x80");
	puts("Initialzing memory...\n");
	init_memory();
	puts("Memory set.\n");
	int * ptr;
	ptr = 0x1FFF00;
	*ptr = 35;
	screen_print_int(*ptr, 10);
	puts("\n");
	puts("Press any key to enter free write mode.\n");
    getc();
    free_write();
	return 0;
}

void free_write() {
	screen_clear();
    char c;
    int i;
    while(1) {
        c = getc();
        putc(c);
        while(i++<100000);
        i=0;
    }
}