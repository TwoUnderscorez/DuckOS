#ifndef MEMORY_H
#define MEMORY_H
#include "kernel.h"
#include "multiboot.h"
#define MMAP_GET_NUM 0
#define MMAP_GET_ADDR 1
#define PAGE_SIZE 4096

struct page_directory_pointer_table_entry {
    unsigned char present : 1;
    unsigned char ro_rw : 1;
    unsigned char kernel_user : 1;
    unsigned char writethrough_writeback : 1;
    unsigned char caching : 1;
    unsigned char accessed : 1;
    unsigned char always0 : 1; 
    unsigned char size : 1;
    unsigned char ignored : 1;
    unsigned char available : 3;
    unsigned int page_directory_table_address : 20;
    unsigned int padd : 32;
} __attribute__((__packed__));

typedef struct page_directory_pointer_table_entry page_directory_pointer_table_entry_t;

struct page_directory_table_entry {
    unsigned char present : 1;
    unsigned char ro_rw : 1;
    unsigned char kernel_user : 1;
    unsigned char writethrough_writeback : 1;
    unsigned char caching : 1;
    unsigned char accessed : 1;
    unsigned char always0 : 1; 
    unsigned char size : 1; // 0 for 4kb
    unsigned char ignored : 1;
    unsigned char available : 3;
    unsigned int page_table_address : 20;
    unsigned int padd : 32;
} __attribute__((__packed__));

typedef struct page_directory_table_entry page_directory_table_entry_t;

struct page_table_entry {
    unsigned char present : 1;
    unsigned char ro_rw : 1;
    unsigned char kernel_user : 1;
    unsigned char writethrough_writeback : 1;
    unsigned char caching : 1;
    unsigned char accessed : 1;
    unsigned char dirty : 1;
    unsigned char always0 : 1;
    unsigned char global : 1;
    unsigned char available : 3;
    unsigned int physical_page_address : 20;
    unsigned int padd : 32;
} __attribute__((__packed__));

typedef struct page_table_entry page_table_entry_t;

extern void loadPageDirectoryAsm(unsigned int * ptr);
extern void enablePagingAsm(void);
extern void enablePaePagingAsm(void);
extern void disablePagingAsm(void);
void init_memory(multiboot_info_t * mbd);
void dump_mmap(void);
unsigned int kalloc_frame();
void kfree_frame(unsigned int page_frame_addr);
unsigned int create_pdpt();
#endif