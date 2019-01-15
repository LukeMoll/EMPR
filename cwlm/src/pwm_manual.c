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

    PINSEL_CFG_Type PinCfg;
    PinCfg.Funcnum = 1;
    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = 0;
    PinCfg.Portnum = PWM_PORT;
    PinCfg.Pinnum =  PWM_PIN;
    PINSEL_ConfigPin(&PinCfg);

//this doesn't do anything. It should, it doesn't.

    LPC_PWM1->LER = 0x0000007F; //enable loading to all registers
    LPC_PWM1->PR = 0x00000001; //sets prescale value to 1
    LPC_PWM1->MCR=0x00000002; //enables reset if TC = MRO
    LPC_PWM1->CTCR=0x00000000; //timer mode
    LPC_PWM1->CCR=0x00000000; // no capture
    LPC_PWM1->PCR=0x00007E00; //sets all channels to single edge, enables outputs from all channels
    LPC_PWM1->MR0=0x00001388; // sets reset value to 5000 (ms)
    LPC_PWM1->MR1=0x000003E8; // sets edge value to 1000 (ms)
    LPC_PWM1->TCR = 0x0000000B; //enable TC and PC for counting, reset at positive edge, enable PWM

    while(1){}
    return 0;
}