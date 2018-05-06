#include "screen.h"
#include "../asm/asmio.h"
#include "../libs/string.h"

unsigned char *vidmem=(unsigned char *)0xb8000;
unsigned char screen_bgfg = 0x07;
unsigned char x=0,y=0, ymax=25;

void set_screen_bgfg(unsigned char bgfg) {
    screen_bgfg = bgfg;
}

// Clears the screen
void screen_clear(){
	int i;
	for(i=0;i<4000;i+=2){
		vidmem[i]=0x20;
		vidmem[i+1]=screen_bgfg;
	}
	x=0;
	y=0;
	set_cursor_position(0, 0);
}

// Scrolls the screen
void screen_scroll(){
	int i = 5000000;
    // while(i) --i;
	for(i=160;i<160*ymax;i++){
		vidmem[i-160]=vidmem[i];
	}
    for(i=160*(ymax-1);i<160*ymax;i+=2) {
		vidmem[i]=0x20;
		vidmem[i+1]=screen_bgfg;
	}
}

void set_ymax(unsigned char y) {
    ymax = y;
}

// Prints a char to the screen
void putc(char c) {
    if(y==ymax){
        screen_scroll();
        y=ymax-1;
    }
    if(c>0x1f&&c!=0x7f){
        vidmem[2*(x+y*80)]=c;
        vidmem[2*(x+y*80)+1]=screen_bgfg;
        x++;
        if(x==80){
            y++;
            x=0;
        }
    }
    else{
        switch(c){
        case 0x08: // \b
            if(x>0){
                x--;
                vidmem[2*(x+y*80)]=' ';
            } else if (x==0 && y>0) {
                x=80;
                y--;
            }
            break;
        case 0x09: // \t
            x+=4;
            if(x>=80){
                x=0;
                y++;
            }
            break;
        case 0x0d: // \r
            x=0;
            break;
        case 0x0a: // \n
            x=0;
            y++;
            break;
        }
    }
    if(y<24) 
        set_cursor_position(y+1, x);
    else
        set_cursor_position(y, x);
}

// Prints a string te screen
void puts(char *string) {
	int i=0;
	while(string[i]){
		putc(string[i]);
        i++;
	}
}

// Sets the cursor position
void set_cursor_position(int row, int col) {
    unsigned short position = ( row*80) + col;				//calc posotion and save it in a WORD
 
    // cursor LOW port to vga INDEX register
    out_byte(0x3D4, 0x0F);
    out_byte(0x3D5, (unsigned char)(position&0xFF));		//get low BYTE of WORD
    // cursor HIGH port to vga INDEX register
    out_byte(0x3D4, 0x0E);
    out_byte(0x3D5, (unsigned char)((position>>8)&0xFF));	//get high BYTE of WORD
 }

 void enable_cursor() { 
    out_byte(0x3D4, 0x0A);
    out_byte(0x3D5, (in_byte(0x3D5) & 0xC0));
    out_byte(0x3D4, 0x0E);
    out_byte(0x3D5, (in_byte(0x3D5) & 0xC0));
 }

// Print an int to the screen
void screen_print_int(int i,int base){
	char buf[100];
	itoa(i,buf,base);
	puts(buf);
}

