#ifndef LIBEMPR_SERIAL_H
#define LIBEMPR_SERIAL_H

int read_usb_serial_blocking(char *buf,int length);
int read_usb_serial_none_blocking(char *buf,int length);

int write_serial(char *buf, int length);

void serial_init(void);

void serial_puts(char *str);
void serial_printf(const char *format, ...);

#endif /* LIBEMPR_SERIAL_H */
