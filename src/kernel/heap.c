#include "heap.h"
#include "kernel.h"
#include "../drivers/screen.h"
#include "../libs/string.h"

/**
 * @brief Init the kernel heap
 * 
 */
void kheap_init(void)
{
    heap_start = (unsigned int)&endkernel + 0x1000;
    heap_end = 0x600000;
    memory_block_header_t *ptr = (memory_block_header_t *)heap_start;
    ptr->length = -1;
    ptr->used = 0;
    ptr->next = 0;
}

/**
 * @brief Kernel heap malloc
 * 
 * @param size 
 * @return void* 
 */
void *malloc(unsigned int size)
{
    memory_block_header_t *ptr = (memory_block_header_t *)heap_start;
    void *retaddr = 0;
    // First fit algorithm
    while ((ptr != 0) && (ptr->used || ptr->length < size))
    {
        ptr = ptr->next;
    }
    if ((unsigned int)ptr > heap_end || (unsigned int)ptr < heap_start)
        goto _outofmem;
    if (ptr->length == -1)
    {
        ptr->length = size;
        ptr->used = 1;
        ptr->next = (memory_block_header_t *)((unsigned int)ptr +
                                              (unsigned int)size +
                                              sizeof(memory_block_header_t));
        if ((unsigned int)ptr->next > heap_end ||
            (unsigned int)ptr->next < heap_start)
            goto _outofmem;
        ptr->next->length = -1;
        ptr->next->used = 0;
        ptr->next->next = 0;
    }
    else
    {
        ptr->used = 1;
    }
    retaddr = (void *)((unsigned int)ptr + sizeof(memory_block_header_t));
    if (!retaddr)
        goto _outofmem;
    memset(retaddr, 0, size);
    return retaddr;
_outofmem:
    puts("Kernel out of memory.\n");
    puts("[KERNEL PANIC] System halted :(");
    while (1)
        ;
}

/**
 * @brief Kernel heap free
 * 
 * @param ptr 
 */
void free(void *ptr)
{
    memory_block_header_t *heapblk = (memory_block_header_t *)((unsigned int)ptr - sizeof(memory_block_header_t));
    heapblk->used = 0;
}

void kheap_print_stats()
{
    memory_block_header_t *ptr = (memory_block_header_t *)heap_start;
    int used_heap = 0,
        heap_nof_blocks = 0,
        fragmented_heap = 0,
        kheap_size = heap_end - heap_start;
    while (ptr->next->length > 0)
    {
        fragmented_heap += ptr->length;
        heap_nof_blocks++;
        if (ptr->used)
            used_heap += ptr->length;
        ptr = ptr->next;
    }

    puts("kheap stats: total: ");
    screen_print_int(kheap_size, 10);
    puts("B, used: ");
    screen_print_int(used_heap, 10);
    puts("B, ");
    screen_print_int(100 * (used_heap / kheap_size), 10);
    puts("%, # of blocks: ");
    screen_print_int(heap_nof_blocks, 10);
    puts(", avg block size: ");
    screen_print_int(fragmented_heap / heap_nof_blocks, 10);
    puts(".\n");
}