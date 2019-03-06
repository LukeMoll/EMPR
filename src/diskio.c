/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h> 
#include <lpc17xx_pinsel.h>
#include <lpc17xx_gpio.h>
#include <libempr/spi.h>

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

/* Definitions of physical drive number for each drive */
#define DEV_SD      0

#define SD_CS_PORT  0
#define SD_CS_PIN   11

#define SD_CD_PORT  0
#define SD_CD_PIN   10

static bool block_addr = false;

static void sd_send_command(uint8_t command, uint32_t args) {
	uint8_t crc;
	switch (command) {
		case 0:
			crc = 0x95;
			break;
		case 8:
			crc = 0x87;
			break;
		default:
			crc = 0x00;
	}

	// commands are 6 bits and preceded with 0b01
	// send the following 6 bytes - command index, args3, args2, args1, args0, CRC
	uint8_t bytes[6] = {
		0x40 | (command & 0x3F),
		args >> 24,
		args >> 16,
		args >> 8,
		args,
		crc,
	};
	// force CS high then low to signal incoming command
	spi_cs_set(SD_CS_PORT, SD_CS_PIN);
	spi_send(bytes, 6);
	spi_cs_clear();
}

static uint8_t sd_get_response(uint32_t *data, bool wait_busy) {
	spi_cs_set(SD_CS_PORT, SD_CS_PIN);

	uint8_t r1;
	while ((r1 = spi_recv_byte()) & 0x80);

	if (data != NULL) {
		uint8_t buf[4];
		spi_recv(buf, 4);
		*data = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
	}

	while (wait_busy && spi_recv_byte() != 0xFF) {}

	spi_cs_clear();
	return r1;
}

static uint8_t sd_send_acmd(uint8_t command, uint32_t args) {
	sd_send_command(55, 0x00000000);
	uint8_t r1 = sd_get_response(NULL, false);
	if (r1 == 0x01)
		sd_send_command(command, args);
	return r1;
}

static bool sd_is_connected(void) {
	GPIO_SetDir(SD_CD_PORT, 1 << SD_CD_PIN, 0);
	return (bool)(GPIO_ReadValue(SD_CD_PORT) & (1 << SD_CD_PIN));
}

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE pdrv) {
	if (pdrv != DEV_SD) {
		return STA_NODISK;
	}

	return 0;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE pdrv) {
	spi_cs_init(SD_CS_PORT, SD_CS_PIN);
	uint8_t r1;

	if (pdrv != DEV_SD)
		return STA_NODISK;


	if (!sd_is_connected())
		return STA_NODISK;

	// assume >= 1ms has passed

	// Hold CS and DI high for >= 74 clock cycles
	spi_cs_clear();
	uint8_t high_bytes[10] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	spi_send(high_bytes, 10);

	r1 = 0;
	while (r1 != 0x01) {
		sd_send_command(0, 0x00000000);
		r1 = sd_get_response(NULL, false);
	}

	uint32_t data;
	sd_send_command(8, 0x000001AA);
	r1 = sd_get_response(&data, false);

	uint32_t acmd41_arg;
	if (r1 != 0x01)
		acmd41_arg = 0x00000000;
	else if (data == 0x000001AA)
		acmd41_arg = 0x40000000;
	else
		return STA_NOINIT; // unknown card!

	r1 = 0x01;
	while (r1 == 0x01) {
		sd_send_acmd(41, acmd41_arg);
		r1 = sd_get_response(NULL, false);
	}

	if (acmd41_arg == 0x40000000) {
		if (r1 != 0x00)
			return STA_NOINIT;

		// SD v2+, check if high capacity
		uint32_t ocr;
		sd_send_command(58, 0x00000000);
		r1 = sd_get_response(&ocr, false);
		if (r1 != 0x00)
			return STA_NOINIT;

		block_addr = (bool)(ocr & (1 << 30));
	} else if (r1 != 0x00) {
		r1 = 0x01;
		while (r1 == 0x01) {
			sd_send_command(1, 0x00000000);
			r1 = sd_get_response(NULL, false);
		}

		if (r1 != 0x00)
			return STA_NOINIT; // unknown card!
	}

	if (!block_addr) {
		sd_send_command(16, FF_BLOCK_SIZE); // set block size to 512 bytes for FAT
		r1 = sd_get_response(NULL, false);
		if (r1 != 0x00)
			return STA_NOINIT;
	}

	return 0;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count) {
	if (pdrv != DEV_SD)
		return RES_PARERR;

	uint32_t addr = block_addr ? sector : sector * FF_BLOCK_SIZE;
	sd_send_command(18, addr);
	uint8_t r1 = sd_get_response(NULL, false);
	if (r1 != 0x00)
		return r1 & 0x20 ? RES_PARERR : RES_ERROR;

	// read in the blocks!
	spi_cs_set(SD_CS_PORT, SD_CS_PIN);
	for (size_t i = 0; i < count; i++) {
		uint8_t token;
		while ((token = spi_recv_byte()) == 0xFF) {}

		if (token != 0xFE)
			return RES_ERROR;

		spi_recv(buff + (FF_BLOCK_SIZE * i), FF_BLOCK_SIZE);
		spi_recv(NULL, 2); // get CRC and throw it away because we don't care
	}
	spi_cs_clear();

	// terminate read
	sd_send_command(12, 0x00000000);

	spi_cs_set(SD_CS_PORT, SD_CS_PIN);
	spi_recv(NULL, 1);

	r1 = sd_get_response(NULL, true);
	if (r1 != 0x00)
		return RES_ERROR;

	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count) {
	if (pdrv != DEV_SD)
		return RES_PARERR;

#if 0
	//
	//For SDC, number of sectors to pre-erased at start of the write transaction can be specified by an ACMD23 prior to CMD25
	//send_acmd(23, (count & 0x007FFFFF));
	//if (response_r1() != 0);
		//return relevant error
		
	sd_send_command(25, sector);
	//if (response_r1() != 0);
		//return relevant error

	size_t i = 0;
	while(count--) {
		BYTE datapack[3] = {0xFC, buff[i], pdrv};
		spi_send((uint8_t *)datapack, 3);
		i++;
		//if((response_r1() & 0x0F) != 0x05);
			//return relevant error
	}
	spi_send_byte(0xFD);
#endif
	return RES_OK;


	// TODO: write from buff onto SDC
	// If write protect, return RES_WRPRT
	// If succeeds, return RES_OK; otherwise return relevant error.
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void *buff) {
	if (pdrv != DEV_SD)
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
