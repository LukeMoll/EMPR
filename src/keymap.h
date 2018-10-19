#ifndef EMPR_KEYMAP
#define EMPR_KEYMAP
#include <stdint.h>
char get_ascii_character(uint8_t key_pressed);
char keypad_keys[16];
#endif