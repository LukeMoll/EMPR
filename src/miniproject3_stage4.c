#include <lpc17xx_pwm.h>
#include <lpc17xx_pinsel.h>
#include <lpc_types.h>

#define PWM_PORT 2
#define PWM_PIN 0

int main(void) {
    /**
     * it don't work
     */
    PINSEL_CFG_Type PinCfg;
    PinCfg.Funcnum = 1;
    PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    PinCfg.Pinmode = PINSEL_PINMODE_TRISTATE;
    PinCfg.Portnum = PWM_PORT;
    PinCfg.Pinnum =  PWM_PIN;
    PINSEL_ConfigPin(&PinCfg);

    PWM_TIMERCFG_Type timerconfig;
    timerconfig.PrescaleOption = PWM_TIMER_PRESCALE_TICKVAL;
    timerconfig.PrescaleValue = 1000;
    PWM_Init(LPC_PWM1, PWM_MODE_TIMER, &timerconfig);

    PWM_MATCHCFG_Type matchconfig;
    matchconfig.IntOnMatch = DISABLE;
    matchconfig.MatchChannel = 1;
    matchconfig.ResetOnMatch = DISABLE;
    matchconfig.StopOnMatch = DISABLE;
    PWM_ConfigMatch(LPC_PWM1, &matchconfig);

    PWM_MatchUpdate(LPC_PWM1, 1, 1000, PWM_MATCH_UPDATE_NOW);

    PWM_Cmd(LPC_PWM1, ENABLE);

    while(1) {};

    return 0;
}