#include "wave.h"

uint32_t wave_triangle32(uint8_t Tres, uint64_t tick) {
    uint8_t Vres = 5;
    uint32_t Vmax = ((1 << 32) - 1);

    Tres &= 0b111;
    uint64_t Tmax = 1 << Tres;

    if(tick < (Tmax / 4)) {
        return (1 << (Vres - Tres + 1)) * tick;
    }
    else if(tick < 3 * (Tmax / 4)) {
        return Vmax - ((1 << (Vres - Tres + 1)) * tick);
    }
    else {
        return ((1 << (Vres - Tres + 1)) * tick) - (2 * Vmax);
    }
}

uint16_t wave_t8triangle16(uint8_t tick) {
    if(tick < 64) {
        // [0,2^6] -> [2^15, 2^16]
        return (tick << 9) + (1 << 15);
    }
    else if(tick < 192) {
        return (1 << 16) - (tick<<9);
    }
    else {
        return (tick << 9);
    }
}

uint16_t wave_t8saw16(uint8_t tick) {
    return tick << 8;
}