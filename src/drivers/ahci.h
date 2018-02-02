#ifndef AHCI_H
#define AHCI_H

// Kinds of FIS specified in Serial ATA Revision 3.0
typedef enum FIS_TYPE
{
	FIS_TYPE_REG_H2D	= 0x27,	// Register FIS - host to device
	FIS_TYPE_REG_D2H	= 0x34,	// Register FIS - device to host
	FIS_TYPE_DMA_ACT	= 0x39,	// DMA activate FIS - device to host
	FIS_TYPE_DMA_SETUP	= 0x41,	// DMA setup FIS - bidirectional
	FIS_TYPE_DATA		= 0x46,	// Data FIS - bidirectional
	FIS_TYPE_BIST		= 0x58,	// BIST activate FIS - bidirectional
	FIS_TYPE_PIO_SETUP	= 0x5F,	// PIO setup FIS - device to host
	FIS_TYPE_DEV_BITS	= 0xA1,	// Set device bits FIS - device to host
};

typedef enum ATA_COMMANDS
{
    ATA_CMD_IDENTIFY = 0xEC,
};

struct FIS_REG_H2D
{
	// DWORD 0
	unsigned char  fis_type;	// FIS_TYPE_REG_H2D
 
	unsigned char  pmport:4;	// Port multiplier
	unsigned char  rsv0:3;		// Reserved
	unsigned char  c:1;		    // 1: Command, 0: Control
 
	unsigned char  command;	    // Command register
	unsigned char  featurel;	// Feature register, 7:0
 
	// DWORD 1
	unsigned char  lba0;		// LBA low register, 7:0
	unsigned char  lba1;		// LBA mid register, 15:8
	unsigned char  lba2;		// LBA high register, 23:16
	unsigned char  device;		// Device register
 
	// DWORD 2
	unsigned char  lba3;		// LBA register, 31:24
	unsigned char  lba4;		// LBA register, 39:32
	unsigned char  lba5;		// LBA register, 47:40
	unsigned char  featureh;	// Feature register, 15:8
 
	// DWORD 3
	unsigned char  countl;		// Count register, 7:0
	unsigned char  counth;		// Count register, 15:8
	unsigned char  icc;		    // Isochronous command completion
	unsigned char  control;	    // Control register
 
	// DWORD 4
	unsigned char  rsv1[4];	// Reserved
};

typedef struct FIS_REG_H2D FIS_REG_H2D_t;

struct FIS_REG_D2H
{
	// DWORD 0
	unsigned char  fis_type;    // FIS_TYPE_REG_D2H
 
	unsigned char  pmport:4;    // Port multiplier
	unsigned char  rsv0:2;      // Reserved
	unsigned char  i:1;         // Interrupt bit
	unsigned char  rsv1:1;      // Reserved
 
	unsigned char  status;      // Status register
	unsigned char  error;       // Error register
 
	// DWORD 1
	unsigned char  lba0;        // LBA low register, 7:0
	unsigned char  lba1;        // LBA mid register, 15:8
	unsigned char  lba2;        // LBA high register, 23:16
	unsigned char  device;      // Device register
 
	// DWORD 2
	unsigned char  lba3;        // LBA register, 31:24
	unsigned char  lba4;        // LBA register, 39:32
	unsigned char  lba5;        // LBA register, 47:40
	unsigned char  rsv2;        // Reserved
 
	// DWORD 3
	unsigned char  countl;      // Count register, 7:0
	unsigned char  counth;      // Count register, 15:8
	unsigned char  rsv3[2];     // Reserved
 
	// DWORD 4
	unsigned char  rsv4[4];     // Reserved
};

typedef struct FIS_REG_D2H FIS_REG_D2H_t;

struct FIS_DATA
{
	// DWORD 0
	unsigned char  fis_type;	// FIS_TYPE_DATA
 
	unsigned char  pmport:4;	// Port multiplier
	unsigned char  rsv0:4;		// Reserved
 
	unsigned char  rsv1[2];	    // Reserved
 
	// DWORD 1 ~ N
	unsigned int   data[1];	    // Payload
};

typedef struct FIS_DATA FIS_DATA_t;

struct FIS_PIO_SETUP
{
	// DWORD 0
	unsigned char  fis_type;	// FIS_TYPE_PIO_SETUP
 
	unsigned char  pmport:4;	// Port multiplier
	unsigned char  rsv0:1;		// Reserved
	unsigned char  d:1;		    // Data transfer direction, 1 - device to host
	unsigned char  i:1;		    // Interrupt bit
	unsigned char  rsv1:1;
 
	unsigned char  status;		// Status register
	unsigned char  error;		// Error register
 
	// DWORD 1
	unsigned char  lba0;		// LBA low register, 7:0
	unsigned char  lba1;		// LBA mid register, 15:8
	unsigned char  lba2;		// LBA high register, 23:16
	unsigned char  device;		// Device register
 
	// DWORD 2
	unsigned char  lba3;		// LBA register, 31:24
	unsigned char  lba4;		// LBA register, 39:32
	unsigned char  lba5;		// LBA register, 47:40
	unsigned char  rsv2;		// Reserved
 
	// DWORD 3
	unsigned char  countl;		// Count register, 7:0
	unsigned char  counth;		// Count register, 15:8
	unsigned char  rsv3;		// Reserved
	unsigned char  e_status;	// New value of status register
 
	// DWORD 4
	unsigned short tc;		    // Transfer count
	unsigned char  rsv4[2];	    // Reserved
};

typedef struct FIS_PIO_SETUP FIS_PIO_SETUP_t;

struct FIS_DMA_SETUP
{
	// DWORD 0
	unsigned char  fis_type;	    // FIS_TYPE_DMA_SETUP
 
	unsigned char  pmport:4;	    // Port multiplier
	unsigned char  rsv0:1;		    // Reserved
	unsigned char  d:1;		        // Data transfer direction, 1 - device to host
	unsigned char  i:1;		        // Interrupt bit
	unsigned char  a:1;             // Auto-activate. Specifies if DMA Activate FIS is needed
    unsigned char  rsved[2];        // Reserved
 
	//DWORD 1&2
    unsigned long long DMAbufferID; /* DMA Buffer Identifier. Used to Identify 
                                     * DMA buffer in host memory. SATA Spec says 
                                     * host specific and not in Spec. Trying AHCI 
                                     * spec might work.
                                     */

    //DWORD 3
    unsigned int rsvd;                  // More reserved

    //DWORD 4
    unsigned int DMAbufOffset;          // Byte offset into buffer. First 2 bits must be 0

    //DWORD 5
    unsigned int TransferCount;         // Number of bytes to transfer. Bit 0 must be 0

    //DWORD 6
    unsigned int resvd;                 // Reserved
};

typedef struct FIS_DMA_SETUP FIS_DMA_SETUP_t;

void init_ahci(void);

#endif