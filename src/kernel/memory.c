#include "memory.h"
#include "multiboot.h"
#include "../drivers/screen.h"

page_directory_pointer_table_entry_t page_dir_ptr_tab[4] __attribute__((aligned(0x20)));
page_directory_table_entry_t page_dir[512] __attribute__((aligned(0x1000)));
page_table_entry_t page_tab[512] __attribute__((aligned(0x1000)));
multiboot_info_t * mbd;

void init_memory(multiboot_info_t * mymbd) {
    mbd = mymbd;
    page_dir_ptr_tab[0].present = 1;
    page_dir_ptr_tab[0].page_directory_table_address = (unsigned int)&page_dir>>12;
    page_dir[0].present = 1;
    page_dir[0].ro_rw = 1;
    page_dir[0].size = 1;
    page_dir[0].page_table_address =0;// (unsigned int)&page_tab>>12;
    page_dir[1].present = 1;
    page_dir[1].ro_rw = 1;
    page_dir[1].page_table_address = (unsigned int)&page_tab>>12;
    unsigned int i, address = 0;
    for(i = 0; i < 512; i++)
    {
        page_tab[i].present = 1;
        page_tab[i].ro_rw = 1;
        page_tab[i].physical_page_address = address>>12;
        address = address + 0x1000;
    }
    enablePaePagingAsm(); 
    loadPageDirectoryAsm(&page_dir_ptr_tab);
    enablePagingAsm();
}

void dump_mmap(void) {
    multiboot_memory_map_t * mmap_ptr = mbd->mmap_addr;
    while(mmap_ptr < (mbd->mmap_addr + mbd->mmap_length) ){
        puts("base: 0x");
        screen_print_int(mmap_ptr->addr, 16);
        puts(" length: 0x");
        screen_print_int(mmap_ptr->len, 16);
        puts(" size: 0x");
        screen_print_int(mmap_ptr->size, 16);
        puts(" type: 0x");
        screen_print_int(mmap_ptr->type, 16);
        puts("\n");
        mmap_ptr = (multiboot_memory_map_t *)((unsigned int)mmap_ptr + sizeof(multiboot_memory_map_t));
    }
}