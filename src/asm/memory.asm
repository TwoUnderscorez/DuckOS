global loadPageDirectoryAsm
global enablePagingAsm
global enablePaePagingAsm
loadPageDirectoryAsm:
    push    ebp
    mov     ebp, esp
    mov     eax, [esp + 8]
    mov     cr3, eax
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

enablePaePagingAsm:
    push    ebp
    mov     ebp, esp
    mov     eax, cr4
    bts     eax, 0x5
    mov     cr4, eax
    pop     ebp
    ret