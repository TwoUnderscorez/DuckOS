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
	puts("Allocating 20 page frames... addrs: ");
	unsigned int * arr = malloc(sizeof(int)*20);
	unsigned int i;
	for(i = 0; i < 20; i++) {
		arr[i] = kalloc_frame();
		screen_print_int(arr[i], 16);
		puts(" ");
	}
	puts("\n");
	puts("Freeing 2 page frames... addrs: ");
	for(i = 0; i < 2; i++) {
		kfree_frame(arr[i]);
		screen_print_int(arr[i], 16);
		puts(" ");
	}
	puts("\n");
	puts("Allocating 4 page frames... addrs: ");
	for(i = 0; i < 4; i++) {
		screen_print_int(kalloc_frame(), 16);
		puts(" ");
	}
	puts("\n");
	free(arr);
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