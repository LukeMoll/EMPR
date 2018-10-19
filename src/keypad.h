#ifndef EMPR_KEYPAD
#define EMPR_KEYPAD
#define KEYPAD_I2C_ADDR 0x21
#include <stdint.h>
uint16_t read_diff(uint8_t * pressedkey, uint16_t current_state);
uint16_t keypad_read();
char * format_keypad_state(uint16_t state);
#endif