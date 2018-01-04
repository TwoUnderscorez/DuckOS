#ifndef LIBC_H
#define LIBC_H

void* 	calloc  (unsigned int nitems, unsigned int size);
void*	malloc	(unsigned int size);
void*	realloc	(void* ptr, unsigned int size);
void	free	(void* ptr);
void	abort	(void);
void	exit	(int exit_code);
#endif