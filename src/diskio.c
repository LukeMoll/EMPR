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
#include <libempr/spi.h>
#include <libempr/serial.h>

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

/* Definitions of physical drive number for each drive */
#define DEV_SD		0

#define SD_CS_PORT	0
#define SD_CS_PIN	16
#define CRC 		0x01

static void send_command(uint8_t command_index, uint32_t args) {
	command_index &= 0b00111111;
	command_index |= 0b10000000;
	uint8_t args0 = args; //args lsb
	uint8_t args1 = args >> 8;
	uint8_t args2 = args >> 16;
	uint8_t args3 = args >> 24; //args msb
	//send the following 6 bytes - command index, args3, args2, args1, args0, CRC
	uint8_t bytes[6] = {
		command_index,
		args3,
		args2,
		args1,
		args0,
		CRC
	};
	spi_send(bytes, 6);
}

static void send_acmd(uint8_t command_index, uint32_t args) {
	send_command(55, 0x00000000);
	send_command(command_index, args);
}

static uint8_t response_r1() {
	uint8_t response = 0xFF;
	while(response == 0xFF) //change this so that it also returns a timeout
		spi_recv(&response, 1);

	return response;
}

static void response_r7(uint8_t *response) {
	//response is always gonna be 5 bytes long
	response[0] = 0xFF;

	while(response[0] == 0xFF) //add a timeout thingy
		spi_recv(response, 5);
}

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE pdrv) {
	serial_printf("disk_status(%d)\r\n", pdrv);

	if (pdrv != DEV_SD) {
		return STA_NODISK;
	}

	return 0;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE pdrv) {
	serial_printf("disk_initialize(%d)\r\n", pdrv);

	bool unknown_card = false;

	if (pdrv != DEV_SD) {
		return STA_NODISK;
	}

	// assuming >= 1 ms has passed before this function is called.
	spi_cs_init(SD_CS_PORT, SD_CS_PIN);
	spi_cs_clear();

	int x = 10;
	while (x--)
		spi_send_byte(0);
	//after sending 80 bytes (>74 dummy clock cycles), set CS low, and send CMD0
	spi_cs_set(SD_CS_PORT, SD_CS_PIN);
	send_command(0, 0x00000000);
	if(response_r1() != 0x01) {
		unknown_card = true;
	};
	send_command(8, 0x000001AA);
	uint8_t r7[5];
	response_r7(r7);
	if(!((r7[4] == 0xAA) && (r7[4] == 0x01))) {
		unknown_card = true;
	}
	//a PC-- would be so incredibly useful here
	uint8_t r1 = 0x01;
	while(r1 == 0x01) {
		r1 = response_r1();
		//if error or timeout, unknows_card()
	}

	if(r1 == 0x00)
		send_command(58, 0x00000000);
	else
		unknown_card = true;

	//read CCS bit in OCR to determine version
	if(unknown_card)
		return STA_NOINIT;

	return 0;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count) {
	serial_printf("disk_read(%d, buff, sector, count)\r\n", pdrv);

	if (pdrv != DEV_SD)
		return RES_PARERR;

	if(response_r1() != 0);
		// return error (or retry? idk)
		
	spi_recv((uint8_t *)buff, count);

	send_command(12, 0x00000000);
	if(response_r1() != 0);
		// return relevant error;
		
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count) {
	serial_printf("disk_write(%d, buff, sector, count)\r\n", pdrv);
	
	if (pdrv != DEV_SD)
		return RES_PARERR;
	//
	//For SDC, number of sectors to pre-erased at start of the write transaction can be specified by an ACMD23 prior to CMD25
	send_acmd(23, (count & 0x007FFFFF));
	if (response_r1() != 0);
		//return relevant error
		
	send_command(25, sector);
	if (response_r1() != 0);
		//return relevant error

	size_t i = 0;
	while(count--) {
		BYTE datapack[3] = {0xFC, buff[i], pdrv};
		spi_send((uint8_t *)datapack, 3);
		i++;
		if((response_r1() & 0x0F) != 0x05);
			//return relevant error
	}
	spi_send_byte(0xFD);
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
	serial_printf("disk_ioctl(%d, %d, buff)\r\n", pdrv, cmd);

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