#include <stdbool.h>
#include <stdint.h>

#include "i2c.h"
#include "serial.h"

void stage1(void)
{
	i2c_init();
	serial_puts("i2c initialised\r\n");

	uint8_t buf = 0;
	int devices = 0;

	uint32_t addr;
	for (addr = 0; addr < 128; addr++) {
		if (i2c_send(addr, &buf, 1) == SUCCESS) {
			serial_printf("found device @ %d (0x%X)\r\n", addr, addr);
			devices++;
		}
	}

	serial_printf("%d devices connected to i2c bus\r\n", devices);
}

#ifndef DEMO
void main(void)
{
	stage1();
	while (1) {};
}
#endif
