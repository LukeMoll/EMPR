#ifndef LIBEMPR_SERIAL_H
#define LIBEMPR_SERIAL_H

#include <stdint.h>
#include <stdbool.h>

void serial_init(void);
void serial_send(uint8_t command, uint8_t *buf, uint32_t length);
uint32_t serial_recv(uint8_t command, uint8_t *buf, uint32_t buflen);

void serial_puts(char *str);
void serial_printf(const char *format, ...);

#endif /* LIBEMPR_SERIAL_H */
