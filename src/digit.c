#include <lpc17xx_systick.h>
#include "i2c.h"
#include <stdbool.h>

#define DIGIT_I2C_ADDR 0x38

// todo move to digit.h
void digit_setPower(uint8_t pwr);
void digit_enableSegmentTest(bool enabled);
void digit_setBits(uint8_t digit, uint8_t mask);
void digit_clearBits(uint8_t digit, uint8_t mask);
void digit_setByte(uint8_t digit, uint8_t byte);
void digit_writeHexByte(uint8_t val, bool high);
void digit_writeTwoHexBytes(uint16_t val);
uint8_t digit_digitToAddr(uint8_t digitNo);
Status digit_update();

/* 
 * 7 segment bit numbering
 *  where 7 is MSB and 0 is LSB:
 * 
 *    ---0---
 *   |       |
 *   5       1
 *   |       |
 *    ---6---
 *   |       |
 *   4       2
 *   |       |
 *    ---3---  (7)
 * 
 */

static uint8_t digit_bytes[6] = {
    0x00, // subadress 0
    0b01110111, // control register: full power (21mA), test OFF, no digits blanked, dynamic mode
    0x00, 0x00, 0x00, 0x00 // data registers: all segments off
};

static const uint8_t DIGIT_HEX_CHARS[] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2 
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8 
    0b01101111, // 9
    0b01011111, // A
    0b01111100, // b
    0b01011000, // c
    0b01011110, // d  
    0b01111001, // E
    0b01110001  // F
};

static const uint8_t DIGIT_CHAR_HYPEN   = 0b01000000;
static const uint8_t DIGIT_CHAR_DOT     = 0b10000000;

/**
 * Sets the segment current
 *  pwr: number from 0-7 (higher bits are ignored).
 *       note that 0 will give NO current to segments, even if they're on.
 */
void digit_setPower(uint8_t pwr) {
    digit_bytes[1] &= 0b10001111; // set pwr bits low first
    digit_bytes[1] |= (pwr & 0b111) << 4; // set C6-4 to pwr
}

/**
 * Enables (or disables) the built-in segment test, turning all segments on (regardless of data setting)
 *  enabled: true to enable the test, false to return to normal operation
 */
void digit_enableSegmentTest(bool enabled) {
    if(enabled) {
        digit_bytes[1] |=  0b1000; // set C3 high
    }
    else {
        digit_bytes[1] &= 0b11110111; // set C3 low
    }
}

/**
 * sets the bits specified in `mask` HIGH for digit `digit`
 *  digit: zero-indexed digit (0-3). Higher bits ignored
 *  mask: bits to set high. HIGH bits in mask will be set HIGH, LOW bits ignored.
 */
void digit_setBits(uint8_t digit, uint8_t mask) {
    digit_bytes[digit_digitToAddr(digit&0b11)] |= mask; // see expander.c
}

/**
 * sets the bits specified in `mask` LOW for digit `digit`
 *  digit: zero-indexed digit (0-3). Higher bits ignored
 *  mask: bits to set low. HIGH bits in mask will be set LOW, LOW bits ignored.
 */
void digit_clearBits(uint8_t digit, uint8_t mask) {
    digit_bytes[digit_digitToAddr(digit&0b11)] &= (~mask); // see expander.c
}

/**
 * sets the bits in digit `digit` to `byte`
 *  digit: zero-indexed digit (0-3). Higher bits ignored
 *  byte: new value for digit bits
 */
void digit_setByte(uint8_t digit, uint8_t byte) {
    digit_bytes[digit_digitToAddr(digit&0b11)] = byte;
}

/**
 * Writes a byte value to be displayed in hex on two of the digits.
 *  val: the value to represent
 *  high: whether to use the lower two digits (false, rightmost) or higher (true, leftmost)
 */
void digit_writeHexByte(uint8_t val, bool high) {
    uint8_t baseAddr = high ? 2 : 0;
    digit_setByte(baseAddr,   DIGIT_HEX_CHARS[ val&0xF ]);
    digit_setByte(baseAddr+1, DIGIT_HEX_CHARS[ (val>>4)&0xF ]);
}

/**
 * As digit_writeHexByte but writes a 16-bit value to all four digits.
 *  val: the value to represent
 */
void digit_writeTwoHexBytes(uint16_t val) {
    digit_writeHexByte(val & 0xFF, false);
    digit_writeHexByte((val >> 8) & 0xFF, true);
}

/**
 * Converts a digit number (0-3) into the correct byte number for digit_bytes
 *  digitNo: the digit number (0 is least significant, rightmost)
 *  returns: correct index for digit_bytes
 */
uint8_t digit_digitToAddr(uint8_t digitNo) {
    switch(digitNo % 4) {
        case 0: return 5;
        case 1: return 4;
        case 2: return 3;
        case 3: return 2;
    }
    // maybe use an array so we don't get complaints about non-void functions...
}

/**
 * Writes the digit data to the 7 segment controller
 * NOTE: other methods will have NO VISIBLE EFFECT until this method is called
 */
Status digit_update() {
    return i2c_write_multiple_bytes(DIGIT_I2C_ADDR, digit_bytes, 6);
}
