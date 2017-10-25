global idt_write

idt_write:
	mov eax, [esp+4]
	lidt [eax]
	ret
