#include "i2c.h"
#include "serial.h"
#include "expander.h"
#include "keypad.h"
#include "keymap.h"
#include "lcd.h"

#include <stdio.h>
#include <status.h>

int main(void) {
    i2c_setup_polling();
    serial_init();
    lcd_init();

    lcd_clear_screen();
    uint8_t byte = lcd_ascii_to_byte("1");//works with offset 39, for some reason rather than 0x80
    uint8_t byte2 = lcd_ascii_to_byte("p"); // keeps printing lowercase t, or 0x74 on offset 0, and 0x55 on offset 0x80. Both should work.¯\_(ツ)_/¯ 
    uint8_t byte3 = lcd_ascii_to_byte("]");
    uint8_t byte4 = 0x98;
    lcd_write_byte(byte4, 0x80);
    // lcd_clear_screen();
    // lcd_write_byte(0x90, 0x81);

    char buf[100];
    uint8_t buf_length = snprintf(buf, 99, "byte: 0x%02x\r\n", byte3);
    write_usb_serial_blocking(buf, buf_length);

    return;
}    

