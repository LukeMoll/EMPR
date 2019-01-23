#ifndef EMPR_ADC
#define EMPR_ADC

#include <stdint.h>

#define MP3_ADC_PORT 0
#define MP3_ADC_PIN 23
#define MP3_ADC_CHANNEL 0
#define MP3_ADC_VREF 3.3

void adc_setup(uint8_t port, uint8_t pin, uint8_t channel);
uint16_t adc_read(uint8_t channel);
float to_voltage(uint16_t read_value);

#endif
