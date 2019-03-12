#include "audiorec.h"
#include "rithandler.h"
#include "adc.h"
#include <lpc17xx_rit.h>

void arec_rithandler(void);

static uint16_t *arec_buf, *arec_head, *arec_end;
static bool recording = false;


void recording_init(uint16_t *buf, size_t len, uint32_t sample_period) {
    arec_buf = buf;
    arec_head = arec_buf;
    arec_end = arec_buf + len * sizeof(buf[0]);
    adc_setup(ADC_PORT, ADC_PIN, ADC_FUNCNUM, ADC_CHANNEL);
    RIT_Init(LPC_RIT);
    RIT_TimerConfig_us(LPC_RIT, sample_period);
}

void recording_start() {
    recording = true;
    RIT_setHandler(arec_rithandler);
    NVIC_EnableIRQ(RIT_IRQn);
}
bool isrecording() {return recording;}

void arec_rithandler(void) {
    if(recording) {
        arec_head = adc_read(ADC_CHANNEL);
        arec_head++;
        if(arec_head >= arec_end) {
            NVIC_DisableIRQ(RIT_IRQn);
            recording = false;
        }
    }
}
