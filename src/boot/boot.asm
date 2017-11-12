global loader
 
extern kmain

; Prepare GRUB header
MODULEALIGN equ  1<<0
MEMINFO     equ  1<<1
FLAGS       equ  MODULEALIGN | MEMINFO
MAGIC       equ    0x1BADB002
CHECKSUM    equ -(MAGIC + FLAGS)
 
section .text

; Define GRUB Header
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

STACKSIZE equ 0x4000
 
loader:
	; Setup the stack
	mov  esp, stack + STACKSIZE

	push eax ; Magic number
	push ebx ; Multiboot information
	call kmain
	
	cli
	.hang:
    		hlt
    		jmp  .hang

section .bss
 
align 4
stack:
    resb STACKSIZE
