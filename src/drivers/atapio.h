#ifndef ATAPIO_H
#define ATAPIO_H
#include "../kernel/pic.h"
#define ATAPIO_SECTOR_SIZE 512
// ATA bus I/O ports
#define ATAPIO_DATA_REGISTER_PORT 0x1F0
#define ATAPIO_ERR_INFO_PORT 0x1F0
#define ATAPIO_SECTOR_COUNT_REGISTER_PORT 0x1F2
#define ATAPIO_LBA_LOW_REGISTER_PORT 0x1F3
#define ATAPIO_LBA_MID_REGISTER_PORT 0x1F4
#define ATAPIO_LBA_HIGH_REGISTER_PORT 0x1F5
#define ATAPIO_DRIVE_REGISTER_PORT 0x1F6
#define ATAPIO_COMMAND_REGISTER_PORT 0x1F7
#define ATAPIO_REGULAR_STATUS_REGISTER_PORT 0x1F7
// ATA commands
#define ATAPIO_IDENTIFY_DEVICE 0xEC
#define ATAPIO_READ 0x20
#define ATAPIO_WRITE 0x30
// ATA IRQ Line
#define ATAPIO_IRQ_LINE PIC1_OFFSET + 0x0E

struct atapio_status_byte
{
    unsigned char ERR : 3; // Indicates an error occurred.
    unsigned char DRQ : 1; // Set when the drive has PIO data to transfer, or is ready to accept PIO data.
    unsigned char SRV : 1; // Overlapped Mode Service Request.
    unsigned char DF : 1;  // Drive Fault Error
    unsigned char RDY : 1; // Bit is clear when drive is spun down, or after an error. Set otherwise.
    unsigned char BSY : 1; // Indicates the drive is preparing to send/receive data
};

typedef struct atapio_status_byte atapio_status_byte_t;

struct atapio_disk_sector
{
    unsigned char bytes[32];
};

typedef struct atapio_disk_sector atapio_disk_sector_t;

void atapio_software_reset(unsigned short DCR);
void atapio_wait_for_rdy(unsigned short DCR);
void atapio_wait_for_drq(unsigned short DCR);
void atapio_read_sectors(unsigned int lba, char sector_count, char *buffer);
void atapio_write_sectors(int lba, char sector_count, char *buffer);
void atapio_irq_handler();
void atapio_io_delay();
#endif