#include <libempr/rithandler.h>
#include <lpc17xx_rit.h>
#include <lpc17xx_clkpwr.h> // https://youtu.be/h-mUGj41hWA
#include <lpc17xx_gpio.h>
#include <stdbool.h>
#include <libempr/serial.h>
#include <libempr/status.h>
#include <libempr/i2c.h>
#include <libempr/digit.h>
#include <libempr/audioplayback.h>
#include <libempr/adc.h>
#include <stddef.h>


void init();
void arc_rithandler(void);

void main(void) {
    init();
    while(1) {};
    return;
}

uint8_t period_us = PLAYBACK_4KHZ;

#define BUF_LEN 0x2000
uint16_t buf[BUF_LEN];
uint16_t seq = 0;
void init(){
    i2c_setup_polling();
    digit_setPower(4);
    digit_writeTwoHexBytes(0xFFFF);
    serial_init();
    serial_puts("Hello!\r\n");
    adc_setup(ADC_PORT, ADC_PIN, ADC_FUNCNUM, ADC_CHANNEL);

    RIT_setHandler(arc_rithandler);

    RIT_Init(LPC_RIT);
    RIT_TimerConfig_us(LPC_RIT, period_us);
    NVIC_EnableIRQ(RIT_IRQn);
    status_code(3);
}

void dumpbuffer(uint16_t * buf, uint16_t len) {
    serial_puts("BUFFER DUMP START\r\n");
    serial_printf("length:%d\r\n", len);
    serial_printf("periodus:%d\r\n", period_us);
    uint16_t i;
    for(i=0; i<len; i++) {
        serial_printf("0x%x\r\n", buf[i]);
        if((seq & 0xF) == 0) {
            digit_writeTwoHexBytes(i);
            digit_setBits(2, 0b10000000); // this should be in digit.h not .c but I cbb
            digit_update();
        }
    }
    serial_puts("BUFFER DUMP END\r\n");
}

void arc_rithandler(void) {
    buf[seq] = adc_read(ADC_CHANNEL) << 4;
    
    if((seq & 0xF) == 0) {
        digit_writeTwoHexBytes(seq);
        digit_setBits(1, 0b10000000);
        digit_update();
    }

    if(++seq >= BUF_LEN) {
        status_code(2);
        digit_clear();
        digit_update();
        NVIC_DisableIRQ(RIT_IRQn);
        dumpbuffer(buf, BUF_LEN);
        seq = 0;
        status_code(1);
        NVIC_EnableIRQ(RIT_IRQn);
    }

}