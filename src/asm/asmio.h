#ifndef ASMIO_H
#define ASMIO_H

extern unsigned char in_byte ( unsigned short port );
extern void out_byte ( unsigned short port , unsigned char data );
extern unsigned short in_word ( unsigned short port );
extern void out_word ( unsigned short port , unsigned short data );
extern void asmcli(void);
extern void asmsti(void);

#endif