#ifndef SCREEN_H
#define SCREEN_H

#define SCREEN_MAX_INT_CHAR_LEN (14)

void screen_clear();
void putc(char c);
void puts(char *string);
void screen_set_cursor_position(int row, int col);
void screen_enable_cursor();
void screen_set_ymax(unsigned char y);
void screen_print_int(int i, int base);
void screen_set_bgfg(unsigned char bgfg);
void screen_scroll();
#endif
