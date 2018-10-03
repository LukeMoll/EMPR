#include "lpc17xx_gpio.h"

#include "pins.h"

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

