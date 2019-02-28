#ifndef EMPR_WAVE
#define EMPR_WAVE
    #include <stdint.h>
    #define WTICK8  3
    #define WTICK16 4
    #define WTICK32 5
    uint32_t wave_triangle32(uint8_t Tres, uint64_t tick);
    uint16_t wave_t8saw16(uint8_t tick);
    uint16_t wave_t8triangle16(uint8_t tick);
#endif