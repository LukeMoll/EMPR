#include "rithandler.h"
#include <lpc17xx_rit.h>
#include <lpc17xx_dac.h>
#include "dac.h"
#include <stdlib.h>
#include "serial.h"

#include "audioplayback.h"

uint16_t *apb_buf, *apb_head, *apb_end;
uint8_t scale = 1;
uint16_t previous_val;
uint16_t new_val;

// private
void apb_rithandler(void);

void playback_init(uint16_t *buf, size_t len, uint32_t sample_period) {
    apb_buf = buf;
    apb_head = apb_buf;
    apb_end = apb_buf + len;

    dac_init();

    RIT_setHandler(apb_rithandler);

    RIT_Init(LPC_RIT);
    RIT_TimerConfig_us(LPC_RIT, sample_period); // 1000us = 1kHz
}

void playback_play() {
    NVIC_EnableIRQ(RIT_IRQn);
}

void playback_pause() {
    NVIC_DisableIRQ(RIT_IRQn);
}



void apb_rithandler(void) {
    if(apb_head != apb_end && NULL != apb_head) {
        DAC_UpdateValue(LPC_DAC, (*apb_head) >> scale);
        apb_head++;
    }
    else {
        playback_pause();
        free(apb_buf); // don't need to free it if it's on the stack
        apb_buf = NULL;
        apb_head = NULL;
        apb_end = NULL;
    }
#if CW_DEMO
    new_val = (*apb_head) << scale;
    if(previous_val - new_val >= CW_DEMO_DELTA) {
        serial_puts(*new_val);
        previous_val = new_val;
    }
#endif
}