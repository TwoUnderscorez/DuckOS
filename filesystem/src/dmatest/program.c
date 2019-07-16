#include "../../lib/libduck.h"
#include "../../lib/string.h"
struct memory_block_header
{
    char used;
    int length;
    struct memory_block_header *next;
} __attribute__((__packed__));
typedef struct memory_block_header memory_block_header_t;

int main(int argc, char **argv);
void disp_heap();

int main(int argc, char **argv)
{
    // Sanity check - allocate an int and assign it a recognizable value
    unsigned int *ptr = malloc(sizeof(int));
    *ptr = 0xDDCCBBAA;
    puts("Heap:\n");
    getc();
    disp_heap(); // And display the heap
    // Allocate 0x1500, which is greater than the initial heap size
    ptr = malloc(0x1500);
    // assign it a recognizable value somewhere neer the end
    *(unsigned int *)((unsigned int)ptr + 0x1450) = 0xDDCCBBAA;
    puts("\nHeap:\n");
    getc();
    disp_heap();
}

void disp_heap()
{
    memory_block_header_t *heap_ptr = (memory_block_header_t *)0x700000;
    unsigned int i;
    unsigned char c;
    if (!heap_ptr->used)
        return;
    do
    {
        screen_set_bgfg(0x70);
        puts("Length: ");
        screen_print_int(heap_ptr->length, 10);
        puts("B; used: ");
        if (heap_ptr->used)
            puts("yes");
        else
            puts("no");
        screen_set_bgfg(0x07);
        for (i = 0; i < heap_ptr->length; i++)
        {
            screen_set_bgfg(0x07);
            c = *((unsigned char *)((unsigned int)heap_ptr + sizeof(memory_block_header_t) + i));
            screen_print_int(c, 16);
            i++;
            screen_set_bgfg(0x0F);
            c = *((unsigned char *)((unsigned int)heap_ptr + sizeof(memory_block_header_t) + i));
            screen_print_int(c, 16);
        }
        heap_ptr = heap_ptr->next;
    } while (heap_ptr->length > 0);
    screen_set_bgfg(0x07);
}