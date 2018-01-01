#include "screen.h"
#include "../asm/asmio.h"
#include "../libs/string.h"

unsigned char *vidmem=(unsigned char *)0xb8000;

unsigned char x=0,y=0;

// Clears the screen
void screen_clear(){
	int i;
	for(i=0;i<4000;i+=2){
		vidmem[i]=0x20;
		vidmem[i+1]=0x07;
	}
	x=0;
	y=0;
	set_cursor_position(0, 0);
}

// Scrolls the screen
void screen_scroll(){
	int i;
	for(i=160;i<4000;i++){
		vidmem[i-160]=vidmem[i];
	}
    for(i=160*24;i<4000;i+=2) {
		vidmem[i]=0x20;
		vidmem[i+1]=0x07;
	}
}

// Prints a char to the screen
void putc(char c) {
    if(y==25){
        screen_scroll();
        y=24;
    }
    if(c>0x1f&&c!=0x7f){
        vidmem[2*(x+y*80)]=c;
        x++;
        if(x==80){
            y++;
            x=0;
        }
    }
    else{
        switch(c){
        case 0x08:
            if(x>0){
                x--;
                vidmem[2*(x+y*80)]=' ';
            } else if (x==0 && y>0) {
                x=80;
                y--;
            }
            break;
        case 0x09:
            x+=4;
            if(x>=80){
                x=0;
                y++;
            }
            break;
        case 0x0a:
            x=0;
            y++;
            break;
        }
    }
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

// Print an int to the screen
void screen_print_int(int i,int base){
	char buf[100];
	itoa(i,buf,base);
	puts(buf);
}

