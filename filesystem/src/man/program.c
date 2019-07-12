#include "../../lib/libduck.h"
#include "../../lib/string.h"
void main(int argc, char **argv);

void main(int argc, char **argv)
{
    char *path;
    if (argc > 1)
    {
        path = malloc(sizeof(char) * 100);
        strcpy(path, "/man/");
        strcat(path, argv[1]);
        strcat(path, ".man");
        int inode_num = path_to_inode(path);
        EXT2_INODE_t *inode = malloc(sizeof(EXT2_INODE_t));
        load_inode(inode, inode);
        char *file_buff = (char *)malloc(inode->size_low);
        load_file(inode, 0, 0, file_buff);
        for (int i = 0; i < inode->size_low; i++)
        {
            if (file_buff[i] > 0x1F || file_buff[i] == 0x0A)
            {
                putc(file_buff[i]);
            }
            else
            {
                putc('.');
            }
        }
    }
}