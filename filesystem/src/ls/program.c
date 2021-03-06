#include "../../lib/libduck.h"
#include "../../lib/string.h"

int main(int argc, char **argv);
void print_filesystem(int inode_num);
void print_filesystem_r(int inode_num, int tab_count);

int main(int argc, char **argv)
{
    if (argc < 2)
        return 1;
    if (!strcmp(argv[1], "-r") && argc > 2)
    {
        print_filesystem_r(path_to_inode(argv[2]), 0);
    }
    else
    {
        print_filesystem(path_to_inode(argv[1]));
    }
    return 0;
}

void print_filesystem(int inode_num)
{
    EXT2_DIRECTORY_ENTRY_t *dirinfo = 0, *dirinfo_bak = 0;
    EXT2_INODE_t *inode = 0, *dirinode = 0;
    unsigned int i = 0, dirsiz_counter = 0;

    inode = malloc(sizeof(EXT2_INODE_t));
    dirinode = malloc(sizeof(EXT2_INODE_t));

    load_inode(inode_num, dirinode);

    dirinfo = dirinfo_bak = malloc(dirinode->size_low);
    load_file(dirinode, 0, 0, dirinfo);
    do
    {
        load_inode(dirinfo->inode, inode);
        screen_print_int(dirinfo->inode, 10);
        putc(' ');
        screen_print_int(inode->size_low, 10);
        puts("B ");
        for (i = 0; i < dirinfo->name_len + 1; i++)
            putc(((char *)&dirinfo->name_ptr)[i]);
        puts("\n");
        dirsiz_counter += dirinfo->size;
        dirinfo = (EXT2_DIRECTORY_ENTRY_t *)((unsigned int)dirinfo +
                                             (unsigned int)dirinfo->size);
    } while (dirsiz_counter < 1024);
    free(dirinfo_bak);
    free(inode);
    free(dirinode);
}

void print_filesystem_r(int inode_num, int tab_count)
{
    EXT2_DIRECTORY_ENTRY_t *dirinfo = 0, *dirinfo_bak = 0;
    EXT2_INODE_t *inode = 0, *dirinode = 0;
    unsigned int entry_count = 0,
                 dirsiz_counter = 0,
                 i = 0;

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
            putc('\t');
        screen_print_int(dirinfo->inode, 10);
        puts(" ");
        for (i = 0; i < dirinfo->name_len + 1; i++)
            putc(((char *)&dirinfo->name_ptr)[i]);
        // if we found a dir print it's contents
        if (((inode->type_prem & 0xF000) == 0x4000) && entry_count > 1)
        {
            if (getc() == 0x0A)
            {
                print_filesystem_r(dirinfo->inode, tab_count + 1);
            }
            else
            {
                putc('\n');
            }
        }
        else
        {
            puts("\n");
        }
        dirsiz_counter += dirinfo->size;
        dirinfo = (EXT2_DIRECTORY_ENTRY_t *)((unsigned int)dirinfo +
                                             (unsigned int)dirinfo->size);
        entry_count++;
    } while (dirsiz_counter < 1024);

    if (dirinfo_bak)
    {
        free(dirinfo_bak);
        dirinfo_bak = 0;
    }
    if (inode)
    {
        free(inode);
        inode = 0;
    }
    if (dirinode)
    {
        free(dirinode);
        dirinode = 0;
    }
}