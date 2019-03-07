#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <lpc_types.h>
#include <lpc17xx_pinsel.h>
#include <lpc17xx_uart.h>

#include "serial.h"

void serial_init(void) {
	static bool inited = false;
	if (inited)
		return;

	PINSEL_CFG_Type pin_cfg;
	pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	pin_cfg.Pinmode = PINSEL_PINMODE_PULLUP;
	pin_cfg.Funcnum = 1;
	pin_cfg.Portnum = 0;

	pin_cfg.Pinnum = 2;
	PINSEL_ConfigPin(&pin_cfg);

	pin_cfg.Pinnum = 3;
	PINSEL_ConfigPin(&pin_cfg);

	UART_CFG_Type uart_cfg;
	UART_ConfigStructInit(&uart_cfg);
	UART_Init(LPC_UART0, &uart_cfg);

	UART_FIFO_CFG_Type fifo_cfg;
	UART_FIFOConfigStructInit(&fifo_cfg);
	UART_FIFOConfig(LPC_UART0, &fifo_cfg);

	UART_TxCmd(LPC_UART0, ENABLE);

	inited = true;
}

void serial_send(uint8_t command, uint8_t *buf, uint32_t length) {
	serial_init();

	uint8_t header[5] = {command, length >> 24, length >> 16, length >> 8, length};
	UART_Send(LPC_UART0, header, 5, BLOCKING);
	UART_Send(LPC_UART0, buf, length, BLOCKING);
}

uint32_t serial_recv(uint8_t command, uint8_t *buf, uint32_t buflen) {
	serial_init();

	while (true) {
		uint8_t header[5];
		UART_Receive(LPC_UART0, header, 5, BLOCKING);
		uint8_t command_byte = header[0];
		uint32_t packlen = header[1] << 24 | header[2] << 16 | header[3] << 8 | header[4];

		if (command == command_byte) {
			uint32_t bytes_to_read = buflen < packlen ? buflen : packlen;
			UART_Receive(LPC_UART0, buf, bytes_to_read, BLOCKING);
			if (buflen < packlen)
				UART_Receive(LPC_UART0, NULL, packlen - buflen, BLOCKING);
			return bytes_to_read;
		} else {
			UART_Receive(LPC_UART0, NULL, packlen, BLOCKING);
		}
	}
}

#define PRINTER_CMD 0x01

void serial_puts(char *str) {
	serial_send(PRINTER_CMD, (uint8_t *)str, strlen(str));
}

void serial_printf(const char *format, ...) {
	va_list valist;
	va_start(valist, format);

	int len = vsnprintf(NULL, 0, format, valist);
	char *buf = malloc(len * sizeof(char));
	vsprintf(buf, format, valist);
	serial_puts(buf);

	va_end(valist);
	free(buf);
}
