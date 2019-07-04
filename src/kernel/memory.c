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
multiboot_info_t *mbd;
unsigned char frame_map[131072];
unsigned int pre_frame_map[20];
unsigned int max_ram;
unsigned int max_frameidx;

void init_memory(multiboot_info_t *mymbd)
{
    memset(&frame_map, '\0', 131072);
    mbd = mymbd;
    // Map a 2 mb single page using an entry in the page directory table (0-0x200000) * 3
    page_dir_ptr_tab[0]
        .present = 1;
    page_dir_ptr_tab[0].page_directory_table_address = (unsigned int)(&page_dir) >> 12;
    for (int i = 0; i < 3; i++)
    {
        page_dir[i].present = 1;
        page_dir[i].ro_rw = 1;
        page_dir[i].size = 1;
        page_dir[i].page_table_address = (i * 0x200000) >> 12;
        page_dir[i].kernel_user = 0;
    }
    puts("Enabaling PAE paging... ");
    enablePaePagingAsm();
    puts("OK!\nSetting CR3... ");
    (void)swapPageDirectoryAsm((unsigned int *)&page_dir_ptr_tab);
    // asm volatile("mov %0, %%cr3" ::"r"((unsigned int *)&page_dir_ptr_tab));
    puts("OK!\nEnabling paging... ");
    enablePagingAsm();
    puts("OK!\nApplying MMAP to the frame map...\n");
    apply_mmap_to_frame_map();
    max_frameidx = addr_to_frameidx(max_ram);
    puts("End of memory is at 0x");
    screen_print_int(max_ram, 16);
    puts(", that's ");
    screen_print_int(max_frameidx, 10);
    puts(" 4kb page frames.\n");
}

void dump_mmap(void)
{
    puts("Memory Map:\n");
    unsigned int tmp_max_ram;
    multiboot_memory_map_t *mmap_ptr = (multiboot_memory_map_t *)mbd->mmap_addr;
    while ((unsigned int)mmap_ptr < (unsigned int)(mbd->mmap_addr + mbd->mmap_length))
    {
        puts("Found {base: 0x");
        screen_print_int(mmap_ptr->addr, 16);
        puts(" length: 0x");
        screen_print_int(mmap_ptr->len, 16);
        puts(" size: 0x");
        screen_print_int(mmap_ptr->size, 16);
        puts(" type: ");
        switch (mmap_ptr->type)
        {
        case MULTIBOOT_MEMORY_AVAILABLE:
            puts("AVL}\n");
            break;
        case MULTIBOOT_MEMORY_BADRAM:
            puts("BADRAM}\n");
            break;
        case MULTIBOOT_MEMORY_NVS:
            puts("NVS}\n");
            break;
        case MULTIBOOT_MEMORY_RESERVED:
            puts("RSV}\n");
            break;
        default:
            puts("?\n");
            break;
        }
        tmp_max_ram = mmap_ptr->addr + mmap_ptr->len;
        if (tmp_max_ram > max_ram)
            max_ram = tmp_max_ram;
        mmap_ptr = (multiboot_memory_map_t *)((unsigned int)mmap_ptr + sizeof(multiboot_memory_map_t));
    }
    puts("Kernel space {base: 0x100000 limit: 0x600000}\n");
    puts("VGA memory {base: 0xb8000 limit: 0xb8fa0}\n");
}

void apply_addr_to_frame_map(unsigned int base, unsigned int limit, unsigned char used)
{
    int limit_frame = addr_to_frameidx(limit), base_frame = addr_to_frameidx(base);
    for (; base_frame <= limit_frame; base_frame++)
    {
        if (used)
            bitmapSet((unsigned char *)&frame_map, base_frame);
        else
            bitmapReset((unsigned char *)&frame_map, base_frame);
    }
}

void load_kernel_pdpt()
{
    swapPageDirectoryAsm((unsigned int *)&page_dir_ptr_tab);
}

