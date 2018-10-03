#include "lpc17xx_gpio.h"
#include "lpc17xx_systick.h"
#include "pins.h"

#include "status.h"
#include "serial.h"
#include <stdio.h>

/**
 * For use with GPIO_SetDir
 */ 
#define DIR_IN 0
#define DIR_OUT 1

void SysTick_Handler(void);

int main(void) {
    uint32_t led_mask = 0 | (1<<LED0_PIN) | (1<<LED1_PIN) | (1<<LED2_PIN) | (1<<LED3_PIN);
    GPIO_SetDir(LED_PORT, led_mask, DIR_OUT);

    serial_init(); //initialises to Baudrate 9600, 8 data bits (screen /dev/ttyACM0 9600,cs8)
    write_usb_serial_blocking("Starting Count\n\r", 17);

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

            char binary_i[4];
            sprintf(binary_i, "%d%d%d%d", (i&8)>>3, (i&4)>>2, (i&2)>>1, (i&1))
            char buf[12];
            sprintf(buf, "%02d\t%01x\t%s\n\r", i, i, binary_i);
            write_usb_serial_blocking(buf, 12);
            i++;
        }
        else {
            status_code(0);
            SYSTICK_IntCmd(DISABLE);
            SYSTICK_Cmd(DISABLE);
            write_usb_serial_blocking("finished count\n\r", 17);
        }
        
    }
}
