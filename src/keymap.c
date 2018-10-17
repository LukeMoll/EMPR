#include keymap.h

constant char keypad_keys[0xF]{
    "D",
    "C",
    "B",
    "A",
    "#",
    "9",
    "6",
    "3",
    "0",
    "8",
    "5",
    "2",
    "*",
    "7", 
    "4",
    "1"
};

char get_ascii_characters(uint8_t key_pressed){
    uint8_t index = key_pressed & 0x0F;
    return keypad_keys[index];
}