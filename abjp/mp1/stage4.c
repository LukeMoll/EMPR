#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <lpc17xx_gpio.h>
#include <lpc17xx_systick.h>

#include "serial.h"

#define LED_0 (1 << 18)
#define LED_1 (1 << 20)
#define LED_2 (1 << 21)
#define LED_3 (1 << 23)

const uint32_t LEDS[4] = { LED_0, LED_1, LED_2, LED_3 };
const uint32_t ALL_LEDS = LED_0 | LED_1 | LED_2 | LED_3;

void send_str(char *str)
{
	static bool serial_inited = false;
	if (!serial_inited) {
		serial_init();
		serial_inited = true;
	}

	write_usb_serial_blocking(str, strlen(str));
	write_usb_serial_blocking("\r\n", 2);
}

void display_number(int n)
{
	bool bits[4];

	int i;
	for (i = 0; i < 4; i++) {
		bits[i] = (bool)(n & (1 << i));
		if (bits[i]) {
			GPIO_SetValue(1, LEDS[i]);
		} else {
			GPIO_ClearValue(1, LEDS[i]);
		}
	}

	char msg[13];
	sprintf(msg, "%3d\t%3X\t%d%d%d%d", n, n, bits[3], bits[2], bits[1], bits[0]);
	send_str(msg);
}

void SysTick_Handler(void)
{
	static int time = 0;
	static int n = 0;

	SYSTICK_ClearCounterFlag();

	if (time < 10) {
		time++;
		return;
	}

	time = 0;

	if (n >= 16) {
		GPIO_ClearValue(1, ALL_LEDS);
		SYSTICK_IntCmd(DISABLE);
		send_str("Finished count");
	} else {
		display_number(n++); // display n and _then_ increment it
	}
}

void main(void)
{
	GPIO_SetDir(1, ALL_LEDS, 1);

	send_str("Starting count");
	send_str("Dec\tHex\tBin");

	SYSTICK_InternalInit(100);
	SYSTICK_IntCmd(ENABLE);
	SYSTICK_Cmd(ENABLE);
}
