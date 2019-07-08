#include "ext2.h"
#include "atapio.h"
#include "screen.h"
#include "../kernel/heap.h"
#include "../libs/math.h"
#include "../libs/string.h"
#include "../drivers/keyboard.h"
#include "../kernel/task.h"

EXT2_SUPERBLOCK_t *ext2_superblock = 0;

#pragma region determining stuff
/**
 * @brief Get EXT2 block size
 * 
 * @return unsigned int block size in sectors
 */
static inline unsigned int ext2_get_block_size()
{
    return (1024 << ext2_superblock->LOG2_BLOCK_SIZE) / ATAPIO_SECTOR_SIZE;
}
/**
 * @brief Convert a block count to sector count for ata
 * 
 * @param block_count 
 * @return unsigned int 
 */
static inline unsigned int ext2_block_to_sector_count(unsigned int block_count)
{
    return block_count * ext2_get_block_size();
}
/**
 * @brief Get the size of a buffer required to load a file
 * (padded to fill an EXT2 block)
 * 
 * @param inode 
 * @return unsigned int size in bytes
 */
static unsigned int ext2_get_block_file_size_in_bytes(
    EXT2_INODE_t *inode)
{
    if (!inode)
        return 0;

    if (inode->size_low % (ext2_get_block_size() * ATAPIO_SECTOR_SIZE) == 0)
    {
        return inode->size_low;
    }
    else
    {
        return (inode->size_low / ext2_get_block_size() + 1) *
               ext2_get_block_size();
    }
}
/**
 * @brief Basically ext2_get_block_file_size_in_bytes() / ext2 block size
 * 
 * @param inode 
 * @return unsigned int 
 */
static inline unsigned int ext2_get_file_size_in_blocks(
    EXT2_INODE_t *inode)
{
    return ext2_get_block_file_size_in_bytes(inode) / ext2_get_block_size();
}
static int determine_the_number_of_block_groups()
{
    /* Rounding up the total number of blocks 
     * divided by the number of blocks per block group 
     */
    int num1 = cell((double)ext2_superblock->BLOCK_NUM / ext2_superblock->NOF_BLOCKS_IN_BLOCK_GROUP);
    /* Rounding up the total number of inodes
     * divided by the number of inodes per block group 
     */
    int num2 = cell((double)ext2_superblock->INODE_NUM / ext2_superblock->NOF_INODES_IN_BLOCK_GROUP);
    if (num1 - num2)
    {
        puts("\nInvalid EXT2 superblock!\n");
        __asm__("int $0x08");
    }
    return num1;
}

static int determine_inode_block_group(int inode)
{
    return (inode - 1) / ext2_superblock->NOF_INODES_IN_BLOCK_GROUP;
}

static int determine_index_of_inode_in_inode_table(int inode)
{
    return (inode - 1) % ext2_superblock->NOF_INODES_IN_BLOCK_GROUP;
}

static int determine_block_of_inode(int inode)
{
    int index = determine_index_of_inode_in_inode_table(inode);
    return (index * ext2_superblock->SIZE_OF_INODE) / 1024 << ext2_superblock->LOG2_BLOCK_SIZE;
}

static int determine_block_group_addr(int block_group_num)
{
    return ext2_superblock->NOF_BLOCKS_IN_BLOCK_GROUP * (unsigned int)block_group_num + EXT2_BLOCK_GROUP_DESCRIPTOR_OFFSET;
}
#pragma endregion

#pragma region convertions
static int ext2_block_to_lba(int block_num)
{
    return EXT2_PARTITION_START + block_num * 2;
}

static int lba_to_ext2_block(int lba)
{
    return (lba - EXT2_PARTITION_START) / 2;
}
#pragma endregion

static void load_superblock(int sb_addr)
{
    ext2_superblock = malloc(sizeof(EXT2_SUPERBLOCK_t));
    atapio_read_sectors(ext2_block_to_lba(sb_addr), 2, (char *)ext2_superblock);
}

static void verify_superblock()
{
    if (ext2_superblock->EXT2_SIGNATURE == 0xEF53)
    {
        return;
    }
    puts("\nInvalid FS or EXT2 superblock!\n");
    __asm__("int $0x08");
}

void print_fs_info()
{
    puts("{signature : ");
    screen_print_int(ext2_superblock->EXT2_SIGNATURE, 16);
    puts("; version : ");
    screen_print_int(ext2_superblock->VERSION_MAJOR, 10);
    puts(".");
    screen_print_int(ext2_superblock->VERSION_MINOR, 10);
    puts("; volume name : ");
    if (ext2_superblock->CSTR_VOLUME_NAME)
    {
        puts((char *)ext2_superblock->CSTR_VOLUME_NAME);
    }
    puts("; NOF block groups : ");
    screen_print_int(determine_the_number_of_block_groups(), 10);
    puts("; block size : ");
    screen_print_int(1024 << ext2_superblock->LOG2_BLOCK_SIZE, 10);
    puts("; First non-reserved inode in file system : ");
    screen_print_int(ext2_superblock->FIRST_NRSV_INODE_NUM, 10);
    puts("; inode size : ");
    screen_print_int(ext2_superblock->SIZE_OF_INODE, 10);
    puts("; inodes in block group : ");
    screen_print_int(ext2_superblock->NOF_INODES_IN_BLOCK_GROUP, 10);
    puts("}\n");
}

