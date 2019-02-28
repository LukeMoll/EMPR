#include <libempr/dac.h>
#include <libempr/wave.h>
#include <lpc17xx_dac.h>
#include <lpc17xx_rit.h>
#include <lpc17xx_clkpwr.h>

#include <libempr/serial.h>

#define DAC_PORT 0
#define DAC_PIN 26
#define DAC_FUNCNUM 2


uint32_t RIT_TimerConfig_us(LPC_RIT_TypeDef *RITx, uint32_t microseconds);
void advance();

int main(void) {
    serial_init();

    dac_init(DAC_PORT, DAC_PIN, DAC_FUNCNUM);

    RIT_Init(LPC_RIT);
    RIT_TimerConfig_us(LPC_RIT, 15);
    NVIC_EnableIRQ(RIT_IRQn);
    while(1) {};
    return 0;

}

uint32_t RIT_TimerConfig_us(LPC_RIT_TypeDef *RITx, uint32_t microseconds) {
    	uint32_t clock_rate, cmp_value;
    	// CHECK_PARAM(PARAM_RITx(RITx));

		// remove divider
		CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_RIT, 1);

        // Get PCLK value of RIT
    	clock_rate = CLKPWR_GetPCLK(CLKPWR_PCLKSEL_RIT);

        /* calculate compare value for RIT to generate interrupt at
         * specified time interval
         * COMPVAL = (RIT_PCLK * time_interval)/1000
         * (with time_interval unit is millisecond)
         */
        cmp_value = (clock_rate /1000000) * microseconds;
        RITx->RICOMPVAL = cmp_value;

        /* Set timer enable clear bit to clear timer to 0 whenever
         * counter value equals the contents of RICOMPVAL
         */
        RITx->RICTRL |= (1<<1);
		return cmp_value;
}

uint8_t tick = 0;
void advance() {
    // uint32_t newVal = wave_triangle32(WTICK8, tick);

    uint32_t newVal = wave_t8saw16(tick) 
        + wave_t8saw16(tick * 3/2) / 4
        // + wave_t8saw16(tick * 9)/9
        // + wave_t8saw16(tick * 27)/27
        ;
    DAC_UpdateValue(LPC_DAC, newVal >> 8);
    tick ++;
    
    // if((tick % 15) == 0) {
        // serial_printf("%u:\t%u\t%03x\r\n", tick, newVal, newVal >> 20);
    // }
}

void RIT_IRQHandler(void) {
	RIT_GetIntStatus(LPC_RIT);

    advance();
}