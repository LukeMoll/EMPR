#include <lpc17xx_pinsel.h>
#include <lpc_types.h>
#include <stdio.h>
#include <lpc17xx_systick.h>

#include "adc.h"
#include "dac.h"

#define DAC_PORT 0
#define DAC_PIN 26
#define DAC_FUNCNUM 2

void Systick_Handler(void);


int main(void) {

    adc_setup(MP3_ADC_PORT, MP3_ADC_PIN, MP3_ADC_CHANNEL);

    dac_init(DAC_PORT, DAC_PIN, DAC_FUNCNUM);
    DAC_UpdateValue(LPC_DAC, 0xFFC);

    SYSTICK_InternalInit(1);
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);

    while(1) {} // stop the main thread from terminating

    return 1;
}

void SysTick_Handler(void) {
    
    SYSTICK_ClearCounterFlag();
    
    uint16_t data_plain = adc_read(MP3_ADC_CHANNEL);
    float voltage = to_voltage(data_plain);
    
    uint16_t dac_value = voltage_to_dac(voltage);
    DAC_UpdateValue(LPC_DAC, dac_value);
}


