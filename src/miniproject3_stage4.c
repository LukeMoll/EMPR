#include <lpc17xx_pwm.h>
#include <lpc17xx_pinsel.h>
#include <lpc_types.h>

#include <lpc17xx_systick.h>
#include "status.h"

#define PWM_PORT 2
#define PWM_PIN 1
#define PWM_CHANNEL 2
#define PWM_ENDVAL 5000
#define PWM_STARTVAL 10000
#define PWM_PRESCALE 1

void PWM_setup_single_edge(void);
void PWM_start(void);

int main(void) {
    /**
     * it work
     */
    PWM_setup_single_edge();
    PWM_start();
}

void PWM_setup_single_edge(void) {

PWM_TIMERCFG_Type timerconfig;
    timerconfig.PrescaleOption = PWM_TIMER_PRESCALE_TICKVAL;
    timerconfig.PrescaleValue = 1;
    PWM_Init(LPC_PWM1, PWM_MODE_TIMER, &timerconfig);
    
    //configure the pins
    
    PINSEL_CFG_Type PinCfg;
    PinCfg.Funcnum = 1;
    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = 0;
    PinCfg.Portnum = PWM_PORT;
    PinCfg.Pinnum =  PWM_PIN;
    PINSEL_ConfigPin(&PinCfg);

    // configure Match 0 and Match 1

    PWM_MATCHCFG_Type matchconfig;
    PWM_MATCHCFG_Type matchconfig0;

    matchconfig0.IntOnMatch = DISABLE;
    matchconfig0.MatchChannel = 0;
    matchconfig0.ResetOnMatch = ENABLE;
    matchconfig0.StopOnMatch = DISABLE;
    PWM_MatchUpdate(LPC_PWM1, 0, PWM_STARTVAL, PWM_MATCH_UPDATE_NOW);
    PWM_ConfigMatch(LPC_PWM1, &matchconfig0); 

    PWM_ChannelConfig(LPC_PWM1, PWM_CHANNEL, PWM_CHANNEL_SINGLE_EDGE); 
    
    matchconfig.IntOnMatch = DISABLE;
    matchconfig.MatchChannel = PWM_CHANNEL;
    matchconfig.ResetOnMatch = DISABLE;
    matchconfig.StopOnMatch = DISABLE;
    PWM_MatchUpdate(LPC_PWM1, PWM_CHANNEL, PWM_ENDVAL, PWM_MATCH_UPDATE_NOW);
    PWM_ConfigMatch(LPC_PWM1, &matchconfig);

    PWM_ChannelCmd(LPC_PWM1, PWM_CHANNEL, ENABLE);
    
    //start everything

}

void PWM_start(void) {
    PWM_ResetCounter(LPC_PWM1);        
    PWM_CounterCmd(LPC_PWM1, ENABLE);   

    PWM_Cmd(LPC_PWM1, ENABLE);
}