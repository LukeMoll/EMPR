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

    uint16_t state = keypad_read();
    uint8_t pressed_key;

    lcd_clear_screen();
    uint8_t length = 1;
    char * message_dyn = malloc(length); // error check this later
    if(read_diff(&pressed_key, state) != 0) {
        strncpy(message_dyn, get_ascii_character(pressed_key), length);

        char prebuf[100];
        uint8_t prebuf_length = snprintf(prebuf, 99, "pre:     0x%0x\r\n", lcd_ascii_to_byte(get_ascii_character(pressed_key)));
        write_usb_serial_blocking(prebuf, prebuf_length);

    }
    else {
        strncpy(message_dyn, "-", length);
    }

    uint8_t * nip_bytes = lcd_a2b(message_dyn, length);
    uint8_t * bytes = lcd_a2b_in_place(message_dyn, length);
    uint8_t single_byte = lcd_ascii_to_byte(message_dyn[0]);
    uint8_t test_byte = 0x7f;
    lcd_write_bytes(&test_byte, 1, 0x80);
    lcd_clear_screen();
    lcd_write_bytes(&single_byte, 1, 0x80);
    char postbuf[100];
    uint8_t postbuf_length = snprintf(postbuf, 99, "post:  ip 0x%0x\r\npost: nip 0x%0x\r\n\r\n", bytes[0], nip_bytes[0]);
    write_usb_serial_blocking(postbuf, postbuf_length);

    free(message_dyn);
    free(bytes);
    return 0;
}    

