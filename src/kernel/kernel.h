#ifndef KERNEL_H
#define KERNEL_H

void free_write();

// A struct that represent the multiboot information passed as a parameter by GRUB. 
// Taken from 3.3 https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Specification
struct multiboot_information_struct {
    unsigned char flags : 4;
    unsigned char mem : 8;
    unsigned char boot_device : 4;
    unsigned char cmdline : 4;
    unsigned char mods_count : 4;
    unsigned char mods_attr : 4;
    unsigned short syms : 12;
    unsigned char mmap_lenght : 4;
    unsigned char mmap_addr : 4;
    unsigned char drives_length : 4;
    unsigned char drives_addr : 4;
    unsigned char config_table : 4;
    unsigned char bootloader_name : 4;
    unsigned char apm_table : 4;
    unsigned char vbe_control_info : 4;
    unsigned char vabe_mode_info : 4;
    unsigned char vbe_mode : 4;
    unsigned char vbe_interface_seg : 2;
    unsigned char vbe_interface_off : 2;
    unsigned char vbe_interface_len : 2;
    unsigned char framebuffer_addr : 2;
    unsigned char framebuffer_pitch : 8;
    unsigned char framebuffer_width : 4;
    unsigned char framebuffer_height : 4;
    unsigned char framebuffer_bpp : 4;
    unsigned char framebuffer_type : 1;
    unsigned char color_info : 6;
} __attribute__((__packed__));

typedef struct multiboot_information_struct multiboot_information_t;

#endif