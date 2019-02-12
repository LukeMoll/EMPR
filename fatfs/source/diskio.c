/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include <libempr/spi.h>
#include <stdbool.h>

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

#define SD_CS_PORT	0
#define SD_CS_PIN	16

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE pdrv) {
	if (pdrv != DEV_MMC) {
		return STA_NODISK;
	}

	// TODO: check how the sd card is. if okay return 0, otherwise return relevant flags.
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE pdrv) {
	if (pdrv != DEV_MMC) {
		return STA_NODISK;
	}

	// assuming >= 1 ms has passed before this function is called.
	spi_cs_init(SD_CS_PORT, SD_CS_PIN);
	spi_cs_clear();

	int x = 10;
	while (x--)
		spi_send_byte(0);
	
	// TODO: initialize SDC in SPI mode (see diagram)
	// If anything fails, return STA_NOINIT; otherwise return 0;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count) {
	if (pdrv != DEV_MMC)
		return RES_PARERR;
	
	// TODO: Read from SDC into buff.
	// If succeeds, return RES_OK; otherwise return relevant error.
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count) {
	if (pdrv != DEV_MMC)
		return RES_PARERR;

	// TODO: write from buff onto SDC
	// If write protect, return RES_WRPRT
	// If succeeds, return RES_OK; otherwise return relevant error.
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void *buff) {
	if (pdrv != DEV_MMC)
		return RES_PARERR;

	// TODO: it complicated http://elm-chan.org/fsw/ff/doc/dioctl.html

	switch (cmd) {
		case CTRL_SYNC:
			break;
		case GET_SECTOR_COUNT:
			break;
		case GET_SECTOR_SIZE:
			break;
		case GET_BLOCK_SIZE:
			break;
		case CTRL_TRIM:
			break;
		// TODO: decide what optional commands to support.
		default:
			return RES_PARERR;
	}
}

