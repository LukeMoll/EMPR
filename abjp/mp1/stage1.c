#include "lpc17xx_gpio.h"

const int DELAY = 1 << 23;

void main(void)
{
	int leds[4] = {18, 20, 21, 23};

	int i;
	for (i = 0; i < 2; i++) {
		int led;
		for (led = 0; led < 4; led++) {
			int value = 1 << leds[led];

			GPIO_SetDir(1, value, 1);
			GPIO_SetValue(1, value);
			
			int x = 0;
			while (x <= DELAY)
				x++;

			GPIO_ClearValue(1, value);
		}
	}
}
