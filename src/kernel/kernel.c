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
	puts("Setting up the GDT... ");
	gdt_setup();
	puts("[OK]\n");
	puts("Setting up the IDT... ");
	idt_setup();
	puts("[OK]\n");
	puts("Setting up the PIC... ");
	PIC_remap();
	puts("[OK]\n");
	puts("Sending test interrupt...\n");
	__asm__("int $0x80");
	getc();
	puts("Initialzing memory...\n");
	init_memory(mbd);
	puts("Memory initialzed!\n");
	dump_frame_map();
	puts("Initialzing heap... ");
	init_heap();
	puts("[OK]\n");
	puts("Initialzing tasking... ");
	__asm__("int $0x81");
	puts("[OK]\n");
	dump_frame_map();
	puts("Initialzing EXT2 filesystem... ");
	init_ext2fs();
	puts("[OK]\n");
	puts("Found ext2 filesystem!\n");
    print_fs_info(); 
	puts("Printing filesystem recursively...\n");
	getc();
	print_filesystem(EXT2_ROOT_DIR_INODE_NUM, 0);
	puts("OK\n");
	puts("Loading ELF... ");
	int * mbuff = malloc(0x14000);
	screen_print_int(mbuff, 16);
	load_file(path_to_inode("/boot/bootscreen.quack"), 0, 0, (void *)mbuff);
	puts("OK\n");
	getc();
	hexDump("elffile", mbuff, 256);
	getc();
	puts("Parsing ELF...\n");
	if(elf_check_supported((Elf32_Ehdr_t *)mbuff)) {
		elf_load_file(mbuff);
		// puts("Running other task...\n");
		// getc();
		__asm__("mov $0x02, %eax");
		__asm__("int $0x82");
		// getc();
		// puts("Returned to main task!\n");
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