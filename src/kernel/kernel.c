#include "kernel.h"
#include "descriptors.h"
#include "memory.h"
#include "multiboot.h"
#include "heap.h"
#include "pic.h"
#include "elf.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../asm/asmio.h"
#include "../boot/bootscreen.h"
#include "../drivers/atapio.h"
#include "../drivers/ext2.h"
#include "../libs/string.h"

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
	puts("Initialzing interrupts...\n");
	puts("Setting up the IDT... ");
	idt_setup();
	puts("[IDT set]\n");
	puts("Setting up the PIC... ");
	PIC_remap();
	puts("[PIC remapped]\n");
	puts("Sending test interrupt...\n");
	__asm__("int $0x80");
	puts("Initialzing memory...\n");
	init_memory(mbd);
	puts("Memory initialized.\n");
	puts("Initialzing heap...\n");
	init_heap();
	puts("Heap initialized.\n");
	puts("Initialzing tasking... ");
	__asm__("int $0x81");
	puts("OK\nRunning other task...\n");
	__asm__("int $0x82");
	puts("Returned to main task!\n");
	puts("Initialzing EXT2 filesystem...\n");
	init_ext2fs();
	puts("Filesystem is ready!\n");
	puts("Printing filesystem recursively...\n");
	print_filesystem(EXT2_ROOT_DIR_INODE_NUM, 0);
	puts("OK\n");
	puts("Loading ELF... ");
	int * mbuff = malloc(1024);
	load_file(4036, 0, 0, (void *)mbuff);
	puts("OK\n");
	getc();
	hexDump("elffile", mbuff, 256);
	getc();
	puts("Parsing ELF...\n");
	int i;
	if(elf_check_supported(mbuff)) {
		elf_load_file(mbuff);
	}
	
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