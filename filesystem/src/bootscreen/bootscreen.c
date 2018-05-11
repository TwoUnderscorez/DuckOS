#include "../../lib/libduck.h"
#include "../../lib/string.h"
void main(void);

void main(void) {
    char * file_buff = malloc(0x2000);
    char * path = malloc(sizeof(char)*50);
    char itoar_buff[3];
    // Prepare the path
    memcpy(path, "/boot/frames/frame", 19);
    path[19] = '\0';
    unsigned int i, j, k;
    // Do 1 to 9
    for(i = 1; i < 10; i++) {
        screen_clear();
        itoa(i, &itoar_buff, 10);
        path[18] = itoar_buff[0];
        load_file(path_to_inode(path), 0, 0, file_buff);
        file_buff[1575] = '\0'; // Make sure there is a null terminaltor
        k=0;
        while(file_buff[k]){
            if(k==6) set_screen_bgfg(0x1E); // Highlight `DuckOS`
            if(k==0) set_screen_bgfg(0xE1);
            putc(file_buff[k]);
            k++;
        }
        j = 0;
        while(j++<25000000);
    }
    // Do 10 to 19
    for(; i < 20; i++) {
        screen_clear();
        itoa(i, &itoar_buff, 10);
        memcpy(&path[17], &itoar_buff, 2);
        load_file(path_to_inode(path), 0, 0, file_buff);
        file_buff[1575] = '\0';
        k=0;
        while(file_buff[k]){
            if(k==6) set_screen_bgfg(0x1E);
            if(k==0) set_screen_bgfg(0xE1);
            putc(file_buff[k]);
            k++;
        }
        j = 0;
        while(j++<25000000);
    }
    getc();
    set_screen_bgfg(0x07); // Retorun to normal colors
}