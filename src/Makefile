CC:=gcc
PRINTF_FLAGS:=-DPRINTF_DISABLE_SUPPORT_LONG_LONG
LOGGER_FLAGS:=-DLOG_USE_COLOR
THIRDPARTY_FLAGS:=$(PRINTF_FLAGS) $(LOGGER_FLAGS)
CFLAGS:=-c -g -Wall -Wno-discarded-qualifiers -Wno-unused-function -Wno-unknown-pragmas -fno-builtin -Wno-char-subscripts -fno-stack-protector -fno-omit-frame-pointer -m32 $(THIRDPARTY_FLAGS)
LD:=ld
LDFLAGS:=-T linker.ld -o kernel.sys -m elf_i386
ASM:=nasm
ASMFLAGS:=-f elf

.PHONY: boot.o kernel.o screen.o keyboard.o asmio.o asmisr.o gdt.o idt.o descriptors.o isr.o string.o memoryasm.o memory.o heap.o task.o syscall.o testuserapp.o pic.o atapio.o ext2.o elf.o bitmap.o math.o

all: kernel.o boot.o testuserapp.o
	$(LD) $(LDFLAGS) boot.o kernel.o screen.o keyboard.o asmio.o asmisr.o gdt.o idt.o descriptors.o isr.o string.o memoryasm.o memory.o heap.o task.o syscall.o testuserapp.o pic.o atapio.o ext2.o elf.o bitmap.o math.o serial.o printf.o log.o

kernel.o: screen.o keyboard.o asmio.o descriptors.o memory.o heap.o atapio.o ext2.o serial.o printf.o log.o
	$(CC) $(CFLAGS) kernel/kernel.c

boot.o: kernel.o
	$(ASM) $(ASMFLAGS) -o boot.o boot/boot.asm

memory.o: memoryasm.o heap.o bitmap.o
	$(CC) $(CFLAGS) kernel/memory.c

memoryasm.o:
	$(ASM) $(ASMFLAGS) -o memoryasm.o asm/memory.asm

gdt.o:
	$(ASM) $(ASMFLAGS) -o gdt.o asm/gdt.asm

idt.o: isr.o
	$(ASM) $(ASMFLAGS) -o idt.o asm/idt.asm

asmisr.o:
	$(ASM) $(ASMFLAGS) -o asmisr.o asm/asmisr.asm

descriptors.o: asmisr.o gdt.o idt.o 
	$(CC) $(CFLAGS) kernel/descriptors.c

screen.o: string.o
	$(CC) $(CFLAGS) drivers/screen.c
    
keyboard.o:
	$(CC) $(CFLAGS) drivers/keyboard.c
    
asmio.o:
	$(ASM) $(ASMFLAGS) -o asmio.o asm/asmio.asm

isr.o: task.o
	$(CC) $(CFLAGS) kernel/isr.c

string.o:
	$(CC) $(CFLAGS) libs/string.c

libc.o:
	$(CC) $(CFLAGS) libs/libc.c

math.o:
	$(CC) $(CFLAGS) libs/math.c

printf.o:
	$(CC) $(CFLAGS) libs/printf.c

log.o:
	$(CC) $(CFLAGS) libs/log.c

bitmap.o:
	$(CC) $(CFLAGS) libs/bitmap.c

heap.o:
	$(CC) $(CFLAGS) kernel/heap.c

task.o: syscall.o elf.o
	$(CC) $(CFLAGS) kernel/task.c

syscall.o:
	$(CC) $(CFLAGS) kernel/syscall.c

pic.o:
	$(CC) $(CFLAGS) kernel/pic.c

testuserapp.o:
	$(ASM) $(ASMFLAGS) -o testuserapp.o asm/testuserapp.asm

atapio.o: pic.o
	$(CC) $(CFLAGS) drivers/atapio.c

ext2.o: math.o
	$(CC) $(CFLAGS) drivers/ext2.c

elf.o:
	$(CC) $(CFLAGS) kernel/elf.c

serial.o:
	$(CC) $(CFLAGS) drivers/serial.c

clean:
	$(shell rm -rf *.o kernel.sys)