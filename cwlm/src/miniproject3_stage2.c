#include <lpc17xx_pinsel.h>
#include <lpc17xx_dac.h>
#include <lpc17xx_systick.h>
#include <lpc17xx_gpio.h>
#include <lpc17xx_timer.h>
#include "dac.h"

#include "generatedfuncs.h"
#include "status.h"

#define DIR_IN 0
#define DIR_OUT 1

#define DAC_PORT 0
#define DAC_PIN 26
#define DAC_FUNCNUM 2

#define SQR_PORT 0
#define SQR_PIN 16

uint16_t sine_wave(uint8_t x, uint8_t amplitudeDenom);
double lookup_sin(double piCoeff);
uint16_t transform10(double normalised);

/* TODO:
 * - sin lookup table (~ldm)
 * - Use timers to replace uint8_t tick
 * - Parameterise and vary frequency
 */

int main(void) {
    dac_init(DAC_PORT, DAC_PIN, DAC_FUNCNUM);

    DAC_UpdateValue(LPC_DAC, 0xFFC);

    GPIO_SetDir(SQR_PORT, 1<<SQR_PIN, DIR_OUT);

    SYSTICK_InternalInit(1);
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);

    while(1){};

    return 0;
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
        // if(cycles % 8 == 0) { // every 2 seconds, change the amplitude
        //     amp_scale_denom = amp_scale_denom < max_amp_scale_denom ? 
        //         amp_scale_denom + 1 :
        //         1;
        // }
        // if(cycles >= 40) { // every 10 seconds, increase the frequency
        //     cycles = 0;
        //     freq_scale_factor = freq_scale_factor < max_freq_scale_factor ?
        //         freq_scale_factor * 2 :
        //         1;
        //     amp_scale_denom = 1;
        // }
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
    return GENERATED_SIN_INT16[tick]/(amplitudeDenom*64) + 512;
}
