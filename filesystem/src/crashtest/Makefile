PROGRAM= program
LIBS=../../lib/libduck.o ../../lib/string.o
EXECUTABLE= ../../bin/crashtest
CC=gcc
CFLAGS=-c -g -Wall -Wno-discarded-qualifiers -Wno-unused-function -Wno-unknown-pragmas -fno-builtin -Wno-char-subscripts -fno-stack-protector -Wno-builtin-declaration-mismatch -m32 -Wno-div-by-zero -Wno-unused-variable
LD= ld
LDFLAGS= -T ../../lib/linker.ld -o $(EXECUTABLE) -m elf_i386

.PHONY: all
.PHONY: clean

all:
	$(CC) $(CFLAGS) $(PROGRAM).c
	$(LD) $(LDFLAGS) $(LIBS) $(PROGRAM).o

clean:
	$(shell rm $(PROGRAM).o)