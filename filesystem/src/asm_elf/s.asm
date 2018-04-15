global _start
section .text
_start:
    mov ebx, 0           ; prints hello
    mov eax, HWADDR        
    push 3
    mov edx, 0    
    int 0x83          

    mov eax, 0            ; exits
    mov ebx, 0      
    int 0x82          

section .data
HWADDR db "Hello ELF!", 0x0A  