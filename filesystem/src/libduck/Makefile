PROGRAM=libduck
LIBPATH=../../lib/libduck.o
CC=gcc
CFLAGS=-c -Wall -Wno-discarded-qualifiers -Wno-unused-function -Wno-unknown-pragmas -fno-builtin -Wno-char-subscripts -fno-stack-protector -m32 -o $(LIBPATH)

.PHONY: all

all:
	$(CC) $(CFLAGS) $(PROGRAM).c
