#include "dac.h"

#include <lpc17xx_pinsel.h>
#include <lpc17xx_dac.h>
#include <lpc_types.h>

void dac_init(uint8_t port, uint8_t pin, uint8_t funcnum) {
    PINSEL_CFG_Type PinCfg;
    PinCfg.Funcnum = funcnum;
    PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL; // need to check these
    PinCfg.Pinmode = PINSEL_PINMODE_TRISTATE; // seems to work fine tho
    PinCfg.Portnum = port;
    PinCfg.Pinnum =  pin;

    PINSEL_ConfigPin(&PinCfg);
    DAC_Init(LPC_DAC);

}

uint16_t voltage_to_dac(float voltage) {
    float value_float = (((voltage)/3.3)*1024.0)-1.0;
    return (uint16_t)value_float;
}