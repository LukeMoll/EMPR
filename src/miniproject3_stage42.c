#include <lpc17xx_pwm.h>
#include <lpc17xx_pinsel.h>
#include <lpc_types.h>

#include "pwm.h"

#define PWM_CHANNEL 3
#define PWM_PERIOD 20000
#define PWM_STARTVAL 3000
#define PWM_ENDVAL 7000
#define PWM_PRESCALE 1

int main(void) {

    PWM_setup_timer(PWM_CHANNEL, PWM_PRESCALE);
    PWM_setup_period(PWM_PERIOD);
    PWM_setup_dual_edge(PWM_CHANNEL, PWM_STARTVAL, PWM_ENDVAL);
    PWM_start();
    while(1){};
    return 1;

}
