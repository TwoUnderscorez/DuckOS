gcc -c -Wall -Wno-discarded-qualifiers -Wno-unused-function -Wno-unknown-pragmas -fno-builtin -Wno-char-subscripts -fno-stack-protector -m32 program.c
ld -T ../../lib/linker.ld -o ../../bin/xxd -m elf_i386 ../../lib/string.o ../../lib/libduck.o program.o
