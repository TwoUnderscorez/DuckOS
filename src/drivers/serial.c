#include "serial.h"
#include "../asm/asmio.h"

void serial_init()
{
    out_byte(
        SERIAL_COM1_INTERRUPT_ENABLE_PORT,
        0x00); // Disable all interrupts
    out_byte(
        SERIAL_COM1_LINE_CTRL_REG_PORT,
        0x80); // Enable DLAB (set baud rate divisor)
    out_byte(
        SERIAL_COM1_INTERRUPT_DIVISOR_LOW,
        0x03); // Set divisor to 3 (lo byte) 38400 baud
    out_byte(
        SERIAL_COM1_INTERRUPT_DIVISOR_HIGH,
        0x00); //                  (hi byte)
    out_byte(
        SERIAL_COM1_LINE_CTRL_REG_PORT,
        0x03); // 8 bits, no parity, one stop bit
    out_byte(
        SERIAL_COM1_INT_ID_FIFO_CTRL_PORT,
        0xC7); // Enable FIFO, clear them, with 14-byte threshold
    out_byte(
        SERIAL_COM1_MODEM_CTRL_REG_PORT,
        0x0B); // IRQs enabled, RTS/DSR set
}