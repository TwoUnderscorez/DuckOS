#include "../../lib/libduck.h"
#include "../../lib/string.h"
int main(int argc, char **argv);
void phexDump(char *desc, void *addr, int len, int offset);

int main(int argc, char **argv)
{
    int len = 0, offset = 0;
    puts("hexdump\n");
    if (argc == 4)
    {
        len = atoi(argv[2]);
        offset = atoi(argv[3]);
    }
    if (argc > 1)
    {
        int inode_num = path_to_inode(argv[1]);
        EXT2_INODE_t *inode = malloc(sizeof(EXT2_INODE_t));
        load_inode(inode_num, inode);
        void *file_buff = malloc(inode->size_low);
        load_file(inode, len, offset, file_buff);
        if (len == 0)
            len = inode->size_low;
        phexDump(argv[1], file_buff, len, offset);
    }
}

void phexDump(char *desc, void *addr, int len, int offset)
{
    int i, j = 0;
    char buff[17], buff2[10];
    unsigned char *pc = (unsigned char *)addr;

    // Output description if given.
    if (desc)
        puts(desc);
    puts(":");

    if (!len)
    {
        puts("  ZERO LENGTH\n");
        return;
    }
    if (len < 0)
    {
        puts("  NEGATIVE LENGTH\n");
        return;
    }
    // Process every byte in the data.
    for (i = 0; i < len; i++)
    {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0)
        {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                puts(buff);
            puts("\n");
            j++;
            if (j == 24)
            {
                putc(' ');
                screen_set_bgfg(0x70);
                puts("--MORE--");
                screen_set_bgfg(0x07);
                getc();
                puts("\b\b\b\b\b\b\b\b\b\b");
                j = 0;
            }

            // Output the offset.
            // printf ("  %04x ", i);
            itoa(i + offset, buff2, 16);
            puts(buff2);
            puts(": ");
        }

        // Now the hex code for the specific character.
        itoa(pc[i], buff2, 16);
        puts(buff2);
        puts(" ");

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0)
    {
        puts("   ");
        i++;
    }

    // And print the final ASCII bit.
    puts("  ");
    puts(buff);
    puts("\n");
}