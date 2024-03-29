#include <stdbool.h>
#include <lpc17xx_pinsel.h>
#include <lpc17xx_gpio.h>
#include <lpc17xx_spi.h>

#include "spi.h"

static bool cs_is_set;
static uint8_t cs_current_port, cs_current_pin;

void spi_init(void) {
	PINSEL_CFG_Type pin_cfg;
	pin_cfg.Funcnum = 3;
	pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	pin_cfg.Pinmode = PINSEL_PINMODE_PULLUP;
	pin_cfg.Portnum = 0;

	pin_cfg.Pinnum = 18; // MOSI (TX)
	PINSEL_ConfigPin(&pin_cfg);
	pin_cfg.Pinnum = 17; // MISO (RX)
	PINSEL_ConfigPin(&pin_cfg);
	pin_cfg.Pinnum = 15; // Clock
	PINSEL_ConfigPin(&pin_cfg);

	SPI_CFG_Type spi_cfg;
	SPI_ConfigStructInit(&spi_cfg);
	//spi_cfg.CPHA = 0;
	//spi_cfg.CPOL = 0;
	spi_cfg.ClockRate = 10000;
	//spi_cfg.Databit = SPI_DATABIT_8;
	SPI_Init(LPC_SPI, &spi_cfg);

	cs_is_set = false;
}

void spi_cs_init(uint8_t portnum, uint8_t pinnum) {
	PINSEL_CFG_Type pin_cfg;
	pin_cfg.Funcnum = 0;
	pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	pin_cfg.Pinmode = PINSEL_PINMODE_PULLUP;
	pin_cfg.Portnum = portnum;
	pin_cfg.Pinnum = pinnum;
	PINSEL_ConfigPin(&pin_cfg);

	GPIO_SetDir(portnum, 1 << pinnum, 1);
	GPIO_SetValue(portnum, 1 << pinnum);
}

void spi_cs_set(uint8_t portnum, uint8_t pinnum) {
	if (cs_is_set)
		GPIO_SetValue(cs_current_port, 1 << cs_current_pin);

	GPIO_ClearValue(portnum, 1 << pinnum);

	cs_current_port = portnum;
	cs_current_pin = pinnum;
	cs_is_set = true;
}

void spi_cs_clear(void) {
	GPIO_SetValue(cs_current_port, 1 << cs_current_pin);
	cs_is_set = false;
}

void spi_send(uint8_t *buf, size_t len) {
	SPI_DATA_SETUP_Type config;
	config.tx_data = buf;
	config.rx_data = NULL;
	config.length = len;
	SPI_ReadWrite(LPC_SPI, &config, SPI_TRANSFER_POLLING);
}

void spi_send_byte(uint8_t datum) {
	spi_send(&datum, 1);
}

void spi_recv(uint8_t *buf, size_t len) {
	SPI_DATA_SETUP_Type config;
	config.tx_data = NULL;
	config.rx_data = buf;
	config.length = len;
	SPI_ReadWrite(LPC_SPI, &config, SPI_TRANSFER_POLLING);
}

uint8_t spi_recv_byte(void) {
	uint8_t datum;
	spi_recv(&datum, 1);
	return datum;
}
