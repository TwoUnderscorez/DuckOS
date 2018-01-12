#include "heap.h"
#include "kernel.h"
#include "../drivers/screen.h"
unsigned int heap_start;
unsigned int heap_end;

void init_heap(void) {
    heap_start = (unsigned int)endkernel + 0x1000;
    heap_end = 0x200000;
    memory_block_header_t * ptr = (memory_block_header_t *)heap_start;
    ptr->length = -1;
    ptr->used = 0;
    ptr->next = 0;
}

void *malloc(unsigned int size) {
    memory_block_header_t * ptr = (memory_block_header_t *)heap_start;
    // First fit algorithm
    while((ptr != 0) && (ptr->used || ptr->length < size ) ){
        ptr = ptr->next;
    }
    if(ptr->length == -1) {
        ptr->length = size;
        ptr->used = 1;
        ptr->next = ptr + size; 
        ptr->next->length = -1;
        ptr->next->used = 0;
        ptr->next->next = 0;
    }
    else { 
        ptr->used = 1;
    }
    if((unsigned int)ptr > heap_end)
        return 0;
    return (void *)((unsigned int)ptr + sizeof(memory_block_header_t));
}

void free(void * ptr) {
    memory_block_header_t * heapblk = (memory_block_header_t *)((unsigned int)ptr - sizeof(memory_block_header_t));
    heapblk->used = 0;
}