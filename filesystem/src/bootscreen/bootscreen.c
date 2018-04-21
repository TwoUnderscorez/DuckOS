#include "../../lib/libduck.h"
#include "../../lib/string.h"
void main(void);

void main(void) {
    char * file_buff = malloc(0x2000);
    char * path = malloc(sizeof(char)*50);
    char itoar_buff[3];
    memcpy(path, "/boot/frames/frame", 19);
    path[19] = '\0';
    unsigned int i, j;
    for(i = 1; i < 10; i++) {
        screen_clear();
        itoa(i, &itoar_buff, 10);
        path[18] = itoar_buff[0];
        load_file(path_to_inode(path), 0, 0, file_buff);
        file_buff[1575] = '\0';
        puts(file_buff);
        j = 0;
        while(j++<30000000);
    }
    for(; i < 20; i++) {
        screen_clear();
        itoa(i, &itoar_buff, 10);
        memcpy(&path[17], &itoar_buff, 2);
        load_file(path_to_inode(path), 0, 0, file_buff);
        file_buff[1575] = '\0';
        puts(file_buff);
        j = 0;
        while(j++<30000000);
    }
    getc();
    screen_clear();
}