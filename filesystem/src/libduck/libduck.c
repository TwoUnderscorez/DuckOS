#include "../../lib/libduck.h"
#define HEAP_START  0x700000
#define HEAP_MAX    0x7FFFFFF
unsigned int heap_end;
char ** argv;
int argc;
extern void main(int argc, char ** argv);

struct memory_block_header {
    char used;
    int length;
    struct memory_block_header * next;
} __attribute__((__packed__));

typedef struct memory_block_header memory_block_header_t;

void init_libduck(void){
    __asm__("movl %%ebx, %0" : "=r"(argc));
    __asm__("movl %%ecx, %0" : "=r"(argv));
    init_heap();
}


void _start(void) {
    init_libduck();
    main(argc, argv);
    _exit();
}

void _exit(void) {
    __asm__("mov $0x01, %eax");
    __asm__("int $0x82");
}

void task_yield() {
    __asm__("mov $0x02, %eax");
    __asm__("int $0x82");
}

void screen_clear() {
    __asm__("mov $0x03, %eax");
    __asm__("int $0x83");
}

void putc(char c) {
    __asm__("mov $0x01, %%eax" :: "b" (c));
    __asm__("int $0x83");
}

void puts(char * string) {
    __asm__("mov $0x02, %%eax" :: "b" (string));
    __asm__("int $0x83");
}

void init_heap(void) {
    heap_end = 0x701000;
    memory_block_header_t * ptr = (memory_block_header_t *)HEAP_START;
    ptr->length = -1;
    ptr->used = 0;
    ptr->next = 0;
}

void extend_heap() {
    __asm__("mov $0x03, %%eax" :: "b" (heap_end));
    __asm__("int $0x82");
    heap_end+=0x1000;
}


void *malloc(unsigned int size) {
    memory_block_header_t * ptr = (memory_block_header_t *)HEAP_START;
    void * retaddr;
    // First fit algorithm
    while((ptr != 0) && (ptr->used || ptr->length < size ) ){
        ptr = ptr->next;
    }
    while((unsigned int)((unsigned int)(ptr) + size) > (heap_end)) {extend_heap();}
    if(ptr->length == -1) {
        ptr->length = size;
        ptr->used = 1;
        ptr->next = (memory_block_header_t *)((unsigned int)ptr + (unsigned int)size); 
        if(getc() == 'q') __asm__("int $0x03" :: "a" (ptr->next));
        ptr->next->used = 0;
        ptr->next->next = 0;
        ptr->next->length = -1;
    }
    else { 
        ptr->used = 1;
    }
    if((unsigned int)ptr > HEAP_MAX)
        return 0;
    retaddr = (void *)((unsigned int)ptr + sizeof(memory_block_header_t));
    // memset(retaddr, 0, size);
    return retaddr;
}

void free(void * ptr) {
    memory_block_header_t * heapblk = (memory_block_header_t *)((unsigned int)ptr - sizeof(memory_block_header_t));
    heapblk->used = 0;
}

char getc() {
    int c;
    __asm__("mov $0x01, %eax");
    __asm__("int $0x84");
    __asm__("movl %%edx, %0" : "=r"(c));
    return (char)c;
}

char * gets(char * buff) {
    __asm__("mov $0x02, %%eax" :: "b" (buff));
    __asm__("int $0x84");
    return buff;
}

void execve(char * path, int argc_l, char ** argv_l, int yield) {
    __asm__("nop" :: "b" (path) );
    __asm__("nop" :: "c" (argv_l) );
    __asm__("nop" :: "d" (argc_l) );
    __asm__("int $0x82" :: "a" (4));
    if(yield) {
        __asm__("mov $0x02, %eax");
        __asm__("int $0x82");
    }
}