/**
 * @brief Load the block group discriptor into a buffer
 * 
 * @param block_group_num 
 * @param buffer sizeof(ATAPIO_SECTOR_SIZE)
 * @return EXT2_BLOCK_GROUP_DESCRIPTOR_t* 
 */
static EXT2_BLOCK_GROUP_DESCRIPTOR_t *
load_block_group_descriptor(
    int block_group_num,
    EXT2_BLOCK_GROUP_DESCRIPTOR_t *buffer)
{
    int addr = determine_block_group_addr(block_group_num);
    atapio_read_sectors(ext2_block_to_lba(addr), 1, (char *)buffer);
    return buffer;
}

static void print_block_group_descriptor(int block_group_num)
{
    EXT2_BLOCK_GROUP_DESCRIPTOR_t *blkgp = 0;
    blkgp = malloc(ATAPIO_SECTOR_SIZE);
    blkgp = load_block_group_descriptor(block_group_num, blkgp);
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

/**
 * @brief Load an inode structure into memory
 * 
 * @param inode_num 
 * @param buf sizeof(EXT2_INODE_t)
 * @return EXT2_INODE_t* 
 */
EXT2_INODE_t *ext2_load_inode(int inode_num, void *buf)
{
    EXT2_INODE_t *inode_table = 0;
    EXT2_BLOCK_GROUP_DESCRIPTOR_t *blkgp = 0;
    EXT2_INODE_t *ret_inode = (EXT2_INODE_t *)buf;
    int block_group = 0, index = 0, addr = 0;
    unsigned int inode_table_size = 0;

    // get the inode's block_group and index in the inode table
    block_group = determine_inode_block_group(inode_num);
    index = determine_index_of_inode_in_inode_table(inode_num);

    inode_table_size = (ATAPIO_SECTOR_SIZE *
                        (((unsigned int)index *
                          sizeof(EXT2_INODE_t)) /
                             512 +
                         1));

    // alloc enough memory for the inode table
    inode_table = malloc(inode_table_size);
    if (!inode_table)
        goto _cleanup;

    blkgp = malloc(ATAPIO_SECTOR_SIZE);
    if (!blkgp)
        goto _cleanup_inode_table;

    blkgp = load_block_group_descriptor(block_group, blkgp);

    // get the inode table addr from the block group descriptor
    addr = determine_block_group_addr(block_group) +
           blkgp->inode_table_addr - 2;

    // read just enough of the inode table
    atapio_read_sectors(
        ext2_block_to_lba(addr),                  // start lba
        (index * sizeof(EXT2_INODE_t)) / 512 + 1, // sector count
        (char *)inode_table);                     // buffer

    if (!ret_inode)
        goto _cleanup_blkgp;

    // copy over the requested inode
    memcpy(
        (void *)ret_inode,           // the caller's buffer
        (void *)&inode_table[index], // inode in loaded inode table
        sizeof(EXT2_INODE_t));       // sizeof inode

_cleanup_blkgp:
    free(blkgp);
_cleanup_inode_table:
    free(inode_table);
_cleanup:
    return ret_inode;
}

void ext2_print_filesystem(int inode_num, int tab_count)
{
    EXT2_DIRECTORY_ENTRY_t *dirinfo, *dirinfo_bak;
    EXT2_INODE_t *inode = malloc(sizeof(EXT2_INODE_t));
    puts("/\n");
    char *item_name_buff = malloc(40);
    unsigned char entry_count = 0;
    dirinfo_bak = dirinfo = malloc(inode->size_low);
    ext2_load_file(inode, 0, 0, dirinfo_bak);
    do
    {
        inode = ext2_load_inode(dirinfo->inode, inode);
        for (int i = 0; i < tab_count; i++)
            puts("\t");
        screen_print_int(dirinfo->inode, 10);
        puts(" ");
        memcpy(item_name_buff, &dirinfo->name_ptr, dirinfo->name_len);
        *(char *)((unsigned int)item_name_buff + dirinfo->name_len) = 0;
        puts(item_name_buff);
        // if we found a dir print it's contents
        if (((inode->type_prem & 0xF000) == 0x4000) && entry_count > 1)
        {
            if (getc() == 0xa)
            {
                ext2_print_filesystem(dirinfo->inode, tab_count + 1);
            }
            else
            {
                puts("\n");
            }
        }
        else
        {
            puts("\n");
        }
        dirinfo = (EXT2_DIRECTORY_ENTRY_t *)((unsigned int)dirinfo + (unsigned int)dirinfo->size);
        entry_count++;
    } while (dirinfo->size > 8);
    free((void *)dirinfo_bak);
    free((void *)inode);
    free((void *)item_name_buff);
}

/**
 * @brief Load a file into memory
 * 
 * @param inode ptr to inode struct
 * @param len max len of file to load, 0 to load whole file
 * @param offset file offset to start at
 * @param buff sizeof(inode->size_low)
 */
void ext2_load_file(
    EXT2_INODE_t *inode,
    unsigned int len,
    unsigned int offset,
    void *buff)
{
    unsigned int i = 0,
                 size_in_blocks = 0,
                 safe_len = 0,
                 safe_offset = 0;

    void *file_buff = 0;

    if (!inode)
        goto _cleanup_all;

    // Make sure that the offset + len isn't more than the file size.
    safe_offset = (unsigned int)min((int)offset, inode->size_low);
    if (len)
        safe_len = (unsigned int)min(len, inode->size_low - safe_offset);
    else
        safe_len = inode->size_low;

    // If they are, return without reading anything.
    if (inode->size_low < (safe_offset + safe_len))
        goto _cleanup_all;

    file_buff = malloc(ext2_get_block_file_size_in_bytes(inode));
    if (!file_buff)
        goto _cleanup_file_buff;

    // Don't support indirect block pointers yet.
    size_in_blocks = max(
        ext2_get_file_size_in_blocks(inode),
        EXT2_INODE_DIRECT_BLOCK_PTR_COUNT);

    for (i = 0;
         i < size_in_blocks && inode->direct_block_pointers[i];
         i++)
    {
        atapio_read_sectors(
            ext2_block_to_lba(inode->direct_block_pointers[i]),
            ext2_get_block_size(),
            (char *)((unsigned int)file_buff +
                     (ext2_get_block_size() * i)));
    }

    memcpy(
        (void *)((unsigned int)buff + safe_offset),
        file_buff,
        safe_len);

_cleanup_all:
_cleanup_file_buff:
    if (!file_buff)
    {
        free(file_buff);
        file_buff = 0;
    }
}

/**
 * @brief Find the inode a path is pointing to
 * 
 * @param partial_path 
 * @return int 
 */
int ext2_path_to_inode(char *partial_path)
{
    char **split_path = 0;
    char *full_path = 0;
    int inode = EXT2_ROOT_DIR_INODE_NUM, i = 1;
    EXT2_INODE_t *tmp_inode_ptr = 0;
    EXT2_DIRECTORY_ENTRY_t *dir = 0, *dir_bak = 0;

    if (!strcmp("/", partial_path))
        return EXT2_ROOT_DIR_INODE_NUM;
    if (partial_path[0] != '/')
        return -1;

    full_path = malloc(sizeof(char) * (strlen(partial_path) + 2));
    if (!full_path)
        goto _cleanup_full_path;

    strcpy(full_path, partial_path);
    strcat(full_path, "/");

    split_path = strsplit(full_path, '/');

    tmp_inode_ptr = malloc(sizeof(EXT2_INODE_t));
    if (!tmp_inode_ptr)
        goto _cleanup_tmp_inode_ptr;

    while (split_path[i])
    {
        tmp_inode_ptr = ext2_load_inode(inode, tmp_inode_ptr);
        if ((tmp_inode_ptr->type_prem & 0xF000) != 0x4000)
        {
            break; // if we've hit a file, exit
        }

        // assuming inode is a dir, load it.
        dir = malloc(tmp_inode_ptr->size_low);
        dir_bak = dir;
        if (!dir_bak)
            goto _cleanup_dir_bak;
        ext2_load_file(tmp_inode_ptr, 0, 0, dir);

        // Skip . and ..
        // TODO: remove this skip
        dir = (EXT2_DIRECTORY_ENTRY_t *)((unsigned int)dir +
                                         (unsigned int)dir->size);
        dir = (EXT2_DIRECTORY_ENTRY_t *)((unsigned int)dir +
                                         (unsigned int)dir->size);

        // enumerate inode's dirinfo to find the inode for `inode/item`
        do
        {
            if (memcmp(split_path[i], (char *)&dir->name_ptr, dir->name_len))
            {
                // If the names are not equal, got to the next dir entry.
                dir = (EXT2_DIRECTORY_ENTRY_t *)((unsigned int)dir +
                                                 (unsigned int)dir->size);
            }
            else
            {
                // Found a match!
                inode = dir->inode;
                break;
            }
        } while (dir->size > 8);

        if (!dir_bak)
        {
            free(dir_bak);
            dir_bak = 0;
        }

        if (inode != dir->inode)
        {
            // path does not exist
            inode = -1;
            goto _cleanup_all;
        }
        i++;
    }

_cleanup_all:
    // _cleanup_split_path:
    if (!split_path)
    {
        i = 0;
        for (i = 0; split_path[i]; i++)
        {
            free(split_path[i]);
        }
        split_path = 0;
    }
_cleanup_dir_bak:
    if (!dir_bak)
    {
        free(dir_bak);
        dir_bak = 0;
    }
_cleanup_tmp_inode_ptr:
    if (!tmp_inode_ptr)
    {
        free(tmp_inode_ptr);
        tmp_inode_ptr = 0;
    }
_cleanup_full_path:
    if (!full_path)
    {
        free(full_path);
        full_path = 0;
    }
    return inode;
}

void ext2_init_fs()
{
    load_superblock(EXT2_SUPERBLOCK_OFFSET);
    verify_superblock();
}