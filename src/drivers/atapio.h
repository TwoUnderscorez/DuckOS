#ifndef ATAPIO_H
#define ATAPIO_H
// ATA bus I/O ports
#define ATA_DATA_REGISTER_PORT           0x1F0
#define ATA_SECTOR_COUNT_REGISTER_PORT   0x1F2
#define ATA_LBA_LOW_REGISTER_PORT        0x1F3
#define ATA_LBA_MID_REGISTER_PORT        0x1F4
#define ATA_LBA_HIGH_REGISTER_PORT       0x1F5
#define ATA_DRIVE_REGISTER_PORT          0x1F6
#define ATA_COMMAND_REGISTER_PORT        0x1F7
#define ATA_REGULAR_STATUS_REGISTER_PORT 0x1F7
// ATA commands
#define ATA_IDENTIFY_DEVICE 0xEC
#define ATA_READ            0x20
#define ATA_WRITE           0x30

struct ata_status_byte {
    unsigned char ERR : 3; // Indicates an error occurred. 
    unsigned char DRQ : 1; // Set when the drive has PIO data to transfer, or is ready to accept PIO data. 
    unsigned char SRV : 1; // Overlapped Mode Service Request. 
    unsigned char DF  : 1; // Drive Fault Error
    unsigned char RDY : 1; // Bit is clear when drive is spun down, or after an error. Set otherwise. 
    unsigned char BSY : 1; // Indicates the drive is preparing to send/receive data
};

typedef struct ata_status_byte ata_status_byte_t;

struct disk_sector {
    unsigned char bytes[32];
};

typedef struct disk_sector disk_sector_t;

void ata_software_reset(unsigned char DCR);
void ata_wait_for_rdy(unsigned char DCR);
void ata_read_sectors(int lba, char sector_count, char *buffer);

#endif