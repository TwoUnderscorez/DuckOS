#include "../../lib/libduck.h"
#include "../../lib/string.h"

void main(int argc, char **argv);
void print_filesystem(int inode_num);
void print_filesystem_r(int inode_num, int tab_count);

void main(int argc, char **argv)
{
    unsigned int i = 0;
    if (argc < 2)
        return;
    if (!strcmp(argv[1], "-r") && argc > 2)
    {
        print_filesystem_r(path_to_inode(argv[2]), 0);
    }
    else
    {
        print_filesystem(path_to_inode(argv[1]));
    }
    return;
}

void print_filesystem(int inode_num)
{
    putc('a');
    EXT2_DIRECTORY_ENTRY_t *dirinfo = 0, *dirinfo_bak = 0;
    EXT2_INODE_t *inode = 0, *dirinode = 0;
    unsigned int i = 0;

    putc('a');
    inode = malloc(sizeof(EXT2_INODE_t));
    dirinode = malloc(sizeof(EXT2_INODE_t));

    putc('a');
    load_inode(inode_num, dirinode);

    putc('a');
    dirinfo = dirinfo_bak = malloc(dirinode->size_low);
    putc('a');
    load_file(dirinode, 0, 0, dirinfo);
    do
    {
        load_inode(dirinfo->inode, inode);
        screen_print_int(dirinfo->inode, 10);
        putc(' ');
        screen_print_int(inode->size_low, 10);
        puts("B ");
        for (i = 0; i < dirinfo->name_len; i++)
            putc(((char *)&dirinfo->name_ptr)[i]);
        puts("\n");
        dirinfo = (EXT2_DIRECTORY_ENTRY_t *)((unsigned int)dirinfo +
                                             (unsigned int)dirinfo->size);
    } while (dirinfo->size > 8);
    free(dirinfo_bak);
    free(inode);
    free(dirinode);
}

void print_filesystem_r(int inode_num, int tab_count)
{
    EXT2_DIRECTORY_ENTRY_t *dirinfo = 0, *dirinfo_bak = 0;
    EXT2_INODE_t *inode = 0, *dirinode = 0;
    unsigned char entry_count = 0, i = 0;

    inode = malloc(sizeof(EXT2_INODE_t));
    dirinode = malloc(sizeof(EXT2_INODE_t));

    load_inode(inode_num, dirinode);
    dirinfo = dirinfo_bak = malloc(dirinode->size_low);

    puts("/\n");

    load_file(dirinode, 0, 0, dirinfo);
    do
    {
        load_inode(dirinfo->inode, inode);
        for (i = 0; i < tab_count; i++)
            puts("\t");
        screen_print_int(dirinfo->inode, 10);
        puts(" ");
        for (i = 0; i < dirinfo->name_len; i++)
            putc(((char *)&dirinfo->name_ptr)[i]);
        // if we found a dir print it's contents
        if (((inode->type_prem & 0xF000) == 0x4000) && entry_count > 1)
        {
            if (getc() == 0x0A)
            {
                print_filesystem_r(dirinfo->inode, tab_count + 1);
            }
        }
        else
        {
            puts("\n");
        }
        dirinfo = (EXT2_DIRECTORY_ENTRY_t *)((unsigned int)dirinfo +
                                             (unsigned int)dirinfo->size);
        entry_count++;
    } while (dirinfo->size > 8);
    free(dirinfo_bak);
    free(inode);
    free(dirinode);
}