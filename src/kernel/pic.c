#include "pic.h"
#include "../asm/asmio.h"

void PIC_sendEOI(unsigned char irq)
{
	if(irq >= 8)
		out_byte(PIC2_COMMAND,PIC_EOI);

	out_byte(PIC1_COMMAND,PIC_EOI);
}

void PIC_remap()
{
	unsigned char a1, a2;
 
	a1 = in_byte(PIC1_DATA);                      // save masks
	a2 = in_byte(PIC2_DATA);
 
	out_byte(PIC1_COMMAND, ICW1_INIT+ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	out_byte(PIC2_COMMAND, ICW1_INIT+ICW1_ICW4);
	io_wait();
	out_byte(PIC1_DATA, PIC1_OFFSET);             // ICW2: Master PIC vector offset
	io_wait();
	out_byte(PIC2_DATA, PIC2_OFFSET);             // ICW2: Slave PIC vector offset
	io_wait();
	out_byte(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	out_byte(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();
 
	out_byte(PIC1_DATA, ICW4_8086);
	io_wait();
	out_byte(PIC2_DATA, ICW4_8086);
	io_wait();
 
	out_byte(PIC1_DATA, a1);   // restore saved masks.
	out_byte(PIC2_DATA, a2);
}

void IRQ_set_mask(unsigned char IRQline) {
    unsigned short port;
    unsigned char value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = in_byte(port) | (1 << IRQline);
    out_byte(port, value);        
}
 
void IRQ_clear_mask(unsigned char IRQline) {
    unsigned short port;
    unsigned char value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = in_byte(port) & ~(1 << IRQline);
    out_byte(port, value);        
}