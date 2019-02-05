#ifndef LIBEMPR_SPI_H
#define LIBEMPR_SPI_H

#include <stddef.h>

void spi_init(void);
void spi_cs_init(uint8_t portnum, uint8_t pinnum);
void spi_cs_set(uint8_t portnum, uint8_t pinnum);
void spi_cs_clear(void);
void spi_send(uint8_t *buf, size_t len);
void spi_send_byte(uint8_t datum);
void spi_recv(uint8_t *buf, size_t len);

#endif /* LIBEMPR_SPI_H */
