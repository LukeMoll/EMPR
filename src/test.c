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
    uint8_t byte = lcd_ascii_to_byte("1");
    lcd_write_byte(byte, 0x80);

    char buf[100];
    uint8_t buf_length = snprintf(buf, 99, "byte: 0x%02x\r\n", byte);
    write_usb_serial_blocking(buf, buf_length);

    return;
}    

