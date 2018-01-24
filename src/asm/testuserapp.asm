section .usrtext
global usermain
usermain:
    mov eax, msg
    int 0x83
    int 0x82
    ret
msg db 'Hello from userland!', 0xA, 0