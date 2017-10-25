#ifndef SCREEN_H
#define SCREEN_H

void screen_clear();
void screen_print_char(char c);
void screen_print(char* string);
void set_cursor_position(int row, int col);
char * itoa( int value, char * str, int base );
void screen_print_int(int i,int base);

#endif
