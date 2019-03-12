#ifndef EMPR_I2C_EXPANDER
#define EMPR_I2C_EXPANDER
uint8_t expander_read(uint8_t addr);
void expander_set(uint8_t addr, uint8_t bits);
void expander_clear(uint8_t addr, uint8_t bits);
#endif