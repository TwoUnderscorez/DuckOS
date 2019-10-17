#include "logger.h"
#include "../drivers/serial.h"

void klogI(char *message)
{
    serial_puts("kmode-info/");
    serial_puts(message);
}

void klogW(char *message)
{
    serial_puts("kmode-warn/");
    serial_puts(message);
}

void klogE(char *message)
{
    serial_puts("kmode-err/");
    serial_puts(message);
}