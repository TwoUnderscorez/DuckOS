gcc -c -Wall -Wno-discarded-qualifiers -Wno-unused-function -Wno-unknown-pragmas -fno-builtin -Wno-char-subscripts -fno-stack-protector -m32 bootscreen.c
ld -T ../../lib/linker.ld -o ../../boot/bootscreen.quack -m elf_i386 ../../lib/libduck.o ../../lib/string.o bootscreen.o
