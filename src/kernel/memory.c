#include "memory.h"
#include "multiboot.h"
#include "../drivers/screen.h"

page_directory_pointer_table_entry_t page_dir_ptr_tab[4] __attribute__((aligned(0x20)));
page_directory_table_entry_t page_dir[512] __attribute__((aligned(0x1000)));
page_table_entry_t page_tab[512] __attribute__((aligned(0x1000)));
multiboot_info_t * mbd;
unsigned int startframe;
unsigned char frame_map[512];
unsigned int pre_frame_map[20];

void init_memory(multiboot_info_t * mymbd) {
    mbd = mymbd;
    startframe = 0x300000;
    // Map a 2 mb single page using an entry in the page directory table (0-0x200000)
    page_dir_ptr_tab[0].present = 1;
    page_dir_ptr_tab[0].page_directory_table_address = (unsigned int)&page_dir>>12;
    page_dir[0].present = 1;
    page_dir[0].ro_rw = 1;
    page_dir[0].size = 1;
    page_dir[0].page_table_address =0;
    // Map 512 4kb pages (0x200000-0x400000)
    page_dir[1].present = 1;
    page_dir[1].ro_rw = 1;
    page_dir[1].page_table_address = (unsigned int)&page_tab>>12;
    unsigned int i, address = 0;
    for(i = 0; i < 512; i++)
    {
        page_tab[i].present = 1;
        page_tab[i].ro_rw = 1;
        page_tab[i].physical_page_address = address>>12;
        page_tab[i].kernel_user = 0;
        address = address + 0x1000;
    }
    puts("Enabaling PAE paging...\n");
    enablePaePagingAsm(); 
    loadPageDirectoryAsm((unsigned int *)&page_dir_ptr_tab);
    enablePagingAsm();
}

void dump_mmap(void) {
    multiboot_memory_map_t * mmap_ptr = (multiboot_memory_map_t *)mbd->mmap_addr;
    while((unsigned int)mmap_ptr < (unsigned int)(mbd->mmap_addr + mbd->mmap_length) ){
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

static unsigned int kalloc_frame_int()
{
    unsigned int i = 0;
    // Search of an unused frame
    while(frame_map[i] != 0)
    {
        i++;
        if(i == 512)
        {
            // Return a null pointer if there were none found
            return(0);
        }
    }
    // Set frame to used status
    frame_map[i] = 1;
    // Return the physical address
    return(startframe + (i*0x1000));
}

unsigned int kalloc_frame()
{
    // Whether or not we are going to allocate a new set of pre-allocatd page frames
    static unsigned char allocate = 1;
    // How many pre-allocated page frames we have used
    static unsigned char pframe = 0;
    // Physical address of the page frame we are about to return
    unsigned int ret;

    if(pframe == 20) // If we are out of pre-allocatd page frames
    {
        allocate = 1;
    }

    if(allocate == 1)
    {
        // Allocate 20 page frames and save their physical addresses
        for(int i = 0; i<20; i++)
        {
            pre_frame_map[i] = (unsigned int)kalloc_frame_int();
            if(!pre_frame_map[i]) // Check for nulls (out of page frames)
                break;
        }
        // Reset values
        pframe = 0;
        allocate = 0;
    }
    ret = (unsigned int)pre_frame_map[pframe];
    pframe++;
    return(ret);
}

void kfree_frame(unsigned int page_frame_addr)
{
    // get the offset from the first frame
    page_frame_addr = (unsigned int)(page_frame_addr - startframe); 
    // Divide by 4kb to get the index of the page frame in frame_map
    frame_map[((unsigned int)page_frame_addr)/0x1000] = 0;
}