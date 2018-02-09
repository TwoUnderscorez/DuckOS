CC:=gcc
CFLAGS:=-c -Wall -Wno-unknown-pragmas -fno-builtin -Wno-char-subscripts -fno-stack-protector -m32
LD:=ld
LDFLAGS:=-T linker.ld -o kernel.sys -m elf_i386
ASM:=nasm
ASMFLAGS:=-f elf

all: kernel.o boot.o testuserapp.o
	$(LD) $(LDFLAGS) boot.o kernel.o screen.o keyboard.o asmio.o asmisr.o gdt.o idt.o descriptors.o isr.o string.o memoryasm.o memory.o heap.o task.o syscall.o testuserapp.o atapio.o bootscreen.o

kernel.o: screen.o keyboard.o asmio.o descriptors.o memory.o heap.o bootscreen.o atapio.o
	$(CC) $(CFLAGS) kernel/kernel.c

boot.o: kernel.o
	$(ASM) $(ASMFLAGS) -o boot.o boot/boot.asm

bootscreen.o:
	$(CC) $(CFLAGS) boot/bootscreen.c

memory.o: memoryasm.o
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

heap.o:
	$(CC) $(CFLAGS) kernel/heap.c

task.o: syscall.o
	$(CC) $(CFLAGS) kernel/task.c

syscall.o:
	$(CC) $(CFLAGS) kernel/syscall.c

testuserapp.o:
	$(ASM) $(ASMFLAGS) -o testuserapp.o asm/testuserapp.asm

atapio.o:
	$(CC) $(CFLAGS) drivers/atapio.c

clean:
	$(shell rm -rf *.o kernel.sys)