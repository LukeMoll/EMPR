#include <stdint.h>

#define WTICK8  3
#define WTICK16 4
#define WTICK32 5

uint32_t wave_triangle32(uint8_t Tres, uint64_t tick) {
    uint8_t Vres = 5;

    Tres &= 0b111;
    uint64_t Tmax = 1 << Tres;

    if(tick < (Tres / 4)) {
        // return 1 << 
    }
    else if(tick < 3 * (Tres / 4)) {

    }
    else {

    }
}