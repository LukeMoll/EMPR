#include "i2c.h"
#include "lcd_buf.h"

#ifndef LCD_I2C_ADDR
    #define LCD_I2C_ADDR 0x3b
#endif

int main(void) {
    i2c_setup_polling();

    lcd_init();
    lcd_buf_flush();

    i2c_write_multiple_bytes(LCD_I2C_ADDR, (uint8_t[]) {
        0x00, // command byte
        0x34, // summat about instructions, next byte will be a command
        0x40  // set CGRAM address to 0
    },3);


    i2c_write_multiple_bytes(LCD_I2C_ADDR, (uint8_t[]) {        
        0x40, // next byte is data to write to CGRAM 0
        // 0x06,
        // 0x09,
        // 0x08,
        // 0x1E,
        // 0x1E,
        // 0x08,
        // 0x09,
        // 0x06
        0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F
    }, 9);

    i2c_write_multiple_bytes(LCD_I2C_ADDR, (uint8_t[]) {
        0x00,
        0x25,
        0x06,
        0x08,
        0x40,
        0xA0,
        0x24,
        0x0C,
        0x06,
        0x02,
        0x01
    }, 11);

    i2c_write_multiple_bytes(LCD_I2C_ADDR, (uint8_t[]) {
        0x40,
        0x00
    }, 2);

    // lcd_buf_write_string("Hello, world! \x11", 15, 0);
    // lcd_buf_write_bytes((uint8_t[]) {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}, 16, 16);
    // lcd_buf_update();
}