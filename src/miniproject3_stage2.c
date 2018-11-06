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
uint16_t sine_wave(uint8_t x, uint8_t amplitudeDenom);
double lookup_sin(double piCoeff);
uint16_t transform10(double normalised);

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

}


uint8_t tick = 0;
uint16_t cycles = 0;

uint8_t freq_scale_factor = 1;
const uint8_t max_freq_scale_factor = 8;

uint8_t amp_scale_denom = 1;
const uint8_t max_amp_scale_denom = 16;

void SysTick_Handler(void) {
    SYSTICK_ClearCounterFlag();

    uint16_t val = sine_wave((tick * freq_scale_factor) % 256, amp_scale_denom);
    DAC_UpdateValue(LPC_DAC, val);

    if(tick == 0) {
        cycles++; // cycles is every 256 ticks - 1 second ~= 4 cycles
        if(cycles % 8 == 0) { // every 2 seconds, change the amplitude
            amp_scale_denom = amp_scale_denom < max_amp_scale_denom ? 
                amp_scale_denom + 1 :
                1;
        }
        if(cycles >= 40) { // every 10 seconds, increase the frequency
            cycles = 0;
            freq_scale_factor = freq_scale_factor < max_freq_scale_factor ?
                freq_scale_factor * 2 :
                1;
            amp_scale_denom = 1;
        }
    }

    tick++; // We like the overflow >:)))
}


/**
 * Generates a sine wave from a 8-bit tick
 *  tick: [0,255]
 *  
 *  returns: [0,1023]
 */
uint16_t sine_wave(uint8_t tick, uint8_t amplitudeDenom) {

    return transform10(
        lookup_sin( 
            ((double)tick) /
            128.0f
        ) 
        /
        ((double) amplitudeDenom)
    );
}

/**
 * Takes radian value (DIVIDED BY PI) and returns sine of value (in interval [-1.0, 1.0])
 */
double lookup_sin(double piCoeff) {
    return sin(piCoeff * M_PI);
}

/**
 * Takes normalised value (in interval [-1.0, 1.0]) and transforms for use with 10-bit ADC.
 */
uint16_t transform10(double normalised) {
    return (uint16_t) (
        ( normalised + 1.0) // [-1,1]; -> [0,2]
        * 511.5 // * 512 would cause 2*512 = 1024, which is out of range for 10 bits
    ); // [0, 1023]
}