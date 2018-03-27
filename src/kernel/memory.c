#include "memory.h"
#include "multiboot.h"
#include "heap.h"
#include "../drivers/screen.h"
#include "../asm/asmio.h"
#include "../libs/string.h"
#include "../libs/bitmap.h"
extern void usermain();

page_directory_pointer_table_entry_t page_dir_ptr_tab[4] __attribute__((aligned(0x20)));
page_directory_table_entry_t page_dir[512] __attribute__((aligned(0x1000)));
page_table_entry_t page_tab[512] __attribute__((aligned(0x1000)));
multiboot_info_t * mbd;
unsigned char frame_map[131072];
unsigned int pre_frame_map[20];
unsigned int max_ram;
unsigned int max_frameidx;

void init_memory(multiboot_info_t * mymbd) {
    memset(&frame_map, '\0', 131072);
    mbd = mymbd;
    // Map a 2 mb single page using an entry in the page directory table (0-0x200000) * 3
    page_dir_ptr_tab[0].present = 1;
    page_dir_ptr_tab[0].page_directory_table_address = (unsigned int)&page_dir>>12;
    page_dir[0].present = 1;
    page_dir[0].ro_rw = 1;
    page_dir[0].size = 1;
    page_dir[0].page_table_address = 0;
    page_dir[0].kernel_user = 0;
    page_dir[1].present = 1;
    page_dir[1].ro_rw = 1;
    page_dir[1].size = 1;
    page_dir[1].page_table_address = 0x200000>>12;
    page_dir[2].kernel_user = 0;
    page_dir[2].present = 1;
    page_dir[2].ro_rw = 1;
    page_dir[2].size = 1;
    page_dir[2].page_table_address = 0x400000>>12;
    page_dir[2].kernel_user = 0;
    ////////////////////////////////////////////////////////////////////////////
    // An unsuccessful attempt at making a Higher Half Kernel
    // unsigned int i, address = 0xB0000000;
    // puts("\n");
    // for(i = 388; i < 475; i++){
    //     screen_print_int(address>>12, 16);
    //     puts(" ");
    //     page_dir[i].present = 1;
    //     page_dir[i].ro_rw = 1;
    //     page_dir[i].size = 1;
    //     page_dir[i].page_table_address = (unsigned int)address>>12;
    //     screen_print_int(page_dir[i].page_table_address, 16);
    //     puts("\n");
    //     getc();
    //     page_dir[i].kernel_user = 0;
    //     address = (unsigned int)((unsigned int)address + (unsigned int)0x200000);
    // }
    puts("Enabaling PAE paging...\n");
    enablePaePagingAsm(); 
    loadPageDirectoryAsm((unsigned int *)&page_dir_ptr_tab);
    enablePagingAsm();
    puts("Applying MMAP to the frame map...\n");
    apply_mmap_to_frame_map();
    max_frameidx = addr_to_frameidx(max_ram);
    puts("End of memory is at 0x");
    screen_print_int(max_ram, 16);
    puts(", that's ");
    screen_print_int(max_frameidx, 10);
    puts(" 4kb page frames.\n");
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
        puts(" type: ");
        switch (mmap_ptr->type) {
            case MULTIBOOT_MEMORY_AVAILABLE:
                puts("AVAILABLE");
                break;
            case MULTIBOOT_MEMORY_BADRAM:
                puts("BADRAM");
                break;
            case MULTIBOOT_MEMORY_NVS:
                puts("NVS");
                break;
            case MULTIBOOT_MEMORY_RESERVED:
                puts("RESERVED");
                break;
        }
        puts("\n");
        mmap_ptr = (multiboot_memory_map_t *)((unsigned int)mmap_ptr + sizeof(multiboot_memory_map_t));
    }
}

