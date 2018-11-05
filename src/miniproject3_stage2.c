#include <lpc17xx_pinsel.h>
#include <lpc17xx_dac.h>
#include <lpc17xx_systick.h>
#include <lpc17xx_gpio.h>
#include <lpc17xx_timer.h>

#include <math.h>
#include "status.h"

#define DIR_IN 0
#define DIR_OUT 1

#define DAC_PORT 0
#define DAC_PIN 26

#define SQR_PORT 0
#define SQR_PIN 16

void init(void);

/* TODO:
 * - sin lookup table (~ldm)
 * - Use timers to replace uint8_t tick
 * - Parameterise and vary frequency
 */

int main(void) {
    init();

    DAC_UpdateValue(LPC_DAC, 0xFFC);

    GPIO_SetDir(SQR_PORT, 1<<SQR_PIN, DIR_OUT);

    SYSTICK_InternalInit(1);
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);

    return 0;
}

void init(void) {
    PINSEL_CFG_Type PinCfg;
    PinCfg.Funcnum = 2;
    PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL; // need to check these
    PinCfg.Pinmode = PINSEL_PINMODE_TRISTATE; // seems to work fine tho
    PinCfg.Portnum = DAC_PORT;
    PinCfg.Pinnum =  DAC_PIN;

    PINSEL_ConfigPin(&PinCfg);
    DAC_Init(LPC_DAC);
    
    // TIM_TIMERCFG_Type TimCfg;
    // TimCfg.PrescaleOption = TIM_PRESCALE_TICKVAL;
    // TimCfg.PrescaleValue = 1000; // Timer scales to 1ms

    // TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &TimCfg);
    // TIM_Cmd(LPC_TIM0, ENABLE);
}


uint8_t tick = 0;
void SysTick_Handler(void) {
    SYSTICK_ClearCounterFlag();

    double tickPi = ((double)tick) * M_PI / 128.0;
    // Transform tick [0,255] to be in interval [0,2pi]

    uint16_t val = (uint16_t) (
        ( sin(tickPi) + 1 ) // sin x [-1,1]; sin x + 1 -> [0,2]
        * 511 // * 512 would cause 1+1*512 = 1024, which is out of range for 10 bits
    ); // [0, 1022] (close enough)

    DAC_UpdateValue(LPC_DAC, val);

    tick++; // We like the overflow >:)))
}