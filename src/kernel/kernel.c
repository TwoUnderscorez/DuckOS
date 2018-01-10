#include "kernel.h"
#include "descriptors.h"
#include "memory.h"
#include "multiboot.h"
#include "heap.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../asm/asmio.h"

int kmain(multiboot_info_t * mbd, unsigned int magic){
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
	init_memory(mbd);
	puts("Memory initialized.\n");
	puts("Initialzing heap...\n");
	init_heap();
	puts("Heap initialized.\n");
	int * ptr = malloc(sizeof(int));
	screen_print_int(ptr, 16);
	puts("\n");
	int * ptr1 = malloc(sizeof(int));
	screen_print_int(ptr1, 16);
	puts("\n");
	free(ptr1);
	int * ptr2 = malloc(sizeof(int));
	screen_print_int(ptr2, 16);
	puts("\n");
	free(ptr);
	int * ptr3 = malloc(sizeof(int));
	screen_print_int(ptr3, 16);
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