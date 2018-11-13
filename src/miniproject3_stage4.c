#include <lpc17xx_pwm.h>
#include <lpc17xx_pinsel.h>
#include <lpc_types.h>

#include <lpc17xx_systick.h>
#include "status.h"

#define PWM_PORT 2
#define PWM_PIN 1
#define PWM_CHANNEL 2
#define PWM_ENDVAL 500

int main(void) {
    /**
     * it don't work
     */
    PINSEL_CFG_Type PinCfg;
    PinCfg.Funcnum = 1;
    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = 0;
    PinCfg.Portnum = PWM_PORT;
    PinCfg.Pinnum =  PWM_PIN;
    PINSEL_ConfigPin(&PinCfg);

    PWM_TIMERCFG_Type timerconfig;
    // timerconfig.PrescaleOption = PWM_TIMER_PRESCALE_TICKVAL;
    // timerconfig.PrescaleValue = 1;
    timerconfig.PrescaleOption = PWM_TIMER_PRESCALE_USVAL;
    timerconfig.PrescaleValue = 1000; // 1ms
    PWM_Init(LPC_PWM1, PWM_MODE_TIMER, &timerconfig);

    PWM_MATCHCFG_Type matchconfig;
    /*
    matchconfig.IntOnMatch = DISABLE;
    matchconfig.MatchChannel = 0;
    matchconfig.ResetOnMatch = ENABLE;
    matchconfig.StopOnMatch = DISABLE;
    PWM_MatchUpdate(LPC_PWM1, PWM_CHANNEL, 0, PWM_MATCH_UPDATE_NOW);
    PWM_ConfigMatch(LPC_PWM1, &matchconfig); */

    matchconfig.IntOnMatch = DISABLE;
    matchconfig.MatchChannel = 2;
    matchconfig.ResetOnMatch = ENABLE;
    matchconfig.StopOnMatch = DISABLE;
    PWM_MatchUpdate(LPC_PWM1, 2, PWM_ENDVAL, PWM_MATCH_UPDATE_NOW);
    PWM_ConfigMatch(LPC_PWM1, &matchconfig);

    PWM_ChannelConfig(LPC_PWM1, PWM_CHANNEL, PWM_CHANNEL_SINGLE_EDGE);

    PWM_ChannelCmd(LPC_PWM1, PWM_CHANNEL, ENABLE);

    PWM_ResetCounter(LPC_PWM1);         // magic voodoo
    PWM_CounterCmd(LPC_PWM1, ENABLE);   // but might make it work

    PWM_Cmd(LPC_PWM1, ENABLE);

    SYSTICK_InternalInit(100);
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);

    while(1) {};

    return 0;
}

uint8_t tick = 0;
uint8_t systickcounter = 0;
void SysTick_Handler(void) {
    
    SYSTICK_ClearCounterFlag();
    // int start = PWM_ENDVAL;
    // int channel = 1;
    // PWM_MATCHCFG_Type PWMMatchCfgDat;
    // PWM_MatchUpdate(LPC_PWM1, 1, start, PWM_MATCH_UPDATE_NOW);
    // PWMMatchCfgDat.IntOnMatch = DISABLE;
    // PWMMatchCfgDat.MatchChannel = ((channel-1)*2 + 1);
    // PWMMatchCfgDat.ResetOnMatch = DISABLE;
    // PWMMatchCfgDat.StopOnMatch = DISABLE;
    // PWM_ConfigMatch(LPC_PWM1, &PWMMatchCfgDat);

    if((++systickcounter)%10 == 0) {
        status_code(tick = tick?0:1);
        PWM_ResetCounter(LPC_PWM1);         // magic voodoo
        PWM_CounterCmd(LPC_PWM1, ENABLE); 
    }

}