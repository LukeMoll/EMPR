// #include "expander.h"
// #include <stdio.h>

#include "keypad.h"
#include <stdint.h>
#include "expander.h"

uint16_t keypad_read() {
    expander_set(KEYPAD_I2C_ADDR, 0b0);
    uint8_t readPins = 0x0F;
    uint8_t i;
    uint8_t buttons;
    uint16_t result = 0;
    expander_set(KEYPAD_I2C_ADDR, 0xFF); // bring everything high
    for(i=0; i<4; i++) {
        expander_clear(KEYPAD_I2C_ADDR, 1<<(i+4)); // set p[i+4] LOW
        buttons = expander_read(KEYPAD_I2C_ADDR) & readPins; // filter out the high pins, safe for assigning
        result |= buttons << (i*4);
        expander_set(KEYPAD_I2C_ADDR, 0xFF); // and bring it all high again
    }
    return result;
}

static uint16_t previous_keypad_state = 0xFFFF;

uint16_t read_diff(uint8_t * pressedkey, uint16_t current_state) {
    uint16_t ret;
    uint16_t delta;
    delta = ((~current_state) & previous_keypad_state);
    // combine current_state and pks to get delta
    char buf[12];
    if(delta != 0) { // if delta != 0
        ret = delta;    // something nonzero and (optinoally) meaningful
        uint8_t i;
        i = 0;
        // find INDEX of least significant nonzero bit of delta
        while((delta && 0x01 != 0) && (i<=16)) {
            delta = delta >> 1;
            i++;
        }
        
        (*pressedkey) = i-1; //that bit
    }
    else {
        ret = delta; // nothing more to do
    }
    // finally, after all processing
    previous_keypad_state = current_state;
    return ret;
}

char * format_keypad_state(uint16_t state) {
    char * buf = malloc(37); // We can error check this later
    char * bufp = buf;

    int i;
    for(i = 0; i < 4; i++) {
        uint8_t nib = ((state >> i*4) & 0xF);
        bufp += sprintf(bufp, "%d %d %d %d\r\n", 
            (nib >> 3) & 1,
            (nib >> 2) & 1,
            (nib >> 1) & 1,
            nib & 1
        );
    }

    return buf;
}
