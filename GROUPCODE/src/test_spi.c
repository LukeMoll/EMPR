#include <libempr/spi.h>
#include <stdint.h>

#define AUDIO_CS_PORT 0
#define AUDIO_CS_PIN 16

// this is about 1ms delay, or 12 bytes' clock cycles
void delay() {uint16_t i; for(i = 0; i < 10000 ; i++);}

void main(void) {
	spi_init();
	spi_cs_init(AUDIO_CS_PORT, AUDIO_CS_PIN);

	spi_cs_set(AUDIO_CS_PORT, AUDIO_CS_PIN);
	spi_send_byte(0x69);
	spi_cs_clear();

	delay();

	spi_cs_set(AUDIO_CS_PORT, AUDIO_CS_PIN);
	spi_send_byte(0x1);
	spi_send_byte(0x2);
	spi_send_byte(0x3);
	spi_send_byte(0x10);
	spi_send_byte(0x20);
	spi_send_byte(0x30);
	spi_cs_clear();

	delay();

	spi_cs_set(AUDIO_CS_PORT, AUDIO_CS_PIN);
	spi_send((uint8_t[]) {
		0x42,
		0x00,
		0x99,
		0xDE,
		0xAD,
		0xBE,
		0xEF
	},7);
	spi_cs_clear();

	return;
}
