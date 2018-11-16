#include <lpc17xx_pwm.h>
#include <lpc17xx_pinsel.h>
#include <lpc_types.h>

#include "pwm.h"

#define PWM_CHANNEL 3
#define PWM_ENDVAL 4000
#define PWM_STARTVAL 10000
#define PWM_PRESCALE 1

int main(void) {
    /**
     * it work
     */
    PWM_setup_timer(PWM_CHANNEL, PWM_STARTVAL);
    PWM_setup_period(PWM_STARTVAL);
    PWM_setup_single_edge(PWM_CHANNEL, PWM_STARTVAL, PWM_ENDVAL, PWM_PRESCALE);
    PWM_start();
}

