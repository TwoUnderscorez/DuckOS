#ifndef KERNEL_H
#define KERNEL_H

void free_write();

struct multiboot_information_struct {
    unsigned char flags_mem_lower;
    unsigned char mem_upper_bootdev;
    unsigned char cmdline_mod_count;
    unsigned short mods_addr_syms;
    unsigned char mmap_len_addr;
    unsigned char drives_len_addr;
    unsigned char cfgtbl_bloader_name;
    unsigned char amptbl_vbe_ctrlinfo;
    unsigned char vbe_mode;

} __attribute__((packed));

typedef struct multiboot_information_struct multiboot_information_t;

#endif