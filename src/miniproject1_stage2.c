#include "lpc17xx_gpio.h"

/**
 * Port and pin numbers of the LPC17xx that correspond to the builtin LEDs on the ARM mbed board.
 */ 
#define LED_PORT 1
#define LED0_PIN 18
#define LED1_PIN 20
#define LED2_PIN 21
#define LED3_PIN 23

/**
 * For use with GPIO_SetDir
 */ 
#define DIR_IN 0
#define DIR_OUT 1

void status_code(uint8_t code);

int main(void) {
	status_code(5);
	return 0;
}

/**
 * lights up an LED pattern that gives the error code (use in debugging)
 * ignores upper 4 bits
 */
void status_code(uint8_t code) {
	uint32_t led_mask = 0 | (1 << LED0_PIN) | (1 << LED1_PIN) | (1<< LED2_PIN) | (1 << LED3_PIN);
	uint32_t lights = 0 | (((code & 1) << LED0_PIN) |
	                       ((code & 2) << LED1_PIN-1) |
	                       ((code & 4) << LED2_PIN-2) |
	                       ((code & 8) << LED3_PIN-3));
	GPIO_SetDir(LED_PORT, led_mask, DIR_OUT);
	GPIO_ClearValue(LED_PORT, led_mask);
	GPIO_SetValue(LED_PORT, lights);
}

