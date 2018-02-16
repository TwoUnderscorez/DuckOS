#include "ext2.h"
#include "atapio.h"
#include "screen.h"
#include "../kernel/heap.h"
#include "../libs/math.h"

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
#pragma endregion

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
    ata_read_sectors(ext2_block_to_lba(sb_addr), 2, ext2_superblock);
}

static void verify_superblock() {
    if(ext2_superblock->EXT2_SIGNATURE == 0xEF53) {
        return;
    }
    puts("\nInvalid FS or EXT2 superblock!\n");
    __asm__("int $0x08");
}

static void print_fs_info() {
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
    int addr = determine_block_group_addr(block_group_num);
    ata_read_sectors(ext2_block_to_lba(addr), 1, ext2_gp_desc);
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

static void print_inode_info(int inode_num) {
    int block_group = determine_inode_block_group(inode_num);
    int index = determine_index_of_inode_in_inode_table(inode_num);
    EXT2_INODE_t * inode_table = malloc(512*(index/512 + 1));
    EXT2_BLOCK_GROUP_DESCRIPTOR_t * blkgp = load_block_group_descriptor(block_group);
    int addr = determine_block_group_addr(block_group) + blkgp->inode_table_addr - 2;
    screen_print_int(addr, 10);
    ata_read_sectors(ext2_block_to_lba(addr), index/512 + 1, inode_table);
    puts("{size : ");
    screen_print_int(inode_table[index].size_low, 10);
    puts("; hard links : ");
    screen_print_int(inode_table[index].hard_links, 10);
    puts("}\n");
    free(inode_table);
    free(blkgp);
}

void init_ext2fs() {
    load_superblock(EXT2_SUPERBLOCK_OFFSET);
    verify_superblock();
    puts("Found ext2 filesystem!\n");
    print_fs_info();
    int inode = 8066;
    print_block_group_descriptor(determine_inode_block_group(inode));
    puts("/: ");
    print_inode_info(inode);
}