void apply_addr_to_frame_map(unsigned int base, unsigned int limit, unsigned char used) {
    int limit_frame = addr_to_frameidx(limit), base_frame = addr_to_frameidx(base);
    for (; base_frame <= limit_frame; base_frame++) {
        if(used) bitmapSet((unsigned char *)&frame_map, base_frame);
        else bitmapReset((unsigned char *)&frame_map, base_frame);
    }
}

void apply_mmap_to_frame_map(void) {
    unsigned int tmp_max_ram;
    multiboot_memory_map_t * mmap_ptr = (multiboot_memory_map_t *)mbd->mmap_addr;
    while((unsigned int)mmap_ptr < (unsigned int)(mbd->mmap_addr + mbd->mmap_length) ){
        puts("Found {base: 0x");
        screen_print_int(mmap_ptr->addr, 16);
        puts(" length: 0x");
        screen_print_int(mmap_ptr->len, 16);
        puts(" size: 0x");
        screen_print_int(mmap_ptr->size, 16);
        puts(" type: ");
        switch (mmap_ptr->type) {
            case MULTIBOOT_MEMORY_AVAILABLE:
                puts("AVL}\n");
                break;
            case MULTIBOOT_MEMORY_BADRAM:
                puts("BADRAM}, applying... ");
                apply_addr_to_frame_map(mmap_ptr->addr,mmap_ptr->addr + mmap_ptr->len, 1);
                puts("OK!\n");
                break;
            case MULTIBOOT_MEMORY_NVS:
                puts("NVS}, applying... ");
                apply_addr_to_frame_map(mmap_ptr->addr,mmap_ptr->addr + mmap_ptr->len, 1);
                puts("OK!\n");
                break;
            case MULTIBOOT_MEMORY_RESERVED:
                puts("RSV}, applying... ");
                apply_addr_to_frame_map(mmap_ptr->addr,mmap_ptr->addr + mmap_ptr->len, 1);
                puts("OK!\n");
                break;
        }
        tmp_max_ram = mmap_ptr->addr + mmap_ptr->len;
        if(tmp_max_ram > max_ram)
            max_ram = tmp_max_ram;
        mmap_ptr = (multiboot_memory_map_t *)((unsigned int)mmap_ptr + sizeof(multiboot_memory_map_t));
    }
    puts("Kernel space {base: 0x100000 limit: 0x600000}, applying... ");
    puts("OK!\n");
    apply_addr_to_frame_map(0x100000, 0x600000, 1);
    puts("VGA memory {base: 0xb8000 limit: 0xb8fa0}, applying... ");
    apply_addr_to_frame_map(0xb8000, 0xb8fa0, 1);
    puts("OK!\n");
}

static unsigned int kalloc_frame_int()
{
    unsigned int i = 1;
    // Search of an unused frame
    while(bitmapGet((unsigned char *)frame_map, i))
    {
        i++;
        if(i == max_frameidx)
        {
            // Return a null pointer if there were none found
            return(0);
        }
    }
    // Set frame to used status
    bitmapSet((unsigned char *)frame_map, i);
    // Return the physical address
    return(i*0x1000);
}

