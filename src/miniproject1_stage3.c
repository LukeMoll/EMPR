#include "lpc17xx_gpio.h"
#include "lpc17xx_systick.h"
#include "pins.h"

#include "status.h"

/**
 * For use with GPIO_SetDir
 */ 
#define DIR_IN 0
#define DIR_OUT 1

void SysTick_Handler(void);

int main(void) {
    // return c_entry();
    uint32_t led_mask = 0 | (1<<LED0_PIN) | (1<<LED1_PIN) | (1<<LED2_PIN) | (1<<LED3_PIN);
    GPIO_SetDir(LED_PORT, led_mask, DIR_OUT);
    status_code(11);
    SYSTICK_InternalInit(100);
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);
    while(1);
    return 1;
}

uint8_t counter = 0;


void SysTick_Handler(void) {
    SYSTICK_ClearCounterFlag();
    counter++;
    if(counter >= 10) { // in case we miss an iteration or something
        counter = 0; // reset counter
        uint32_t val = GPIO_ReadValue(LED_PORT);
        if(val & (1<<LED0_PIN)) {
            GPIO_ClearValue(LED_PORT, (1<<LED0_PIN));
        }
        else {
            GPIO_SetValue(LED_PORT, (1<<LED0_PIN));
        }
    }
    // status_code(1);
}