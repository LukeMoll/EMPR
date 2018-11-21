#include <lpc17xx_pwm.h>
#include <lpc17xx_pinsel.h>
#include <lpc_types.h>
#include <lpc17xx_systick.h>

#include "pwm.h"

#define PWM_CHANNEL 3
#define PWM_ENDVAL 0
#define PWM_PERIOD 20000
#define PWM_PRESCALE 1

uint32_t length = PWM_ENDVAL;
void SysTick_Handler(void);

int main(void) {
    /**
     * it work
     */

    PWM_setup_timer(PWM_CHANNEL, PWM_PRESCALE);
    PWM_setup_period(PWM_PERIOD);
    PWM_setup_single_edge(PWM_CHANNEL, length);
    PWM_start();

    SYSTICK_InternalInit(1);
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);

    while(1){};
    //still need to do the demo bit for it
}

void SysTick_Handler(void){
    
    SYSTICK_ClearCounterFlag();
    
    switch(length) {
        case PWM_PERIOD :
            length = 0;
            break;
        default:
            length += 4;
            break;
    }

    PWM_setup_single_edge(PWM_CHANNEL, length);
}

