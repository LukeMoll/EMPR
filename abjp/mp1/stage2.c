#include "lpc17xx_gpio.h"
#include "lpc17xx_systick.h"

const int LEDS[4] = {1 << 18, 1 << 20, 1 << 21, 1 << 23};
const int ALL_LEDS = 1 << 18 | 1 << 20 | 1 << 21 | 1 << 23;
const int DELAY = 1 << 23;

void wait(void)
{
	int x = 0;
	while (x < DELAY)
		x++;
}

void display_number(int n)
{
	int i;
	for (i = 0; i < 4; i++) {
		if (n & (1 << i)) {
			GPIO_SetValue(1, LEDS[i]);
		} else {
			GPIO_ClearValue(1, LEDS[i]);
		}
	}
}

void main(void)
{
	GPIO_SetDir(1, ALL_LEDS, 1);

	int n;
	for (n = 0; n < 16; n++) {
		display_number(n);
		wait();
	}

	GPIO_ClearValue(1, ALL_LEDS);
}
