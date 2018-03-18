#ifndef EXT2_H
#define EXT2_H

// Some consts
#define EXT2_PARTITION_START                2048
#define EXT2_SUPERBLOCK_OFFSET              1
#define EXT2_BLOCK_GROUP_DESCRIPTOR_OFFSET  2
#define EXT2_ROOT_DIR_INODE_NUM             2
// File System States 
#define EXT2_FS_STATE_CLEAN 1
#define EXT2_FS_STATE_ERR   2
// Error Handling Methods 
#define EXT2_ERR_HANDLE_METHOD_IGNORE       1
#define EXT2_ERR_HANDLE_METHOD_READ_ONLY    2
#define EXT2_ERR_HANDLE_METHOD_KERNEL_PANIC 3
// Creator Operating System IDs 
#define EXT2_CREATOR_OS_LINUX   0
#define EXT2_CREATOR_OS_GNU     1
#define EXT2_CREATOR_OS_MASIX   2
#define EXT2_CREATOR_OS_FREEBSD 3
#define EXT2_CREATOR_OS_OTHER   4

struct EXT2_SUPERBLOCK {
    unsigned int        INODE_NUM;
    unsigned int        BLOCK_NUM;
    unsigned int        SUPERUSER_RESV_BLOCKS;
    unsigned int        UNALLOCATED_BLOCKS;
    unsigned int        UNALLOCATED_INODES;
    unsigned int        SUPERBLOCK_BLOCK_NUM;
    unsigned int        LOG2_BLOCK_SIZE;
    unsigned int        LOG2_FRAG_SIZE;
    unsigned int        NOF_BLOCKS_IN_BLOCK_GROUP;
    unsigned int        NOF_FRAGS_IN_BLOCK_GROUP;
    unsigned int        NOF_INODES_IN_BLOCK_GROUP;
    unsigned int        LAST_MOUNT_TIME;
    unsigned int        LAST_WRITE_TIME;
    unsigned short      NOF_MOUNTS_SINCE_CCHECK;    // consistency check
    unsigned short      NOF_MOUNTS_B4_CCHECK;
    unsigned short      EXT2_SIGNATURE;             // 0xEF53
    unsigned short      FS_STATE;
    unsigned short      EXT2_ERR_HANDLING_METHOD;
    unsigned short      VERSION_MINOR;
    unsigned int        LAST_CCECK;
    unsigned int        CCHECK_INTERVAL;
    unsigned int        OS_ID;
    unsigned int        VERSION_MAJOR;
    unsigned short      USER_ID_THAT_CAN_USE_RSV_BLOCKS;
    unsigned short      GROUP_ID_THAT_CAN_USE_RSV_BLOCKS;
    unsigned int        FIRST_NRSV_INODE_NUM;
    unsigned short      SIZE_OF_INODE;
    unsigned short      SUPERBLOCK_BLOCK_GROUP;
    unsigned int        OPTIONAL_FEATURES;
    unsigned int        REQUIRED_FEATURES;
    unsigned int        REQUIRED_FEATURES_UNSUPPORTED_MOUNT_READ_ONLY;
    unsigned char       FS_ID[16];
    unsigned char       CSTR_VOLUME_NAME[16];
    unsigned char       CSTR_MOUNT_PATH[64];
    unsigned int        COMPRESSION_ALGORITHMS;
    unsigned char       NOF_FILE_BLOCK_PREALLOC;
    unsigned char       NOF_DIR_BLOCK_PREALLOC;
    unsigned short      RESV1;
    unsigned char       JOURNAL_ID[16];
    unsigned int        JOURNAL_INODE;
    unsigned int        JOURNAL_DEV;
    unsigned int        HEAD_OF_ORPHAN_INODE_LIST;
    unsigned char       RESV2[787];
} __attribute__((packed));

typedef struct EXT2_SUPERBLOCK EXT2_SUPERBLOCK_t;

struct EXT2_BLOCK_GROUP_DESCRIPTOR {
    unsigned int    block_usage_bitmap;
    unsigned int    inode_usage_bitmap;
    unsigned int    inode_table_addr;
    unsigned short  nof_unallocated_blocks_in_group;
    unsigned short  nof_unallocated_inodes_in_group;
    unsigned short  nof_dirs_in_group;
    unsigned char   resv[13];
} __attribute__((packed));

typedef struct EXT2_BLOCK_GROUP_DESCRIPTOR EXT2_BLOCK_GROUP_DESCRIPTOR_t;

struct EXT2_INODE {
    unsigned short  type_prem;
    unsigned short  user_id;
    unsigned int    size_low;
    unsigned int    last_access_time;
    unsigned int    creation_time;
    unsigned int    modification_time;
    unsigned int    deletion_time;
    unsigned short  group_id;
    unsigned short  hard_links;
    unsigned int    sector_usage;
    unsigned int    flags;
    unsigned int    OS_type;
    unsigned int    direct_block_pointers[12];
    unsigned int    singly_indirect_block_pointer;
    unsigned int    doubly_indirect_block_pointer;
    unsigned int    triply_indirect_block_pointer;
    unsigned int    generation_num;
    unsigned int    extended_attribute_block;
    unsigned int    size_high_dir_acl;
    unsigned int    block_addr_of_fragment;
    unsigned char   os_specific[12];
}__attribute__((packed));

typedef struct EXT2_INODE EXT2_INODE_t;

struct EXT2_DIRECTORY_ENTRY {
    unsigned int inode;
    unsigned short size;
    unsigned short name_len;
    unsigned char name_ptr;
}__attribute__((packed));

typedef struct EXT2_DIRECTORY_ENTRY EXT2_DIRECTORY_ENTRY_t;

void init_ext2fs();
void print_filesystem(int inode_num, int tab_count);
EXT2_INODE_t * load_inode(int inode_num);
void load_file(int inode_num, int seek, int skip, void * buff);

#endif