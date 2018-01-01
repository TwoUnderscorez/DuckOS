global gdt_write

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
