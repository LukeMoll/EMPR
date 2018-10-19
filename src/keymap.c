#include "keymap.h"


char keypad_keys[16] = "DCBA#9630852*741";
// {
//     "D",
//     "C",
//     "B",
//     "A",
//     "#",
//     "9",
//     "6",
//     "3",
//     "0",
//     "8",
//     "5",
//     "2",
//     "*",
//     "7", 
//     "4",
//     "1"
// };

char get_ascii_character(uint8_t key_pressed){
    uint8_t index = key_pressed & 0x0F;
    return (keypad_keys[index]);
}