void apply_mmap_to_frame_map(void)
{
    unsigned int tmp_max_ram;
    multiboot_memory_map_t *mmap_ptr = (multiboot_memory_map_t *)mbd->mmap_addr;
    apply_addr_to_frame_map((unsigned int)mmap_ptr, (unsigned int)mmap_ptr + 1, 1);
    while ((unsigned int)mmap_ptr < (unsigned int)(mbd->mmap_addr + mbd->mmap_length))
    {
        puts("Found {base: 0x");
        screen_print_int(mmap_ptr->addr, 16);
        puts(" length: 0x");
        screen_print_int(mmap_ptr->len, 16);
        puts(" size: 0x");
        screen_print_int(mmap_ptr->size, 16);
        puts(" type: ");
        switch (mmap_ptr->type)
        {
        case MULTIBOOT_MEMORY_AVAILABLE:
            puts("AVL}\n");
            break;
        case MULTIBOOT_MEMORY_BADRAM:
            puts("BADRAM}, applying... ");
            apply_addr_to_frame_map(mmap_ptr->addr, mmap_ptr->addr + mmap_ptr->len, 1);
            puts("OK!\n");
            break;
        case MULTIBOOT_MEMORY_NVS:
            puts("NVS}, applying... ");
            apply_addr_to_frame_map(mmap_ptr->addr, mmap_ptr->addr + mmap_ptr->len, 1);
            puts("OK!\n");
            break;
        case MULTIBOOT_MEMORY_RESERVED:
            puts("RSV}, applying... ");
            apply_addr_to_frame_map(mmap_ptr->addr, mmap_ptr->addr + mmap_ptr->len, 1);
            puts("OK!\n");
            break;
        }
        tmp_max_ram = mmap_ptr->addr + mmap_ptr->len;
        if (tmp_max_ram > max_ram)
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
    while (bitmapGet((unsigned char *)frame_map, i))
    {
        i++;
        if (i == max_frameidx)
        {
            // Return a null pointer if there were none found
            return (0);
        }
    }
    // Set frame to used status
    bitmapSet((unsigned char *)frame_map, i);
    // Return the physical address
    return (i * 0x1000);
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

    if (pframe == 20) // If we are out of pre-allocatd page frames
    {
        allocate = 1;
    }

    if (allocate == 1)
    {
        // Allocate 20 page frames and save their physical addresses
        for (i = 0; i < 20; i++)
        {
            pre_frame_map[i] = (unsigned int)kalloc_frame_int();
            if (!pre_frame_map[i]) // Check for nulls (out of page frames)
                break;
        }
        // Reset values
        pframe = 0;
        allocate = 0;
    }
    ret = (unsigned int)pre_frame_map[pframe];
    pframe++;
    // asmcli();
    disablePagingAsm();
    memset((void *)ret, 0, 0x1000);
    enablePagingAsm();
    // asmsti();
    return (ret);
}

void kfree_frame(unsigned int page_frame_addr)
{
    // get the offset from the first frame
    page_frame_addr = (unsigned int)(page_frame_addr);
    // Divide by 4kb to get the index of the page frame in frame_map
    bitmapReset((unsigned char *)frame_map, ((unsigned int)page_frame_addr) / 0x1000);
}

// Create a page directory pointer table for a userland process
unsigned int create_pdpt()
{
    unsigned int task_pdpt, task_dt;
    // Get a page frame for each pdpt, dt, pt because their address
    // must be 0x1000 (4096) byte aligned.
    task_pdpt = kalloc_frame();
    task_dt = kalloc_frame();
    // We are entering a critical section
    // asmcli();
    // Temporarily disable paging so we can write to the physical
    // addresses of paging tables without problems
    disablePagingAsm();
    // Create the pointers
    page_directory_pointer_table_entry_t *temp_pdpt;
    temp_pdpt = (page_directory_pointer_table_entry_t *)task_pdpt;
    page_directory_table_entry_t *temp_dt;
    temp_dt = (page_directory_table_entry_t *)task_dt;
    // Map the kernel space
    temp_pdpt[0].page_directory_table_address = (unsigned int)temp_dt >> 12;
    temp_pdpt[0].present = 1;
    temp_pdpt[0].kernel_user = 1;
    temp_pdpt[0].ro_rw = 1;
    for (int i = 0; i < 3; i++)
    {
        temp_dt[i].present = 1;
        temp_dt[i].ro_rw = 1;
        temp_dt[i].size = 1;
        temp_dt[i].page_table_address = (i * 0x200000) >> 12;
        temp_dt[i].kernel_user = 0;
        temp_dt[i].always0 = 0;
        temp_dt[i].available = 0;
    }
    // Re-enable interrupts and paging
    enablePagingAsm();
    // asmsti();
    return task_pdpt;
}

void map_vaddr_to_pdpt(page_directory_pointer_table_entry_t *pdpt,
                       page_table_entry_t *data, unsigned int base,
                       unsigned int limit)
{
    // We are entering a critical section
    // asmcli();
    // Temporarily disable paging so we can write to the physical
    // addresses of paging tables without problems
    disablePagingAsm();
    unsigned int pdpt_idx, pd_idx, pt_idx;
    page_directory_table_entry_t *pdir;
    page_table_entry_t *ptab;
    for (; base <= limit; base += 0x1000)
    {
        // Find the indices of the vaddr in the pdpt, pd and pt.
        pdpt_idx = pd_idx = pt_idx = 0;
        pdpt_idx = (base >> 30) & 0b11;
        pd_idx = (base >> 21) & 0b00111111111;
        pt_idx = (base >> 12) & 0b00000000000111111111;
        // Create a page directory pointer table entry for a page directory table if it's not present.
        if (!pdpt[pdpt_idx].present)
        {
            pdpt[pdpt_idx].page_directory_table_address = (unsigned int)kalloc_frame() >> 12;
            pdpt[pdpt_idx].present = 1;
            pdpt[pdpt_idx].ro_rw = 1;
            pdpt[pdpt_idx].kernel_user = 1;
        }
        // Create a page directory table entry for a page table if it's not present.
        pdir = (page_directory_table_entry_t *)(pdpt[pdpt_idx].page_directory_table_address << 12);
        if (!pdir[pd_idx].present)
        {
            pdir[pd_idx].page_table_address = (unsigned int)kalloc_frame() >> 12;
            pdir[pd_idx].present = 1;
            pdir[pd_idx].ro_rw = 1;
            pdir[pd_idx].kernel_user = 1;
        }
        ptab = (page_table_entry_t *)(pdir[pd_idx].page_table_address << 12);
        // Copy the data to the appropriate index in the page table.
        if (!ptab[pt_idx].present)
        {
            data->physical_page_address = kalloc_frame() >> 12;
            memcpy(&ptab[pt_idx], data, sizeof(page_table_entry_t));
        }
    }
    // Re-enable interrupts and paging
    enablePagingAsm();
    // asmsti();
}

int addr_to_frameidx(unsigned int addr)
{
    return addr / 0x1000;
}

unsigned int frameidx_to_addr(int frameidx)
{
    return frameidx * 0x1000;
}

void dump_frame_map(void)
{
    unsigned int i;
    int used_frames = 0;
    for (i = 0; i < 131072 * 8; i++)
    {
        if (bitmapGet(frame_map, i))
        {
            used_frames += 1;
        }
    }
    puts("Used page frames: ");
    screen_print_int(used_frames, 10);
    puts(" of ");
    screen_print_int(max_frameidx, 10);
    puts(" (");
    screen_print_int(max_ram / 1000000, 10);
    puts("MB), that's about %");
    screen_print_int(used_frames * 100 / max_frameidx, 10);
    puts(".\n");
}

void brk(page_directory_pointer_table_entry_t *pdpt, unsigned int heap_end)
{
    page_table_entry_t *data = malloc(sizeof(page_table_entry_t));
    memset((void *)data, '\0', sizeof(page_table_entry_t));
    data->kernel_user = 1;
    data->present = 1;
    data->ro_rw = 1;
    map_vaddr_to_pdpt(pdpt, data, heap_end + 1, heap_end + 2); // another page
    free(data);
}