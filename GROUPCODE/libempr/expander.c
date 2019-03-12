#include "i2c.h"

#include "expander.h"

uint8_t expander_read(uint8_t addr) {
    uint8_t dest = 0;
    i2c_read_byte(addr, &dest);
    return dest;
}

void expander_set(uint8_t addr, uint8_t bits) {
    uint8_t state = expander_read(addr);
    state |= bits; // Any HIGH bits in `bits` should be set high, otherwise keep it the same
    i2c_write_byte(addr, state);
}

void expander_clear(uint8_t addr, uint8_t bits) {
    uint8_t state = expander_read(addr);
    state &= (~bits); // Any HIGH bits in `bits` should be set low, otherwise ignore them
    i2c_write_byte(addr, state);
}