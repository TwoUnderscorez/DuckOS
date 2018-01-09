#include "memory.h"
#include "kernel.h"
#include "../drivers/screen.h"

page_directory_pointer_table_entry_t page_dir_ptr_tab[4] __attribute__((aligned(0x20)));
page_directory_table_entry_t page_dir[512] __attribute__((aligned(0x1000)));
page_table_entry_t page_tab[512] __attribute__((aligned(0x1000)));

void init_memory() {
    page_dir_ptr_tab[0].present = 1;
    page_dir_ptr_tab[0].page_directory_table_address = (unsigned int)&page_dir>>12;
    page_dir[0].present = 1;
    page_dir[0].ro_rw = 1;
    page_dir[0].page_table_address = (unsigned int)&page_tab>>12;
    unsigned int i, address = 0;
    for(i = 0; i < 512; i++)
    {
        page_tab[i].present = 1;
        page_tab[i].ro_rw = 1;
        page_tab[i].physical_page_address = address>>12;
        address = address + 0x1000;
    }
    loadPageDirectoryAsm(&page_dir_ptr_tab);	
    enablePaePagingAsm(); 
    enablePagingAsm();
}