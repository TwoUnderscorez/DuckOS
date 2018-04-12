#include "ext2.h"
#include "atapio.h"
#include "screen.h"
#include "../kernel/heap.h"
#include "../libs/math.h"
#include "../libs/string.h"

EXT2_SUPERBLOCK_t * ext2_superblock = 0;

#pragma region determining stuff
static int determine_the_number_of_block_groups() {
    /* Rounding up the total number of blocks 
     * divided by the number of blocks per block group 
     */
    int num1 = cell((double)ext2_superblock->BLOCK_NUM/ext2_superblock->NOF_BLOCKS_IN_BLOCK_GROUP);
    /* Rounding up the total number of inodes
     * divided by the number of inodes per block group 
     */
    int num2 = cell((double)ext2_superblock->INODE_NUM/ext2_superblock->NOF_INODES_IN_BLOCK_GROUP);
    if (num1 - num2) {
        puts("\nInvalid EXT2 superblock!\n");
        __asm__("int $0x08");
    }
    return num1;
} 

static int determine_inode_block_group(int inode) {
    return (inode - 1) / ext2_superblock->NOF_INODES_IN_BLOCK_GROUP;
}

static int determine_index_of_inode_in_inode_table(int inode) {
    return (inode - 1) % ext2_superblock->NOF_INODES_IN_BLOCK_GROUP;
}

static int determine_block_of_inode(int inode) {
    int index = determine_index_of_inode_in_inode_table(inode);
    return ( index * ext2_superblock->SIZE_OF_INODE) / 1024<<ext2_superblock->LOG2_BLOCK_SIZE;
}

static int determine_block_group_addr(int block_group_num) {
    return ext2_superblock->NOF_BLOCKS_IN_BLOCK_GROUP * (unsigned int)block_group_num + EXT2_BLOCK_GROUP_DESCRIPTOR_OFFSET;
}
#pragma endregionc

#pragma region convertions
static int ext2_block_to_lba(int block_num){
    return EXT2_PARTITION_START + block_num * 2;
}

static int lba_to_ext2_block(int lba){
    return (lba - EXT2_PARTITION_START)/2;
}
#pragma endregion

static void load_superblock(int sb_addr) {
    ext2_superblock = malloc(sizeof(EXT2_SUPERBLOCK_t));
    ata_read_sectors(ext2_block_to_lba(sb_addr), 2, (char *)ext2_superblock);
}

static void verify_superblock() {
    if(ext2_superblock->EXT2_SIGNATURE == 0xEF53) {
        return;
    }
    puts("\nInvalid FS or EXT2 superblock!\n");
    __asm__("int $0x08");
}

void print_fs_info() {
    puts("{signature : ");
    screen_print_int(ext2_superblock->EXT2_SIGNATURE, 16);
    puts("; version : ");
    screen_print_int(ext2_superblock->VERSION_MAJOR, 10);
    puts(".");
    screen_print_int(ext2_superblock->VERSION_MINOR, 10);
    puts("; volume name : ");
    if(ext2_superblock->CSTR_VOLUME_NAME){
        puts((char *)ext2_superblock->CSTR_VOLUME_NAME);
    }
    puts("; NOF block groups : ");
    screen_print_int(determine_the_number_of_block_groups(), 10);
    puts("; block size : ");
    screen_print_int(1024<<ext2_superblock->LOG2_BLOCK_SIZE, 10);
    puts("; First non-reserved inode in file system : ");
    screen_print_int(ext2_superblock->FIRST_NRSV_INODE_NUM, 10);
    puts("; inode size : ");
    screen_print_int(ext2_superblock->SIZE_OF_INODE, 10);
    puts("; inodes in block group : ");
    screen_print_int(ext2_superblock->NOF_INODES_IN_BLOCK_GROUP, 10);
    puts("}\n");
}

static EXT2_BLOCK_GROUP_DESCRIPTOR_t * load_block_group_descriptor(int block_group_num) {
    EXT2_BLOCK_GROUP_DESCRIPTOR_t * ext2_gp_desc = malloc(512);
    if(!ext2_gp_desc) puts("null");
    memset((void *)ext2_gp_desc, '\0', sizeof(512));
    int addr = determine_block_group_addr(block_group_num);
    ata_read_sectors(ext2_block_to_lba(addr), 1, (char *)ext2_gp_desc);
    return ext2_gp_desc;
}

static void print_block_group_descriptor(int block_group_num) {
    EXT2_BLOCK_GROUP_DESCRIPTOR_t * blkgp = load_block_group_descriptor(block_group_num);
    puts("Group ");
    screen_print_int(block_group_num, 10);
    puts(" : ");
    puts("{inode table : ");
    screen_print_int(blkgp->inode_table_addr, 10);
    puts("; nof dirs : ");
    screen_print_int(blkgp->nof_dirs_in_group, 10);
    puts("}\n");
    free(blkgp);
}

