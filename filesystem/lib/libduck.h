#ifndef LIBDUCK_H
#define LIBDUCK_H

extern void main(void);
void _start(void);
void _exit(void);
void task_yield();
void init_libduck(void);
void screen_clear();
void puts(char * string);
void init_heap(void);
void extend_heap();
void *malloc(unsigned int size);
void free(void * ptr);
char getc();
char * gets(char * buff);
#endif