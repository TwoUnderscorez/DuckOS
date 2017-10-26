#include "keyboard.h"
#include "asmio.h"

//key down only
char scanducks[181] =
{
  0,0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b' /* <- Backspace */,
  '\t' /* <- Tab */, 'q', 'w', 'e', 'r',  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n' /* <- Enter key */, 
   0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0 /*<- Left shift */,
  '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,	 /*<- Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
	0,	/* All other keys are undefined */ 
};

char getc(){
    char c;
    c = scanducks[get_scanduck_from_keyboard()];
    return c;
}

int get_key() {
	return -1;
}

char get_scanduck_from_keyboard() {
	char c=in_byte(0x60), ex=0;
	do {			
		ex = in_byte(0x60);
		if(ex!=c) {		    //if what I pressed a split second ago != what I pressed now
			c=ex;		    //get scan code
			if(c>0)
				return c;   //return c
		}
	}while(1);
}

/* 
Keys:
One, Two, Three, Four, Five, Six, Seven, Eight, Nine, Zero, Minus, Equals, Backspace, Tab, q, w, e, r,t, y, u, i, o, p, OpeningSquareBracket, ClosingSquareBracket, Return, a, s, d, f, g, h, j, k, Apostrophy, BackTick, LShift, BackSlash, z, x, c, v, b, n, m, Comma, Dot, ForwardSlash, KeyPad_Atrisk,Alt, Space, CapsLock, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, NumLock, ScrollLock, Home, UpArrow, PageUp, KeyPad_Minus, LeftArrow, RightArrow, KeyPad_Plus, End, DownArrow, PageDown, Insert, Delete, F11, F12
*/