#include "../../lib/libduck.h"
void main(void);

void main(void) {
    char * file_buff = malloc(0x2000);
    char * path = malloc(sizeof(char)*50);
    char itoar_buff[3];
    memcpy(path, "/boot/bootscreen/frames/frame", 29);
    path[30] = '\0';
    unsigned int i, j;
    for(i = 0; i < 9; i++) {
        screen_clear();
        itoa(i, &itoar_buff, 10);
        path[29] = itoar_buff[0];
        load_file(path_to_inode(path), 0, 0, file_buff);
        file_buff[1575] = '\0';
        puts(file_buff);
        j = 0;
        // puts(path);
        while(j++<30000000);
    }
    for(; i < 20; i++) {
        screen_clear();
        itoa(i, &itoar_buff, 10);
        memcpy(&path[28], &itoar_buff, 2);
        load_file(path_to_inode(path), 0, 0, file_buff);
        file_buff[1575] = '\0';
        puts(file_buff);
        j = 0;
        while(j++<30000000);
    }
    getc();
    screen_clear();
}