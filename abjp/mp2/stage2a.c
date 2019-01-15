#include <stdint.h>

#include "i2c.h"
#include "lcd.h"

void stage2a(void)
{
	i2c_init();

	lcd_init();

	lcd_puts("Hello World");
}

#ifndef DEMO
void main(void)
{
	stage2a();
	while (1) {};
}
#endif
