#include "../../lib/libduck.h"
void main(int argc, char ** argv);

void main(int argc, char ** argv) {
    if(argc > 1) {
        puts(argv[1]);
        puts(":\n");
        int inode_num = path_to_inode(argv[1]);
        EXT2_INODE_t * inode = malloc(sizeof(EXT2_INODE_t));
        load_inode(inode_num, inode);
        char * file_buff = (char *)malloc((inode->sector_usage+1)*512);
        load_file(inode_num, 0, 0, file_buff);
        for(int i = 0; i < inode->size_low; i++) {
            if(file_buff[i] > 0x20){
                putc(file_buff[i]);
            }
            else{
                putc('.');
            }
        }
    }
}