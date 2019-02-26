#include <libempr/dac.h>

#define DAC_PORT 0
#define DAC_PIN 26
#define DAC_FUNCNUM 2

uint32_t tick = 0;

void advance() {
    
    tick++;
}