#include "kernel.h"
#include "descriptors.h"
#include "memory.h"
#include "multiboot.h"
#include "heap.h"
#include "pic.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../asm/asmio.h"
#include "../boot/bootscreen.h"
#include "../drivers/atapio.h"
#include "../drivers/ext2.h"

int kmain(multiboot_info_t * mbd, unsigned int magic){
	screen_clear();
	unsigned int i;
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
	// __asm__("int $0x81");
	puts("OK\nRunning other task...\n");
	// __asm__("int $0x82");
	puts("Returned to main task!\n");
	unsigned char * sector = malloc(sizeof(unsigned char)*512);
	puts("Reading...\n");
	ata_read_sectors(0, 1, sector);
	*(sector + 509) = 0xFA;
	puts("Writing...\n");
	ata_write_sectors(0, 1, sector); 
	ata_io_delay();
	puts("Reading...\n");
	ata_read_sectors(0, 1, sector);
	puts("Dumping...\n");
	for(i=0; i<512; i++){
		screen_print_int(*(sector + i), 16);
		puts(" ");
	}
	puts("\n");
	screen_print_int(sizeof(EXT2_SUPERBLOCK_t), 10);
	puts("\nPress any key to enter free write mode.\n");
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