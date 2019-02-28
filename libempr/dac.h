#ifndef EMPR_DAC
#define EMPR_DAC

#include <stdint.h>

#define DAC_PORT 0
#define DAC_PIN 26
#define DAC_FUNCNUM 2

void dac_init();
uint16_t voltage_to_dac(float voltage);

#endif
