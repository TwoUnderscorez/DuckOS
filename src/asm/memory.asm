global swapPageDirectoryAsm
global enablePagingAsm
global enablePaePagingAsm
global disablePagingAsm
swapPageDirectoryAsm:
    push    ebp
    mov     ebp, esp
    mov     eax, [esp + 8]
    push    ebx
    mov     ebx, cr3
    mov     cr3, eax
    mov     eax, ebx
    pop     ebx
    pop     ebp
    ret

enablePagingAsm:
    push    ebp
    mov     ebp, esp
    mov     eax, cr0
    or      eax, 0x80000000
    mov     cr0, eax
    pop     ebp
    ret

disablePagingAsm:
    push    ebp
    mov     ebp, esp
    mov     eax, cr0
    and     eax, 0xF7FFFFFFF
    mov     cr0, eax
    pop     ebp
    ret

enablePaePagingAsm:
    push    ebp
    mov     ebp, esp
    mov     eax, cr4
    bts     eax, 0x5
    mov     cr4, eax
    pop     ebp
    ret