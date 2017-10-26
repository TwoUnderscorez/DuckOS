#include "screen.h"
#include "descriptors.h"
#include "keyboard.h"

void free_write();

int kmain(void *mbd, unsigned int magic){
    int i = 0;
    while(i++<50000000);
	screen_clear();
	if (magic!=0x2BADB002){
		screen_print("Invalid multiboot header.");
		return -1;
	}
	screen_print("Welcome to DuckOS!!!\n");
	screen_print("Setting up the GDT.\n");
	gdt_setup();
	screen_print("GDT set.\n");
	screen_print("Setting up the IDT...\n");
	idt_setup();
	screen_print("IDT set.\n");    
	screen_print("Sending interrupt.\n");
	__asm__("int $0x00");
	__asm__("int $0x80");
    screen_print("Press any key to enter free write mode.\n");
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
        screen_print_char(c);
        while(i++<100000);
        i=0;
    }
}

