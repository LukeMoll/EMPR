#include "i2c.h"
#include "serial.h"

#include <stdio.h>
#include <status.h>

#define KEYPAD_I2C_ADDR 0x21
// this could be 0x20, not sure

uint8_t expander_read(uint8_t addr);
void expander_set(uint8_t addr, uint8_t bits);
void expander_clear(uint8_t addr, uint8_t bits);

uint16_t keypad_read();

int main(void) {
    i2c_setup_polling();
    serial_init();
    write_usb_serial_blocking("Hi\n\r",5);
    char buf[10];
    status_code(1);
    sprintf(buf, "[%d]\n\r", keypad_read());
    status_code(2);
    write_usb_serial_blocking(buf, 10);
    status_code(0);
    return 0;
}

uint8_t expander_read(uint8_t addr) {
    uint8_t * dest;
    (*dest) = 0; // idk if this is required
    i2c_read_byte(addr, dest);
    return (*dest);
}

void expander_set(uint8_t addr, uint8_t bits) {
    uint8_t state = expander_read(addr);
    uint8_t state = 0;
    state |= bits; // Any HIGH bits in `bits` should be set high, otherwise keep it the same
    i2c_write_byte(addr, state);
}

void expander_clear(uint8_t addr, uint8_t bits) {
    uint8_t state = expander_read(addr);
    state &= (~bits); // Any HIGH bits in `bits` should be set low, otherwise ignore them
    i2c_write_byte(addr, state);
}

uint16_t keypad_read() {
    expander_set(KEYPAD_I2C_ADDR, 0b0);
    status_code(15);
    uint8_t readPins = 0x0F;
    int i;
    uint8_t buttons;
    uint16_t result = 0;
    for(i=0; i<4; i++) {
        status_code(i<<2);
        expander_set(KEYPAD_I2C_ADDR, (1<<(i+4)) | readPins); // set ONLY pin y high
        buttons = expander_read(KEYPAD_I2C_ADDR) & readPins; // filter out the high pins, safe for assigning
        result |= buttons << (i*4);
        expander_clear(KEYPAD_I2C_ADDR, 0xF0);
    }
    return result;
}

// char * format_keypad_state(uint16_t state) {
//     char[37] buf;
//     char * bufp = buf;

//     int i;
//     for(i = 0; i < 4; i++) {
//         // uint8_t 
//         // bufp += sprintf(bufp, "%d %d %d %d\n\r", )
//     }

//     return buf;
// }