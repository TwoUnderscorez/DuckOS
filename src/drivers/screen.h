#ifndef SCREEN_H
#define SCREEN_H

void screen_clear();
void putc(char c);
void puts(char* string);
void set_cursor_position(int row, int col);
void enable_cursor() ;
void set_ymax(unsigned char y);
void screen_print_int(int i,int base);
void set_screen_bgfg(unsigned char bgfg);
void screen_scroll();
#endif
