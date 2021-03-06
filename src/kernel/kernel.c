#include "kernel.h"
#include "../drivers/screen.h"
#include "descriptors.h"
#include "memory.h"
#include "multiboot.h"
#include "heap.h"
#include "pic.h"
#include "elf.h"
#include "task.h"
#include "../drivers/serial.h"
#include "../drivers/keyboard.h"
#include "../asm/asmio.h"
#include "../drivers/atapio.h"
#include "../drivers/ext2.h"
#include "../libs/string.h"
#include "../libs/printf.h"
#include "../libs/log.h"

int kmain(multiboot_info_t *mbd, unsigned int magic)
{
	screen_clear();
	if (magic != 0x2BADB002)
	{
		puts("Invalid multiboot header.");
		return -1;
	}
	puts("Welcome to DuckOS!!!\n");
	screen_set_ymax(25);
	puts("Setting up the GDT... ");
	gdt_setup();
	puts("[OK]\nSetting up the IDT... ");
	idt_setup();
	puts("[OK]\nInitialzing serial...");
	serial_init();
	printf_serial(
		"%s",
		"kmode/If you are seeing this message, serial is working!\n");
	puts("[OK]\nnInitialzing logger...");
	klog_set_level(KLOG_DEBUG);
	klog_set_quiet(0);
	klog_info("%s", "Logger initialized.");
	puts("[OK]\nSetting up the PIC... ");
	PIC_remap();
	puts("[OK]\n");
	puts("Sending test interrupt...\n");
	__asm__("int $0x80");
	getc();
	puts(" [OK]\n");

	init_memory(mbd);

	kheap_init();

	klog_info("Initialzing tasking... ");
	__asm__("int $0x81");
	klog_info("OK!");

	ext2_init_fs();

	char **argv = strsplit("/boot/bootscreen.quack ", ' ');
	execve(argv[0], 1, argv);
	__asm__("mov $0x02, %eax");
	__asm__("int $0x82");
	int j;
	for (int i = 0; i < 25; i++)
	{
		j = 0;
		while (j++ < 10000000)
			;
		screen_scroll();
	}
	argv = strsplit("/bin/terminal ", ' ');
	screen_set_ymax(24);
	screen_clear();
	screen_enable_cursor();
	do
	{
		execve(argv[0], 1, argv);
		task_set_next_task_forever();
		__asm__("mov $0x02, %eax");
		__asm__("int $0x82");
		screen_clear();
		screen_set_cursor_position(0, 0);
		puts("You have closed the last terminal, relaunching...\n");
	} while (1);
	puts("Press any key to enter free write mode.\n");
	getc();
	free_write();
	return 0;
}

void free_write()
{
	screen_clear();
	char c;
	int i;
	while (1)
	{
		c = getc();
		putc(c);
		while (i++ < 100000)
			;
		i = 0;
	}
}