unsigned int kalloc_frame()
{
    // Whether or not we are going to allocate a new set of pre-allocatd page frames
    static unsigned char allocate = 1;
    // How many pre-allocated page frames we have used
    static unsigned char pframe = 0;
    // Physical address of the page frame we are about to return
    unsigned int ret;
    int i;

    if(pframe == 20) // If we are out of pre-allocatd page frames
    {
        allocate = 1;
    }

    if(allocate == 1)
    {
        // Allocate 20 page frames and save their physical addresses
        for(i = 0; i<20; i++)
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
    page_frame_addr = (unsigned int)(page_frame_addr); 
    // Divide by 4kb to get the index of the page frame in frame_map
    bitmapReset((unsigned char *)frame_map, ((unsigned int)page_frame_addr)/0x1000);
}

// Create a page directory pointer table for a userland process
unsigned int create_pdpt() {
    unsigned int task_pdpt, task_dt;
    // Get a page frame for each pdpt, dt, pt because their address 
    // must be 0x1000 (4096) byte aligned.
    task_pdpt = kalloc_frame();
    task_dt = kalloc_frame();
    // We are entering a critical section
    asmcli();
    // Temporarily disable paging so we can write to the physical
    // addresses of paging tables without problems
    disablePagingAsm();
    // Create the pointers
    page_directory_pointer_table_entry_t * temp_pdpt;
    temp_pdpt = (page_directory_pointer_table_entry_t *)task_pdpt;
    page_directory_table_entry_t * temp_dt;
    temp_dt = (page_directory_table_entry_t *)task_dt;
    // Map the kernel space
    temp_pdpt[0].page_directory_table_address = (unsigned int)temp_dt>>12;
    temp_pdpt[0].present = 1;
    temp_dt[0].present = 1;
    temp_dt[0].ro_rw = 1;
    temp_dt[0].size = 1;
    temp_dt[0].page_table_address = 0;
    temp_dt[0].kernel_user = 0; 
    // Map user space (0x300000-0x301000)
    page_table_entry_t * data = malloc(sizeof(page_table_entry_t));
    data->present = 1;
    data->physical_page_address = kalloc_frame()>>12;
    data->ro_rw = 1;
    data->kernel_user = 1;
    map_vaddr_to_pdpt(temp_pdpt, data, 0x300000);
    // temp_dt[1].present = 1;
    // temp_dt[1].page_table_address = (unsigned int)temp_tab>>12;
    // temp_dt[1].kernel_user = 1;
    // temp_tab[256].present = 1;
    // temp_tab[256].ro_rw = 1;
    // unsigned int fr = kalloc_frame();
    // temp_tab[256].physical_page_address = fr>>12;
    // temp_tab[256].kernel_user = 1;
    // Re-enable interrupts and paging
    /* Temporary loading of the user task to the appropriate location
     * in memory. will be removed. 
     */
    memcpy((void *)(data->physical_page_address<<12), &usermain, 0x1000); // temp
    enablePagingAsm();
    asmsti();
    return task_pdpt;
}

void map_vaddr_to_pdpt(page_directory_pointer_table_entry_t * pdpt, 
                       page_table_entry_t * data, unsigned int vaddr)
{
    unsigned int pdpt_idx, pd_idx, pt_idx;
    page_directory_table_entry_t * pdir;
    page_table_entry_t * ptab;
    pdpt_idx = (vaddr>>29)&0b11;
    pd_idx   = (vaddr>>21)&0b00111111111;
    pt_idx   = (vaddr>>12)&0b00000000000111111111;
    if(!pdpt[pdpt_idx].present) {
        pdpt[pdpt_idx].page_directory_table_address = (unsigned int)kalloc_frame()>>12;
        pdpt[pdpt_idx].present = 1;
        pdpt[pdpt_idx].kernel_user = data->kernel_user;
    }
    pdir = (page_directory_table_entry_t *)(pdpt[pdpt_idx].page_directory_table_address<<12);
    if(!pdir[pd_idx].present) {
        pdir[pd_idx].page_table_address = (unsigned int)kalloc_frame()>>12;
        pdir[pd_idx].present = 1;
        pdir[pd_idx].kernel_user = data->kernel_user;
    }
    ptab = (page_table_entry_t *)(pdir[pd_idx].page_table_address<<12);
    memcpy(&ptab[pt_idx], data, sizeof(page_table_entry_t));
}

int addr_to_frameidx(unsigned int addr) {
    return addr/0x1000;
}

unsigned int frameidx_to_addr(int frameidx) {
    return frameidx*0x1000;
}

void dump_frame_map(void) {
    unsigned int i;
    int used_frames = 0;
    for(i = 0; i < 131072*8; i++) {
        if(bitmapGet(frame_map, i)){
            used_frames += 1;
        }
    }
    puts("Used page frames: ");
    screen_print_int(used_frames, 10);
    puts("\n");
}