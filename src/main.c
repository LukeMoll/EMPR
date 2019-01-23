#include <stdbool.h>
#include <libempr/serial.h>

void main(void) {
	serial_init();
	serial_puts("Hello, world!\r\n");

	while (true) {}
}
