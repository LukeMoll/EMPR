#ifndef EMPR_KEYPAD
#define EMPR_KEYPAD
char * format_keypad_state(uint16_t state);
uint16_t read_diff(uint8_t * pressedkey, uint16_t current_state);
uint16_t keypad_read();
#endif