#include <stdlib.h>
#include <string.h>
#include <lpc17xx_rtc.h>
#include <libempr/spi.h>
#include <libempr/serial.h>
#include "ff.h"

FRESULT list_files(char *path) {
	FRESULT res;
	DIR dp;

	if ((res = f_opendir(&dp, path)) != FR_OK)
		return res;

	FILINFO fno;
	while ((res = f_readdir(&dp, &fno)) == FR_OK) {
		if (fno.fname[0] == 0)
			break;

		serial_printf("%s%s", path, fno.fname);
		if (fno.fattrib & AM_DIR)
			serial_puts("/");
		serial_puts("\r\n");
	}

	return res;
}

FRESULT print_file(char *path) {
	FRESULT res;
	FIL fp;

	if ((res = f_open(&fp, path, FA_READ)) != FR_OK)
		return res;

	UINT size = f_size(&fp);
	UINT bytes_read;
	char *buf = malloc((size * sizeof(char)) + 1);
	if ((res = f_read(&fp, buf, size, &bytes_read)) != FR_OK)
		return res;
	buf[bytes_read] = '\0';

	if ((res = f_close(&fp)) != FR_OK)
		return res;

	serial_puts(buf);
	serial_puts("\r\n");
	free(buf);

	return FR_OK;
}

FRESULT write_file(char *path, char *contents) {
	FRESULT res;
	FIL fp;

	if ((res = f_open(&fp, path, FA_WRITE | FA_CREATE_ALWAYS)) != FR_OK)
		return res;

	UINT bytes_written;
	if ((res = f_write(&fp, contents, strlen(contents), &bytes_written) != FR_OK))
		return res;

	if ((res = f_close(&fp)) != FR_OK)
		return res;

	return FR_OK;
}

void main(void) {
	RTC_Init(LPC_RTC);

	// Set time to 2018-03-06 08:00
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_YEAR, 2018);
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_MONTH, 3);
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_DAYOFMONTH, 6);
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_HOUR, 8);
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_MINUTE, 0);
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_SECOND, 0);

	spi_init();

	FRESULT res;

	FATFS fs;
	res = f_mount(&fs, "", 1);
	serial_printf("f_mount returned %d\r\n", res);

	res = list_files("");
	serial_printf("list_files returned %d\r\n", res);

	res = print_file("TRAGEDY.TXT");
	serial_printf("print_file returned %d\r\n", res);

	res = write_file("TEST.TXT", "Hello, world!");
	serial_printf("write_file returned %d\r\n", res);

	f_unmount("");

	while (1);
}
