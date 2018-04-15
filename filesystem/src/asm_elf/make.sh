nasm -f elf s.asm
ld -T ../../lib/linker.ld -o ../../bin/asmelf_test -m elf_i386 s.o
