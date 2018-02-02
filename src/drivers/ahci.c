#include "ahci.h"

void init_ahci(void) {
    FIS_REG_H2D_t fis;
    memset(&fis, 0, sizeof(FIS_REG_H2D_t));
    fis.fis_type = FIS_TYPE_REG_H2D;
    fis.command = ATA_CMD_IDENTIFY;
    fis.device = 0;			// Master device
    fis.c = 1;				// Write command register
    
}