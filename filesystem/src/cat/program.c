#include "../../lib/libduck.h"
void main(int argc, char **argv);

void main(int argc, char **argv)
{
    int inode_num = 0;
    char *file_buff = 0;
    EXT2_INODE_t *inode = malloc(sizeof(EXT2_INODE_t));
    for (int i = 1; i < argc; i++)
    {
        puts(argv[i]);
        puts(":\n");
        inode_num = path_to_inode(argv[i]);
        load_inode(inode_num, inode);
        file_buff = (char *)malloc(inode->size_low);
        load_file(inode, 0, 0, file_buff);
        for (int i = 0; i < inode->size_low; i++)
        {
            // If it's a printable character, print it
            if (file_buff[i] > 0x1F || file_buff[i] == 0x0A)
            {
                putc(file_buff[i]);
            }
            else
            { // If not, print a dot
                putc('.');
            }
        }
        free(file_buff);
    }
}