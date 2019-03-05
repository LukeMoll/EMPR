#include <lpc17xx_rit.h>
#include <lpc17xx_clkpwr.h>
#include <lpc17xx_dac.h>
#include "dac.h"
#include <stdlib.h>

#include "audioplayback.h"

uint8_t *apb_buf, *apb_head, *apb_end;
uint8_t scale = 0;

// private
uint32_t RIT_TimerConfig_us(LPC_RIT_TypeDef *RITx, uint32_t microseconds);

void playback_init(uint8_t *buf, size_t len, uint32_t sample_period) {
    apb_buf = buf;
    apb_head = apb_buf;
    apb_end = apb_buf + len;

    dac_init();

    RIT_Init(LPC_RIT);
    RIT_TimerConfig_us(LPC_RIT, sample_period); // 1000us = 1kHz
}

void playback_play() {
    NVIC_EnableIRQ(RIT_IRQn);
}

void playback_pause() {
    NVIC_DisableIRQ(RIT_IRQn);
}

uint32_t RIT_TimerConfig_us(LPC_RIT_TypeDef *RITx, uint32_t microseconds) {
    	uint32_t clock_rate, cmp_value;

		// remove divider
		CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_RIT, 1);

        // Get PCLK value of RIT
    	clock_rate = CLKPWR_GetPCLK(CLKPWR_PCLKSEL_RIT);

        /* calculate compare value for RIT to generate interrupt at
         * specified time interval
         * COMPVAL = (RIT_PCLK * time_interval)/1000
         * (with time_interval unit is millisecond)
         */
        cmp_value = (clock_rate / 1000000) * microseconds;
        RITx->RICOMPVAL = cmp_value;

        /* Set timer enable clear bit to clear timer to 0 whenever
         * counter value equals the contents of RICOMPVAL
         */
        RITx->RICTRL |= (1<<1);
		return cmp_value;
}

void RIT_IRQHandler(void) {
	RIT_GetIntStatus(LPC_RIT);

    if(apb_head != apb_end && NULL != apb_head) {
        DAC_UpdateValue(LPC_DAC, (*apb_head) << scale);
        apb_head++;
    }
    else {
        playback_pause();
        free(apb_buf);
        apb_buf = NULL;
        apb_head = NULL;
        apb_end = NULL;
    }
}