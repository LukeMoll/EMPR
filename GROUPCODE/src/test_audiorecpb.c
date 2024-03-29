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


#define BUF_LEN 0x3000
#define PERIOD 100
uint16_t buf[BUF_LEN];
uint16_t seq = 0;
void init(){
    i2c_setup_polling();
    // digit_setPower(4);
    // digit_writeTwoHexBytes(0xFFFF);
    serial_init();
    serial_puts("Hello!\r\n");
    adc_setup(ADC_PORT, ADC_PIN, ADC_FUNCNUM, ADC_CHANNEL);

    RIT_setHandler(arc_rithandler);

    RIT_Init(LPC_RIT);
    RIT_TimerConfig_us(LPC_RIT, PERIOD);
    NVIC_EnableIRQ(RIT_IRQn);
    status_code(1);
}


void arc_rithandler(void) {
    buf[seq] = adc_read(ADC_CHANNEL);
    
    // if((seq & 0xF) == 0) {
    //     digit_writeTwoHexBytes(seq);
    //     digit_update();
    // }

    if(++seq >= BUF_LEN) {
        status_code(2);
        digit_clear();
        digit_update();
        NVIC_DisableIRQ(RIT_IRQn);
        playback_init(buf, BUF_LEN, PERIOD);
        playback_play();
    }

}