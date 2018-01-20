global gdt_write
global tss_flush
gdt_write:
	mov 	eax, [esp+4]; 1st param is the addr of the GDT descriptor in memory
	lgdt 	[eax]
	mov 	ax, 0x10 	; Kernel DataSeg
	mov 	ds, ax
	mov 	es, ax
	mov 	fs, ax
	mov 	gs, ax
	mov 	ss, ax
	jmp 	0x08:flush	; Kernel CodeSeg
flush:
	ret

tss_flush:
   mov ax, 0x2B      ; Load the index of our TSS structure - The index is
                     ; 0x28, as it is the 5th selector and each is 8 bytes
                     ; long, but we set the bottom two bits (making 0x2B)
                     ; so that it has an RPL of 3, not zero.
   ltr ax            ; Load 0x2B into the task state register.
   ret 