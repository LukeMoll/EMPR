#include "keymap_none_blocking.h"


char keypad_keys[16] = "DCBA#9630852*741";
int8_t keypad_numbers[16] = {0xD,0xC,0xB,0xA,-1,9,6,3,0,8,5,2,-2,7,4,1};
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

char keymap_get_ascii_character(uint8_t key_pressed){
    uint8_t index = key_pressed & 0x0F;
    return (keypad_keys[index]);
}

/**
 * Returns a usable integer representation of the pressed key, as follows:
 *  "0"-"9": the numbers 0-9
 *  "A"-"D": the values 0xA-0xD
 *  "*","#": the numbers -1 and -2 respectively
 * The * and # keys are assigned values such that to filter them out, the
 * programmer only need to see if the value returned is positive.
 */
int8_t keymap_get_number(uint8_t key_pressed) {
    return keypad_numbers[key_pressed & 0x0F];
}