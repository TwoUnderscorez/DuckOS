#include "../../lib/libduck.h"
#include "../../lib/string.h"
struct memory_block_header {
    char used;
    int length;
    struct memory_block_header * next;
} __attribute__((__packed__));
typedef struct memory_block_header memory_block_header_t;

void main(int argc, char ** argv);
void disp_heap();

void main(int argc, char ** argv) {
    unsigned int * ptr = malloc(sizeof(int));
    *ptr = 0xDDCCBBAA;
    puts("Heap:\n");
    getc();
    disp_heap();
    ptr = malloc(0x1500);
    *(unsigned int *)((unsigned int)ptr + 0x1450) = 0xDDCCBBAA;
    puts("\nHeap:\n");
    getc();
    disp_heap();
}

void disp_heap() {
    memory_block_header_t * heap_ptr = 0x700000;
    unsigned int i;
    unsigned char c;
    if(!heap_ptr->used) return;
    do {
        set_screen_bgfg(0x70);
        puts("Length: ");
        screen_print_int(heap_ptr->length, 10);
        puts("B; used: ");
        if(heap_ptr->used) puts("yes");
        else puts("no");
        set_screen_bgfg(0x07);
        for(i = 0; i < heap_ptr->length; i++){
            set_screen_bgfg(0x07);
            c = *((unsigned char *)((unsigned int)heap_ptr + sizeof(memory_block_header_t) + i));
            screen_print_int(c, 16);
            i++;
            set_screen_bgfg(0x0F);
            c = *((unsigned char *)((unsigned int)heap_ptr + sizeof(memory_block_header_t) + i));
            screen_print_int(c, 16);
        }
        heap_ptr = heap_ptr->next;
    } while(heap_ptr->length > 0);
    set_screen_bgfg(0x07);
}