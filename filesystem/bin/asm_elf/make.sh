nasm -f elf s.asm
ld -T linker.ld -o asmelf -m elf_i386 s.o