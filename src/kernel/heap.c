#include "heap.h"
#include "kernel.h"
#include "../drivers/screen.h"
#include "../libs/string.h"

static void init_memory_block(
    memory_block_header_t *ptr,
    int length,
    char used,
    memory_block_header_t *next)
{
    ptr->length = length;
    ptr->used = used;
    ptr->next = next;
}

/**
 * @brief Init the kernel heap
 * 
 */
void kheap_init(void)
{
    heap_start = (unsigned int)&endkernel + 0x1000;
    heap_end = 0x600000;
    memory_block_header_t *ptr = (memory_block_header_t *)heap_start;
    init_memory_block(ptr, -1, 0, (memory_block_header_t *)0x0);
}

/**
 * @brief Recursively merge fragmented memory blocks
 * 
 * TODO: Make sure this works properly
 *  
 * @param ptr 
 * @param size 
 * @return int 
 */
static int merge_memory_blocks(memory_block_header_t *ptr, unsigned int size)
{
    // If ptr and ptr->next excist and are unused.
    if (ptr && !ptr->used && ptr->next && !ptr->next->used)
    {
        // ptr and ptr->next are larger than size
        if (ptr->length + ptr->next->length >= (int)size)
        {
            // Update the size of the current block
            ptr->length += ((int)sizeof(memory_block_header_t) +
                            ptr->next->length);
            // Merge the blocks and return
            ptr->next = ptr->next->next;
            return 1;
        }
        // If ptr->next->next is valid and unused
        else if (ptr->next->next && !ptr->next->next->used)
        {
            // See (recursively) if the next block is free and is large enough
            if (merge_memory_blocks(
                    ptr->next,
                    size - (unsigned int)ptr->length))
            {
                // Update the size of the current block
                ptr->length += ((int)sizeof(memory_block_header_t) +
                                ptr->next->length);
                // Merge the blocks and return
                ptr->next = ptr->next->next;
                return 1;
            }
        }
    }
    return 0;
}

/**
 * @brief Kernel heap malloc
 * 
 * @param size 
 * @return void* 
 */
void *malloc(unsigned int size)
{
    memory_block_header_t *ptr = (memory_block_header_t *)heap_start,
                          *tmpptr = 0;
    void *retaddr = 0;
    int new_ptr_n_len = 0;
    // First fit algorithm
    while (ptr->length > -1)
    {
        if ((ptr->length >= (int)size && !ptr->used) ||
            merge_memory_blocks(ptr, size))
        {
            puts(" \b");
            break;
        }
        ptr = ptr->next;
    }
    if ((unsigned int)ptr > heap_end || (unsigned int)ptr < heap_start)
        goto _outofmem;
    if (ptr->length == -1)
    {
        init_memory_block(
            ptr,
            (int)size,
            1,
            (memory_block_header_t *)((unsigned int)ptr +
                                      (unsigned int)size +
                                      sizeof(memory_block_header_t)));

        if ((unsigned int)ptr->next > heap_end ||
            (unsigned int)ptr->next < heap_start)
            goto _outofmem;

        init_memory_block(
            ptr->next,
            -1,
            0,
            (memory_block_header_t *)0x0);
    }
    else if (ptr->length < (int)size)
    {
        goto _outofmem;
    }
    else
    {
        ptr->used = 1;

        // The length of the newly created memory block
        // hdr size * 2: current header + new block's header
        new_ptr_n_len =
            (int)ptr->next -
            ((int)ptr +
             (2 * (int)sizeof(memory_block_header_t)) +
             (int)size); // <- size is less than ptr->length

        // Create new smaller fragments from current block
        if (
            (new_ptr_n_len > ((int)sizeof(memory_block_header_t) + 2)) &&
            ptr->next->length > 0)
        {
            ptr->length = (int)size;
            tmpptr = ptr->next;
            ptr->next = (memory_block_header_t *)((unsigned int)ptr +
                                                  (unsigned int)size +
                                                  sizeof(memory_block_header_t));

            init_memory_block(
                ptr->next,
                new_ptr_n_len,
                0,
                tmpptr);
        }
    }
    retaddr = (void *)((unsigned int)ptr + sizeof(memory_block_header_t));
    if (!ptr)
        goto _outofmem;
    memset(retaddr, 0, size);
    return retaddr;
_outofmem:
    puts("KMODE Exception: Kernel out of memory.\n");
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
    if (heapblk->length < 1 ||
        (unsigned int)heapblk->next > heap_end ||
        heapblk->used > 1)
    {
        puts("KMODE Exception: Called kfree on invalid memory.\n");
        puts("[KERNEL PANIC] System halted :(");
        while (1)
            ;
    }
    heapblk->used = 0;
}

void kheap_print_stats()
{
    memory_block_header_t *ptr = (memory_block_header_t *)heap_start;
    int used_heap = 0,
        heap_nof_blocks = 0,
        heap_nof_used_blocks = 0,
        fragmented_heap = 0,
        kheap_size = heap_end - heap_start;
    while (ptr->next->length > 0)
    {
        fragmented_heap += ptr->length;
        heap_nof_blocks++;
        if (ptr->used)
        {
            used_heap += ptr->length;
            heap_nof_used_blocks++;
        }
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
    puts(" with ");
    screen_print_int(heap_nof_used_blocks, 10);
    puts(" of them used, avg used block size: ");
    screen_print_int(fragmented_heap / heap_nof_blocks, 10);
    puts(".\n");
}