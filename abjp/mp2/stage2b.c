#include <stdint.h>

#include "i2c.h"
#include "lcd.h"

void stage2b(void)
{
	i2c_init();

	lcd_init();

	lcd_puts("Hello");
	lcd_set_cursor(1, 0);
	lcd_puts("World");
}

#ifndef DEMO
void main(void)
{
	stage2b();
	while (1) {};
}
#endif
