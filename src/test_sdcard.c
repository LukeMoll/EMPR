#include <stdlib.h>
#include <libempr/serial.h>
#include "ff.h"

void main(void) {
	serial_puts("starting\r\n");

	FRESULT res;
	FATFS fs;
	res = f_mount(&fs, "", 1);

	serial_printf("f_mount res: %d\r\n", res);

	char cwd[128];
	res = f_getcwd(cwd, 128);
	serial_printf("f_getcwd res: %d\r\n", res);
	if (res == FR_OK)
		serial_printf("cwd: \"%s\"\r\n", cwd);

	f_mount(0, "", 0);

	while (1) {}
}
