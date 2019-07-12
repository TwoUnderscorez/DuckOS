#include "../../lib/libduck.h"
#include "../../lib/string.h"
int main(void);

int main(void)
{
    EXT2_INODE_t *inode = 0;
    char *file = 0;
    char *path = 0;
    char itoar_buff[3];

    path = malloc(sizeof(char) * 50);
    inode = malloc(sizeof(EXT2_INODE_t));
    // Prepare the path
    memcpy(path, "/boot/frames/frame", 19);
    path[19] = '\0';
    unsigned int i, j, k;
    // Do 1 to 9
    for (i = 1; i < 10; i++)
    {
        screen_clear();
        itoa(i, (char *)&itoar_buff, 10);
        path[18] = itoar_buff[0];
        load_inode(path_to_inode(path), inode);
        file = malloc(inode->size_low);
        load_file(inode, 0, 0, file);

        file[1575] = '\0'; // Make sure there is a null terminaltor
        k = 0;
        while (file[k])
        {
            if (k == 6)
                screen_set_bgfg(0x1E); // Highlight `DuckOS`
            if (k == 0)
                screen_set_bgfg(0xE1);
            putc(file[k]);
            k++;
        }
        j = 0;
        while (j++ < 25000000)
            ;
        free(file);
    }
    // Do 10 to 19
    for (; i < 20; i++)
    {
        screen_clear();
        itoa(i, (char *)&itoar_buff, 10);
        memcpy(&path[17], &itoar_buff, 2);
        load_inode(path_to_inode(path), inode);
        load_file(inode, 0, 0, file);
        file[1575] = '\0';
        k = 0;
        while (file[k])
        {
            if (k == 6)
                screen_set_bgfg(0x1E);
            if (k == 0)
                screen_set_bgfg(0xE1);
            putc(file[k]);
            k++;
        }
        j = 0;
        while (j++ < 25000000)
            ;
    }
    getc();
    screen_set_bgfg(0x07); // Retorun to normal colors
}