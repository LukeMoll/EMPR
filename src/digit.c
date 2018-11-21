#include <lpc17xx_systick.h>
#include "i2c.h"
#include <stdbool.h>
#include "status.h"

#define DIGIT_I2C_ADDR 0x38

// test code only
int main(void) {
    i2c_setup_polling();

    SYSTICK_InternalInit(100);
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE); 

    while(1) {};
    return 1;
}
// *******************************************************************

uint8_t digit_bytes[6] = {
    0x00, // subadress 0
    0b01110111, // control register: full power (21mA), test OFF, no digits blanked, dynamic mode
    0x00, 0x00, 0x00, 0x00 // data registers: all segments off
};

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
    digit_bytes[2 + digit&0b11] |= mask; // see expander.c
}

/**
 * sets the bits specified in `mask` LOW for digit `digit`
 *  digit: zero-indexed digit (0-3). Higher bits ignored
 *  mask: bits to set low. HIGH bits in mask will be set LOW, LOW bits ignored.
 */
void digit_clearBits(uint8_t digit, uint8_t mask) {
    digit_bytes[2 + digit&0b11] &= (~mask); // see expander.c
}

/**
 * sets the bits in digit `digit` to `byte`
 *  digit: zero-indexed digit (0-3). Higher bits ignored
 *  byte: new value for digit bits
 */
void digit_setByte(uint8_t digit, uint8_t byte) {
    digit_bytes[2 + digit&0b11] = byte;
}


/**
 * Writes the digit data to the 7 segment controller
 * NOTE: other methods will have NO VISIBLE EFFECT until this method is called
 */
Status digit_update() {
    return i2c_write_multiple_bytes(DIGIT_I2C_ADDR, digit_bytes, 6);
}

uint8_t digit_test_power = 0;
void SysTick_Handler(void) {
    SYSTICK_ClearCounterFlag();

    status_code(1);
    digit_setPower(
        (digit_test_power = (digit_test_power + 1) % 7)
    );
    digit_enableSegmentTest(true);
    status_code(2);

    // i2c_write_multiple_bytes(DIGIT_I2C_ADDR, digit_bytes, 1);
    digit_update();  
}