EXT2_INODE_t * load_inode(int inode_num) {
    // get the inode's block_group and index in the inode table
    int block_group = determine_inode_block_group(inode_num);
    int index = determine_index_of_inode_in_inode_table(inode_num);
    // alloc enough memory for the inode table
    EXT2_INODE_t * inode_table = malloc(512*((index*sizeof(EXT2_INODE_t))/512 + 1));
    memset((void *)inode_table, '\0', 512*((index*sizeof(EXT2_INODE_t))/512 + 1));
    if(!inode_table) puts("null");
    EXT2_BLOCK_GROUP_DESCRIPTOR_t * blkgp = load_block_group_descriptor(block_group);
    // get the inode table addr from the block group descriptor
    int addr = determine_block_group_addr(block_group) + blkgp->inode_table_addr - 2;
    // read just enough of the inode table
    ata_read_sectors(ext2_block_to_lba(addr), (index*sizeof(EXT2_INODE_t))/512 + 1, (char *)inode_table);
    // alloc mem for a single inode
    EXT2_INODE_t * ret_inode = malloc(sizeof(EXT2_INODE_t));
    if(!ret_inode) puts("null");
    // copy over the requested inode
    memcpy((void *)ret_inode, (void *)&inode_table[index], sizeof(EXT2_INODE_t));
    // free the table and the descriptor and return the inode
    free((void *)inode_table);
    free((void *)blkgp);
    return ret_inode;
}

static EXT2_DIRECTORY_ENTRY_t * load_directory_structure(int inode_num) {
    // get the dir's inode
    EXT2_INODE_t * inode = load_inode(inode_num);
    EXT2_DIRECTORY_ENTRY_t * dirinfo = malloc(2*inode->size_low);
    //                                        ^ idk why
    if(!dirinfo) puts("null");
    memset((void *)dirinfo, '\0', 2*inode->size_low);
    int i;
    // read the contents
    for(i = 0; i < inode->size_low/(1024<<ext2_superblock->LOG2_BLOCK_SIZE); i++) {
        ata_read_sectors( ext2_block_to_lba(inode->direct_block_pointers[i]), 
                          (1024<<ext2_superblock->LOG2_BLOCK_SIZE)/ATA_SECTOR_SIZE, 
                          (char *)( (unsigned int)dirinfo + (1024<<ext2_superblock->LOG2_BLOCK_SIZE)*i) );
        if (i>10) break;
    }
    free((void *)inode);
    return dirinfo;
}

static void print_dir_info(int inode_num) {
    EXT2_DIRECTORY_ENTRY_t * dirinfo = load_directory_structure(inode_num);
    while(dirinfo->size > 8) {
        puts("inode num: ");
        screen_print_int(dirinfo->inode, 10);
        puts("; name: ");
        puts((char *)&dirinfo->name_ptr);
        puts("\n");
        dirinfo = (EXT2_DIRECTORY_ENTRY_t *)((unsigned int)dirinfo + (unsigned int)dirinfo->size); 
    }
    free((void *)dirinfo);
    puts("\n");
}

static void print_inode_info(int inode_num) {
    EXT2_INODE_t * inode = load_inode(inode_num);
    puts("{size : ");
    screen_print_int(inode->size_low, 10);
    puts("; hard links : ");
    screen_print_int(inode->hard_links, 10);
    puts("; inode num : ");
    screen_print_int(inode_num, 10);
    puts("; type : ");
    screen_print_int(inode->type_prem, 16);
    puts("}\n");
    free((void *)inode);
}

void print_filesystem(int inode_num, int tab_count) {
    EXT2_DIRECTORY_ENTRY_t * dirinfo;
    EXT2_INODE_t * inode;
    puts("/\n");
    unsigned char entry_count = 0;
    dirinfo = load_directory_structure(inode_num);
    char in;
    do {
        inode = load_inode(dirinfo->inode);
        for(int i = 0; i<tab_count; i++) puts("\t");
        screen_print_int(dirinfo->inode, 10);
        puts(" ");
        puts((char *)&dirinfo->name_ptr);
        // if we found a dir print it's contents
        if (( (inode->type_prem & 0xF000) == 0x4000) && entry_count > 1)
        {
            if(getc()==0xa){
                print_filesystem(dirinfo->inode, tab_count+1);
            }
            else{
                puts("\n");
            }
        } 
        else{
            puts("\n");
        }
        dirinfo = (EXT2_DIRECTORY_ENTRY_t *)((unsigned int)dirinfo + (unsigned int)dirinfo->size); 
        entry_count++;
    } while(dirinfo->size > 8);
    free((void *)dirinfo);
    free((void *)inode);
}

void load_file(int inode_num, int seek, int skip, void * buff) {
    EXT2_INODE_t * inode = load_inode(inode_num);
    if(!inode) return;
    int i;
    for(i = 0; i < 12; i++){
        if(inode->direct_block_pointers[i]){
            ata_read_sectors( ext2_block_to_lba(inode->direct_block_pointers[i]), 
                              (1024<<ext2_superblock->LOG2_BLOCK_SIZE)/ATA_SECTOR_SIZE, 
                              (char *)( (unsigned int)buff + (1024<<ext2_superblock->LOG2_BLOCK_SIZE)*i) );
        }
        else {
            break;
        }
    }
    free(inode);
}

void init_ext2fs() {
    load_superblock(EXT2_SUPERBLOCK_OFFSET);
    verify_superblock();   
}