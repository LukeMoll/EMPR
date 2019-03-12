#include "adc.h"

#include <lpc17xx_adc.h>
#include <lpc17xx_pinsel.h>
#include <lpc_types.h>

void adc_setup(uint8_t port, uint8_t pin, uint8_t funcnum, uint8_t channel) {
    // Set up the input pin
    PINSEL_CFG_Type PinCfg;
    PinCfg.Funcnum = funcnum;
    PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    PinCfg.Pinmode = PINSEL_PINMODE_TRISTATE; 
    PinCfg.Portnum = port;
    PinCfg.Pinnum =  pin;

    PINSEL_ConfigPin(&PinCfg);

    ADC_Init(LPC_ADC, 200000);
    
    ADC_StartCmd(LPC_ADC, ADC_START_NOW);
    ADC_ChannelCmd(LPC_ADC, channel, ENABLE);
}

uint16_t adc_read(uint8_t channel) {
    /**
     * ADC seems to have strange voltage response with high resistances (1000's of ohms)
     */
    ADC_StartCmd(LPC_ADC, ADC_START_NOW); // might be needed for multiple reads?
    // FlagStatus status_chan = // can assign the status if you want
    ADC_ChannelGetStatus(LPC_ADC, channel, 1); // this is needed here, for some reason ¯\_(ツ)_/¯
    return ADC_GetData(channel);
}

float to_voltage(uint16_t read_value) {
    return (((float) read_value) * MP3_ADC_VREF) / 4095.0;
}
