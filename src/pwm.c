#include "pwm.h"

#include <lpc17xx_pwm.h>
#include <lpc17xx_pinsel.h>
#include <lpc_types.h>

#include "status.h"

void PWM_setup_single_edge(uint8_t channel, uint32_t period, uint32_t length, uint32_t prescale) {

if(channel>6) {
    //checks if you're using a valid channel
    status_code(5);
}

PWM_TIMERCFG_Type timerconfig;
    timerconfig.PrescaleOption = PWM_TIMER_PRESCALE_TICKVAL;
    timerconfig.PrescaleValue = prescale;
    PWM_Init(LPC_PWM1, PWM_MODE_TIMER, &timerconfig);
    
    //configure the pins
    
    PINSEL_CFG_Type PinCfg;
    PinCfg.Funcnum = PWM_FUNCNUM;
    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = 0;
    PinCfg.Portnum = PWM_PORT;
    PinCfg.Pinnum =  channel-1;
    PINSEL_ConfigPin(&PinCfg);

    // configure Match 0 and Match 1

    PWM_MATCHCFG_Type matchconfig;
    PWM_MATCHCFG_Type matchconfig0;

    matchconfig0.IntOnMatch = DISABLE;
    matchconfig0.MatchChannel = 0;
    matchconfig0.ResetOnMatch = ENABLE;
    matchconfig0.StopOnMatch = DISABLE;
    PWM_MatchUpdate(LPC_PWM1, 0, period, PWM_MATCH_UPDATE_NOW);
    PWM_ConfigMatch(LPC_PWM1, &matchconfig0); 

    PWM_ChannelConfig(LPC_PWM1, channel, PWM_CHANNEL_SINGLE_EDGE); 
    
    matchconfig.IntOnMatch = DISABLE;
    matchconfig.MatchChannel = channel;
    matchconfig.ResetOnMatch = DISABLE;
    matchconfig.StopOnMatch = DISABLE;
    PWM_MatchUpdate(LPC_PWM1, channel, length, PWM_MATCH_UPDATE_NOW);
    PWM_ConfigMatch(LPC_PWM1, &matchconfig);

    PWM_ChannelCmd(LPC_PWM1, channel, ENABLE);

}

void PWM_start(void) {
    PWM_ResetCounter(LPC_PWM1);        
    PWM_CounterCmd(LPC_PWM1, ENABLE);   

    PWM_Cmd(LPC_PWM1, ENABLE);
}