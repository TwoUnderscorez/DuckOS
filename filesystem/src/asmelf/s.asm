global _start
section .text
_start:
    mov ebx, HWADDR           ; prints hello
    mov eax, 0x02        
    mov edx, 0    
    int 0x83          

    mov eax, 1            ; exits
    mov ebx, 0      
    int 0x82          

section .data
HWADDR db "Hello ELF!", 0x0A  