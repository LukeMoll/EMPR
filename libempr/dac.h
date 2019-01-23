#ifndef EMPR_DAC
#define EMPR_DAC

#include <stdint.h>

void dac_init(uint8_t port, uint8_t pin, uint8_t funcnum);
uint16_t voltage_to_dac(float voltage);

#endif
