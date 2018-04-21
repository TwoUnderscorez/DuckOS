gcc -c -Wall -Wno-discarded-qualifiers -Wno-unused-function -Wno-unknown-pragmas -fno-builtin -Wno-char-subscripts -fno-stack-protector -m32 program.c
ld -T ../../lib/linker.ld -o ../../bin/ps -m elf_i386 ../../lib/libduck.o ../../lib/string.o program.o
