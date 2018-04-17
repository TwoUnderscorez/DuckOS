nasm -f elf s.asm
ld -T ../../lib/linker.ld -o ../../bin/asmelf -m elf_i386 s.o
