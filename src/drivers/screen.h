#ifndef SCREEN_H
#define SCREEN_H

void screen_clear();
void putc(char c);
void puts(char* string);
void set_cursor_position(int row, int col);
void screen_print_int(int i,int base);

#endif
