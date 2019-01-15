#include "serial.h"
#include "status.h"
#include <lpc17xx_systick.h>

#include <stdio.h>

#include <lpc17xx_timer.h>
#include <lpc17xx_rit.h>

void RIT_IRQHandler(void);
void SysTick_Handler(void);
uint16_t counter;
uint8_t tick;

/*
* well it works now, who know why
**/

int main(void) {

    serial_init();

    write_usb_serial_blocking("begin\r\n", 8);
    counter = 0;

    RIT_Init(LPC_RIT);

    RIT_TimerConfig(LPC_RIT, 1000);

    NVIC_EnableIRQ(RIT_IRQn);
    while(1){};
    return 0;

}    


void RIT_IRQHandler(void) {

    RIT_GetIntStatus(LPC_RIT);  //clears interupt flag so it doesn't get called

    tick++;
    status_code(tick);
    
}