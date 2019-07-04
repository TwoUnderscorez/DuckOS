#include "atapio.h"
#include "../asm/asmio.h"
#include "../libs/string.h"
#include "screen.h"
#include "../kernel/pic.h"

void atapio_software_reset(unsigned short DCR)
{
	int i;
	out_byte(DCR, 0x04);
	out_byte(DCR, 0x00);
	for (i = 0; i > 0; i++)
	{
		in_byte(DCR);
	}
	atapio_wait_for_rdy(DCR);
}

void atapio_wait_for_rdy(unsigned short DCR)
{
	unsigned char stsbyte;
	atapio_status_byte_t *stsbyte_ptr = (atapio_status_byte_t *)&stsbyte;
	do
	{
		stsbyte = in_byte(DCR);
	} while (stsbyte_ptr->BSY && !stsbyte_ptr->RDY);
}

void atapio_wait_for_drq(unsigned short DCR)
{
	unsigned char stsbyte;
	atapio_status_byte_t *stsbyte_ptr = (atapio_status_byte_t *)&stsbyte;
	do
	{
		stsbyte = in_byte(DCR);
	} while (!stsbyte_ptr->DRQ);
}

/**
 * @brief ATA Write sectors
 * 
 * @param lba linear block address 
 * @param sector_count 
 * @param buffer 
 */
void atapio_read_sectors(unsigned int lba, char sector_count, char *buffer)
{
	out_byte(ATAPIO_ERR_INFO_PORT, 0x00);
	out_byte(ATAPIO_SECTOR_COUNT_REGISTER_PORT, (unsigned char)sector_count);
	out_byte(ATAPIO_LBA_LOW_REGISTER_PORT, (unsigned char)lba);
	out_byte(ATAPIO_LBA_MID_REGISTER_PORT, (unsigned char)(lba >> 8));
	out_byte(ATAPIO_LBA_HIGH_REGISTER_PORT, (unsigned char)(lba >> 16));
	// master drive + lba + high lba bits
	out_byte(ATAPIO_DRIVE_REGISTER_PORT, 0xE0 | 0x40 | ((lba >> 24) & 0x0F));
	out_byte(ATAPIO_COMMAND_REGISTER_PORT, ATAPIO_READ); //read command
	unsigned int offset = 0, i = 0;
	while (sector_count--)
	{
		atapio_wait_for_rdy(ATAPIO_REGULAR_STATUS_REGISTER_PORT);
		// Spun up
		atapio_wait_for_drq(ATAPIO_REGULAR_STATUS_REGISTER_PORT);
		// Ready for transfer
		for (i = 0; i < 256; i++)
		{
			*((unsigned short *)buffer + offset + i) = in_word(ATAPIO_DATA_REGISTER_PORT);
		}
		offset += 256;
	}
	return;
}

/**
 * @brief ATA write sectors
 * 
 * @param lba linear block addres
 * @param sector_count 
 * @param buffer 
 */
void atapio_write_sectors(int lba, char sector_count, char *buffer)
{
	out_byte(ATAPIO_ERR_INFO_PORT, 0x00);
	out_byte(ATAPIO_ERR_INFO_PORT, 0x00);
	out_byte(ATAPIO_SECTOR_COUNT_REGISTER_PORT, (unsigned char)sector_count);
	out_byte(ATAPIO_LBA_LOW_REGISTER_PORT, (unsigned char)lba);
	out_byte(ATAPIO_LBA_MID_REGISTER_PORT, (unsigned char)(lba >> 8));
	out_byte(ATAPIO_LBA_HIGH_REGISTER_PORT, (unsigned char)(lba >> 16));
	// master drive + lbal + high lba bits
	out_byte(ATAPIO_DRIVE_REGISTER_PORT, 0xE0 | 0x40 | ((lba >> 24) & 0x0F));
	out_byte(ATAPIO_COMMAND_REGISTER_PORT, ATAPIO_WRITE);
	unsigned int offset = 0, i = 0;
	while (sector_count--)
	{
		atapio_wait_for_rdy(ATAPIO_REGULAR_STATUS_REGISTER_PORT);
		atapio_wait_for_drq(ATAPIO_REGULAR_STATUS_REGISTER_PORT);
		for (i = 0; i < 256; out_word(ATAPIO_DATA_REGISTER_PORT, *((unsigned short *)buffer + offset + i++)))
			;
		offset += 256;
	}
	return;
}

void atapio_irq_handler()
{
	PIC_sendEOI(ATAPIO_IRQ_LINE);
}

void atapio_io_delay()
{
	unsigned int i;
	for (i = 0; i < 10000; i++)
		io_wait();
}