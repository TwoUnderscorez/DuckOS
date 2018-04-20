#include "kernel.h"
#include "descriptors.h"
#include "memory.h"
#include "multiboot.h"
#include "heap.h"
#include "pic.h"
#include "elf.h"
#include "task.h"
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
	screen_print_int(strlen("asdf"), 10);
	getc();
	execve("/boot/bootscreen.quack", 0, 0);
	__asm__("mov $0x02, %eax");
	__asm__("int $0x82");
	execve("/bin/terminal", 0, 0);
	set_next_task_forever();
	__asm__("mov $0x02, %eax");
	__asm__("int $0x82");
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