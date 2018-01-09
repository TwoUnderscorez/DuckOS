#include "memory.h"
#include "kernel.h"
#include "../drivers/screen.h"

unsigned long long page_dir_ptr_tab[4] __attribute__((aligned(0x20)));

page_directory_table_entry_t page_dir[512] __attribute__((aligned(0x1000)));
page_table_entry_t page_tab[512] __attribute__((aligned(0x1000)));

//unsigned long long page_dir[512] __attribute__((aligned(0x1000)));
//unsigned long long page_tab[512] __attribute__((aligned(0x1000)));

void init_memory() {
    page_dir_ptr_tab[0] = (unsigned long long)&page_dir | 1;
    unsigned long long * ptr1;
    ptr1 = &page_dir;
    *ptr1 = (unsigned long long)0;
    unsigned long long * ptr2;
    ptr2 = &page_tab[0];
    *ptr2 = (unsigned long long)0;

    //page_dir[0] = (unsigned long long)&page_tab | 3;
    ///*
    page_dir[0].present = 1;
    page_dir[0].ro_rw = 1;
    page_dir[0].page_table_address = (unsigned int)&page_tab>>12;//*/
    puts("\n");
    screen_print_int(&page_tab, 2);
    puts("\n");
    screen_print_int(*ptr1, 2);
    puts("\n");
    //*ptr1 = (unsigned long long)&page_tab | 3;
    screen_print_int(*ptr1, 2);
    puts("\n");
    unsigned int i, address = 0;
    for(i = 0; i < 512; i++)
    {
        //*((unsigned long long *)((unsigned int)ptr2 + (unsigned int)(8*i))) = address | 3; 
        //page_tab[i] = address | 3; // map address and mark it present/writable
        
        page_tab[i].present = 1;
        page_tab[i].ro_rw = 1;
        page_tab[i].physical_page_address = address>>12;
        address = address + 0x1000;
    }
    __asm__("movl %cr4, %eax; bts $5, %eax; movl %eax, %cr4"); // set bit5 in CR4 to enable PAE		 
    __asm__("movl %%eax, %%cr3" :: "a" (&page_dir_ptr_tab)); // load PDPT into CR3
    __asm__("movl %cr0, %eax; orl $0x80000000, %eax; movl %eax, %cr0;");
}