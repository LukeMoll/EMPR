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
        

        char char_pressed = get_ascii_character(pressed_key);
        uint8_t byte_to_send = lcd_ascii_to_byte(char_pressed);
        
        lcd_write_byte(byte_to_send, 0x80);

        //something isn't working in the following lines, can't tell what.

        // strncpy(message_dyn, char_pressed, length);
        // uint8_t * bytes = lcd_a2b_in_place(message_dyn, length);
        // lcd_write_byte(message_dyn[0], 0x80);

        char prebuf[100];
        uint8_t prebuf_length = snprintf(prebuf, 99, "char = %c, byte = %02x\r\n", char_pressed, byte_to_send);
        write_usb_serial_blocking(prebuf, prebuf_length);
        free(bytes);
    }
    else {
        strncpy(message_dyn, "-", length);
    }
    free(message_dyn);
    
    return 0;
}    

