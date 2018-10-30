#include <lpc17xx_adc.h>
#include <lpc17xx_pinsel.h>
#include <lpc_types.h>
#include <stdio.h>
#include "serial.h"

#define MP3_ADC_PORT 0
#define MP3_ADC_PIN 23
#define MP3_ADC_CHANNEL 0

int main(void) {
    serial_init();

    // Set up the input pin
    PINSEL_CFG_Type PinCfg;
    PinCfg.Funcnum = 1;
    PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
    PinCfg.Portnum = MP3_ADC_PORT;
    PinCfg.Pinnum =  MP3_ADC_PIN;

    PINSEL_ConfigPin(&PinCfg);

    ADC_Init(LPC_ADC, 200000);

    ADC_ChannelCmd(LPC_ADC, MP3_ADC_CHANNEL, ENABLE);
    ADC_StartCmd(LPC_ADC, ADC_START_NOW);

    FlagStatus status_chan = ADC_ChannelGetStatus(LPC_ADC, MP3_ADC_CHANNEL, 1);
    uint16_t data_chan = ADC_ChannelGetData(LPC_ADC, MP3_ADC_CHANNEL);
    uint16_t data_plain = ADC_GetData(MP3_ADC_CHANNEL);
    
    FlagStatus status_global = ADC_GlobalGetStatus(LPC_ADC, 1);

    float voltage = (((float) data_plain) * 3) / 4095.0;

    char buf[200];
    write_usb_serial_blocking(buf, snprintf(buf, 199, "\r\nchannel:\t%u\t(status %d)\r\nplain:\t\t%u\r\nvoltage:\t%f\r\n", data_chan, status_chan, data_plain, voltage));

    return 0;
}