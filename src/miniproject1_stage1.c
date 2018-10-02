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

/**
 * This value, when used in a busywait (see `void count_delay(uint32_t)` as an example) causes
 * the cpu to wait for approximately one second. Multiplying this value has unexpected consequences
 */ 
#define ACTUAL_BS_ONE_SECOND 545000000

int main(void);
void count_delay(uint32_t value);

int main(void) {
    uint32_t led_mask = 0 | (1<<LED0_PIN) | (1<<LED1_PIN) | (1<<LED2_PIN) | (1<<LED3_PIN);

    uint8_t i;
    GPIO_SetDir(LED_PORT, led_mask, DIR_OUT);
    for(i = 0; i < 2; i ++) {
        GPIO_ClearValue(LED_PORT, led_mask); // ClearValue sets all the 1 bits in led_mask to LOW
        count_delay(ACTUAL_BS_ONE_SECOND);

        GPIO_SetValue(LED_PORT, (1<<LED0_PIN));
        count_delay(ACTUAL_BS_ONE_SECOND);

        GPIO_ClearValue(LED_PORT, led_mask);
        GPIO_SetValue(LED_PORT, (1<<LED1_PIN)); // SetValue sets all the 1 bits in led_mask to HIGH
        count_delay(ACTUAL_BS_ONE_SECOND);

        GPIO_ClearValue(LED_PORT, led_mask);
        GPIO_SetValue(LED_PORT, (1<<LED2_PIN));
        count_delay(ACTUAL_BS_ONE_SECOND);

        GPIO_ClearValue(LED_PORT, led_mask);
        GPIO_SetValue(LED_PORT, (1<<LED3_PIN));
        count_delay(ACTUAL_BS_ONE_SECOND);
    }
    GPIO_ClearValue(LED_PORT, led_mask);
    return 0;
}

void count_delay(uint32_t value) {
    uint32_t i;
    while(i < value) {
        i++;
    }
}