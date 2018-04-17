#ifndef MEMORY_H
#define MEMORY_H
#include "kernel.h"
#include "multiboot.h"
#include "isr.h"
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
    unsigned char present : 1; // If the bit is set, the page is actually in physical memory at the moment. 
    unsigned char ro_rw : 1; // 'Read/Write' permissions flag.
    unsigned char kernel_user : 1; // 'User/Supervisor' bit.
    unsigned char writethrough_writeback : 1; // 'Write-Through' abilities of the page.
    unsigned char caching : 1; 
    unsigned char accessed : 1; // is used to discover whether a page has been read or written to. If it has, then the bit is set, otherwise, it is not.
    unsigned char dirty : 1; // If the Dirty flag ('D') is set, then the page has been written to. This flag is not updated by the CPU, and once set will not unset itself. 
    unsigned char always0 : 1;
    unsigned char global : 1; // if set, prevents the TLB from updating the address in its cache if CR3 is reset. 
    unsigned char available : 3; // Free for use by me
    unsigned int physical_page_address : 20; 
    unsigned int padd : 32;
} __attribute__((__packed__));

typedef struct page_table_entry page_table_entry_t;

extern unsigned int swapPageDirectoryAsm(unsigned int * ptr);
extern void enablePagingAsm(void);
extern void enablePaePagingAsm(void);
extern void disablePagingAsm(void);
void init_memory(multiboot_info_t * mbd);
void dump_mmap(void);
void apply_addr_to_frame_map(unsigned int base, unsigned int limit, unsigned char used);
void apply_mmap_to_frame_map(void);
unsigned int kalloc_frame();
void kfree_frame(unsigned int page_frame_addr);
unsigned int create_pdpt();
int addr_to_frameidx(unsigned int addr);
unsigned int frameidx_to_addr(int frameidx);
void dump_frame_map(void);
void load_kernel_pdpt();
void map_vaddr_to_pdpt(page_directory_pointer_table_entry_t * pdpt, page_table_entry_t * data, unsigned int base, unsigned int limit);
void brk(page_directory_pointer_table_entry_t * pdpt, unsigned int heap_end);
#endif