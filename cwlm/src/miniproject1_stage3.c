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
    SYSTICK_InternalInit(100);
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);
    while(1);
    return 1;
}

uint8_t counter = 0;
uint8_t i = 0;

void SysTick_Handler(void) {
    SYSTICK_ClearCounterFlag();
    counter++;
    if(counter >= 10) { // in case we miss an iteration or something
        counter = 0; // reset counter
        if(i < 16) {
            status_code(i);
            i++;
        }
        else {
            status_code(0);
            SYSTICK_IntCmd(DISABLE);
            SYSTICK_Cmd(DISABLE);
        }
        
    }
}
