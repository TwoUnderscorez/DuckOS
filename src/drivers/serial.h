#ifndef SERIAL_H
#define SERIAL_H

#define SERIAL_COM1_PORT 0x3F8
#define SERIAL_COM2_PORT 0x2F8

#define SERIAL_COM1_DATA_REGISTER_PORT (SERIAL_COM1_PORT + 0)
#define SERIAL_COM1_INTERRUPT_ENABLE_PORT (SERIAL_COM1_PORT + 1)
#define SERIAL_COM1_INTERRUPT_DIVISOR_LOW (SERIAL_COM1_PORT + 0)
#define SERIAL_COM1_INTERRUPT_DIVISOR_HIGH (SERIAL_COM1_PORT + 1)
#define SERIAL_COM1_INT_ID_FIFO_CTRL_PORT (SERIAL_COM1_PORT + 2)
#define SERIAL_COM1_LINE_CTRL_REG_PORT (SERIAL_COM1_PORT + 3)
#define SERIAL_COM1_MODEM_CTRL_REG_PORT (SERIAL_COM1_PORT + 4)
#define SERIAL_COM1_LINE_STATUS_REG_PORT (SERIAL_COM1_PORT + 5)
#define SERIAL_COM1_MODEM_STATUS_REG_PORT (SERIAL_COM1_PORT + 6)
#define SERIAL_COM1_SCRATCH_REG_PORT (SERIAL_COM1_PORT + 7)

#define SERIAL_COM2_DATA_REGISTER_PORT (SERIAL_COM2_PORT + 0)
#define SERIAL_COM2_INTERRUPT_ENABLE_PORT (SERIAL_COM2_PORT + 1)
#define SERIAL_COM2_INTERRUPT_DIVISOR_LOW (SERIAL_COM2_PORT + 0)
#define SERIAL_COM2_INTERRUPT_DIVISOR_HIGH (SERIAL_COM2_PORT + 1)
#define SERIAL_COM2_INT_ID_FIFO_CTRL_PORT (SERIAL_COM2_PORT + 2)
#define SERIAL_COM2_LINE_CTRL_REG_PORT (SERIAL_COM2_PORT + 3)
#define SERIAL_COM2_MODEM_CTRL_REG_PORT (SERIAL_COM2_PORT + 4)
#define SERIAL_COM2_LINE_STATUS_REG_PORT (SERIAL_COM2_PORT + 5)
#define SERIAL_COM2_MODEM_STATUS_REG_PORT (SERIAL_COM2_PORT + 6)
#define SERIAL_COM2_SCRATCH_REG_PORT (SERIAL_COM2_PORT + 7)

void serial_init();
char serial_getc();
void serial_putc(char c);
void serial_puts(char *string);
#endif