#include "../../lib/libduck.h"
#include "../../lib/string.h"
#define HEAP_START 0x700000
#define HEAP_MAX 0x7FFFFFF
unsigned int heap_end;
char **argv;
int argc;
extern void main(int argc, char **argv);
char *itoa(int value, char *str, int base);

struct memory_block_header
{
    char used;
    int length;
    struct memory_block_header *next;
} __attribute__((__packed__));

typedef struct memory_block_header memory_block_header_t;

void init_libduck(void)
{
    __asm__("movl %%esi, %0"
            : "=r"(argc));
    screen_print_int(argc, 10);
    __asm__("movl %%edi, %0"
            : "=r"(argv));
    heap_init();
}

void _start(void)
{
    init_libduck();
    main(argc, argv);
    _exit();
}

void _exit(void)
{
    __asm__("mov $0x01, %eax");
    __asm__("int $0x82");
}

void task_yield()
{
    __asm__("mov $0x02, %eax");
    __asm__("int $0x82");
}

void screen_clear()
{
    __asm__("mov $0x03, %eax");
    __asm__("int $0x83");
}

void screen_set_bgfg(unsigned char bgfg)
{
    __asm__("mov $0x04, %%eax" ::"b"(bgfg));
    __asm__("int $0x83");
}

void putc(char c)
{
    __asm__("mov $0x01, %%eax" ::"b"(c));
    __asm__("int $0x83");
}

void puts(char *string)
{
    __asm__("mov $0x02, %%eax" ::"b"(string));
    __asm__("int $0x83");
}

void heap_init(void)
{
    heap_end = 0x701000;
    memory_block_header_t *ptr = (memory_block_header_t *)HEAP_START;
    ptr->length = -1;
    ptr->used = 0;
    ptr->next = 0;
}

void extend_heap()
{
    __asm__("mov $0x03, %%eax" ::"b"(heap_end));
    __asm__("int $0x82");
    heap_end += 0x1000;
}

void *malloc(unsigned int size)
{
    memory_block_header_t *mblkptr = (memory_block_header_t *)HEAP_START;
    void *retaddr;
    // First fit algorithm
    while ((mblkptr != 0) && (mblkptr->used || mblkptr->length < size))
    {
        mblkptr = mblkptr->next;
    }
    if (mblkptr->length == -1)
    {
        mblkptr->length = size;
        mblkptr->used = 1;
        mblkptr->next = (memory_block_header_t *)((unsigned int)mblkptr +
                                                  (unsigned int)size +
                                                  sizeof(memory_block_header_t));

        while ((unsigned int)mblkptr->next > heap_end)
            extend_heap();
        mblkptr->next->length = -1;
        mblkptr->next->used = 0;
        mblkptr->next->next = 0;
    }
    else
    {
        mblkptr->used = 1;
    }
    if ((unsigned int)mblkptr > heap_end)
        return 0;
    retaddr = (void *)((unsigned int)mblkptr + sizeof(memory_block_header_t));
    return retaddr;
}

void free(void *ptr)
{
    memory_block_header_t *heapblk =
        (memory_block_header_t *)((unsigned int)ptr -
                                  sizeof(memory_block_header_t));
    heapblk->used = 0;
}

char getc()
{
    int c;
    __asm__("mov $0x01, %eax");
    __asm__("int $0x84");
    __asm__("movl %%edx, %0"
            : "=r"(c));
    return (char)c;
}

char *gets(char *buff)
{
    __asm__("mov $0x02, %%eax" ::"b"(buff));
    __asm__("int $0x84");
    return buff;
}

void execve(char *path, int argc_l, char **argv_l, int yield)
{
    __asm__("nop" ::"b"(path));
    __asm__("nop" ::"d"(argc_l));
    __asm__("nop" ::"c"(argv_l));
    __asm__("int $0x82" ::"a"(4));
    if (yield)
    {
        __asm__("mov $0x02, %eax");
        __asm__("int $0x82");
    }
}

void load_inode(int inode_num, void *buf)
{
    __asm__("nop" ::"b"(inode_num));
    __asm__("nop" ::"c"(buf));
    __asm__("int $0x85" ::"a"(0x01));
}

void load_directory_structure(EXT2_INODE_t *inode, void *buf)
{
    __asm__("nop" ::"b"(inode));
    __asm__("nop" ::"c"(buf));
    __asm__("int $0x85" ::"a"(0x02));
}

void __attribute__((optimize("O0"))) load_file(EXT2_INODE_t *inode, int seek, int skip, void *buff)
{
    int out;
    __asm__("nop" ::"b"(inode));
    __asm__("nop" ::"c"(seek));
    __asm__("nop" ::"d"(skip));
    __asm__("movl %1, %%edi\n\t"
            : "=r"(out)
            : "r"(buff));
    __asm__("int $0x85" ::"a"(0x03));
}

int __attribute__((optimize("O0"))) path_to_inode(char *path)
{
    int inode;
    __asm__("nop" ::"b"(path));
    __asm__("int $0x85" ::"a"(0x04));
    __asm__("movl %%edx, %0"
            : "=r"(inode));
    return inode;
}

int __attribute__((optimize("O0"))) path_exists(char *path)
{
    int inode;
    __asm__("nop" ::"b"(path));
    __asm__("int $0x85" ::"a"(0x05));
    __asm__("movl %%edx, %0"
            : "=r"(inode));
    return inode;
}