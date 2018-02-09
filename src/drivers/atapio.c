#include "atapio.h"
#include "../asm/asmio.h"
#include "../libs/string.h"
#include "screen.h"

void ata_software_reset(unsigned char DCR) {
    int i;
    out_byte(DCR, 0x04);
    out_byte(DCR, 0x00);
    for(i=0; i > 0; i++){
        in_byte(DCR);
    }
    ata_wait_for_rdy(DCR);
}

void ata_wait_for_rdy(unsigned char DCR) {
    unsigned char stsbyte;
    ata_status_byte_t * stsbyte_ptr = (ata_status_byte_t *)&stsbyte;
    do {
        stsbyte = in_byte(DCR);
    } while( stsbyte_ptr->BSY && !stsbyte_ptr->RDY );
}

void ata_wait_for_drq(unsigned char DCR) {
    unsigned char stsbyte;
    ata_status_byte_t * stsbyte_ptr = (ata_status_byte_t *)&stsbyte;
    do {
        stsbyte = in_byte(DCR);
    } while( !stsbyte_ptr->DRQ );
}

void ata_read_sectors(int lba, char sector_count, char *buffer) {
	out_byte(ATA_DRIVE_REGISTER_PORT, 0xE0 | 0x40 | ((lba>>24) & 0x0F)); //master drive + lba + high lba bits
	out_byte(ATA_SECTOR_COUNT_REGISTER_PORT, (unsigned char)sector_count); //sector count
	out_byte(ATA_LBA_LOW_REGISTER_PORT, (unsigned char)lba);
	out_byte(ATA_LBA_MID_REGISTER_PORT, (unsigned char)(lba>>8));
	out_byte(ATA_LBA_HIGH_REGISTER_PORT, (unsigned char)(lba>>16));
	out_byte(ATA_COMMAND_REGISTER_PORT, ATA_READ); //read command
	int offset = 0;
	for(;sector_count--;){
		ata_wait_for_rdy(ATA_REGULAR_STATUS_REGISTER_PORT);
        ata_wait_for_drq(ATA_REGULAR_STATUS_REGISTER_PORT);
		int i;
		for(i = 0; i < 256; i++){
            *((unsigned short *)buffer + offset + i)=in_word(ATA_DATA_REGISTER_PORT);
        }
		offset += 256;
	}
	return;
}
