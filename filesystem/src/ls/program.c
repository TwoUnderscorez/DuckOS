#include "../../lib/libduck.h"
#include "../../lib/string.h"
void main(int argc, char ** argv);
void print_filesystem(int inode_num);
void print_filesystem_r(int inode_num, int tab_count);

void main(int argc, char ** argv) {
    if(argc<2) return;
    if(!strcmp(argv[1], "-r") && argc > 2) {
        print_filesystem_r(path_to_inode(argv[2]), 0);
    }
    else {
        print_filesystem(path_to_inode(argv[1]));
    }
}

void print_filesystem(int inode_num) {
    EXT2_DIRECTORY_ENTRY_t * dirinfo, * dirinfo_bak;
    EXT2_INODE_t * inode = malloc(sizeof(EXT2_INODE_t)), * dirinode = malloc(sizeof(EXT2_INODE_t));
    load_inode(inode_num, dirinode);
    dirinfo = dirinfo_bak = malloc((dirinode->sector_usage + 1) * 512);
    unsigned int i;
    load_directory_structure(inode_num, dirinfo_bak);
    do {
        load_inode(dirinfo->inode, inode);
        screen_print_int(dirinfo->inode, 10);
        puts(" ");
        screen_print_int(inode->size_low, 10);
        puts("B ");
        for(i=0; i < dirinfo->name_len; i++) putc(((char *)&dirinfo->name_ptr)[i]);
        puts("\n");
        dirinfo = (EXT2_DIRECTORY_ENTRY_t *)((unsigned int)dirinfo + (unsigned int)dirinfo->size); 
    } while(dirinfo->size > 8);
    free((void *)dirinfo_bak);
    free((void *)inode);
}

void print_filesystem_r(int inode_num, int tab_count) {
    EXT2_DIRECTORY_ENTRY_t * dirinfo, * dirinfo_bak;
    EXT2_INODE_t * inode = malloc(sizeof(EXT2_INODE_t)), * dirinode = malloc(sizeof(EXT2_INODE_t));
    load_inode(inode_num, dirinode);
    dirinfo = dirinfo_bak = malloc((dirinode->sector_usage + 2) * 512);
    puts("/\n");
    unsigned char entry_count = 0, i = 0;
    load_directory_structure(inode_num, dirinfo);
    do {
        load_inode(dirinfo->inode, inode);
        for(i = 0; i<tab_count; i++) puts("\t");
        screen_print_int(dirinfo->inode, 10);
        puts(" ");
        for(i=0; i < dirinfo->name_len; i++) putc(((char *)&dirinfo->name_ptr)[i]);
        // if we found a dir print it's contents
        if (( (inode->type_prem & 0xF000) == 0x4000) && entry_count > 1)
        {
            if(getc()==0x0A){
                print_filesystem_r(dirinfo->inode, tab_count+1);
            }
        }
        else{
            puts("\n");
        }
        dirinfo = (EXT2_DIRECTORY_ENTRY_t *)((unsigned int)dirinfo + (unsigned int)dirinfo->size); 
        entry_count++;
    } while(dirinfo->size > 8);
    free((void *)dirinfo_bak);
    free((void *)inode);
}