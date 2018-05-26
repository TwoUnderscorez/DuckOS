#include "heap.h"
#include "kernel.h"
#include "../drivers/screen.h"
#include "../libs/string.h"

void init_heap(void) {
    heap_start = (unsigned int)&endkernel + 0x1000;
    heap_end = 0x600000;
    memory_block_header_t * ptr = (memory_block_header_t *)heap_start;
    ptr->length = -1;
    ptr->used = 0;
    ptr->next = 0;
}

void *malloc(unsigned int size) {
    memory_block_header_t * ptr = (memory_block_header_t *)heap_start;
    void * retaddr;
    // First fit algorithm
    while((ptr != 0) && (ptr->used || ptr->length < size ) ){
        ptr = ptr->next;
    }
    if((unsigned int)ptr > heap_end || (unsigned int)ptr < heap_start)
    {
        puts("Kernel out of memory.\n");
        puts("[KERNEL PANIC] System halted :(");
        while(1);   
    }
    if(ptr->length == -1) {
        ptr->length = size;
        ptr->used = 1;
        ptr->next = ptr + size + sizeof(memory_block_header_t); 
        if((unsigned int)ptr->next > heap_end || (unsigned int)ptr->next < heap_start)
        {
            puts("Kernel out of memory.\n");
            puts("[KERNEL PANIC] System halted :(");
            while(1);   
        }
        ptr->next->length = -1;
        ptr->next->used = 0;
        ptr->next->next = 0;
    }
    else { 
        ptr->used = 1;
    }
    retaddr = (void *)((unsigned int)ptr + sizeof(memory_block_header_t));
    memset(retaddr, 0, size);
    return retaddr;
}

void free(void * ptr) {
    memory_block_header_t * heapblk = (memory_block_header_t *)((unsigned int)ptr - sizeof(memory_block_header_t));
    heapblk->used = 0;
}