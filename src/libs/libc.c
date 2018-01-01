#include "libc.h"

void *memcpy(void *dst, void *src, int count){
	void *temp_dst = dst;
	while(count--) *(char *)dst++ = *(char *)src++;
	return temp_dst;
}

void *memset(void *dst, char data, int count){
	void *temp_dst = dst;
	while(count--) *(char *)dst++ = data;
	return temp_dst;
}