#ifndef HEAP_H
#define HEAP_H
unsigned int heap_start;
unsigned int heap_end;
// Every block in the heap will start with this header
struct memory_block_header
{
    char used;
    int length;
    struct memory_block_header *next;
} __attribute__((__packed__));

typedef struct memory_block_header memory_block_header_t;

void kheap_init(void);
void *malloc(unsigned int size);
void free(void *ptr);
void kheap_print_stats();

#endif