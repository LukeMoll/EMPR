#include "i2c.h"
#include "serial.h"
#include "expander.h"
#include "keypad.h"
#include "keymap.h"
#include "lcd.h"

#include <stdio.h>
#include <status.h>

void busy_wait();

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

        //this works:

        char char_pressed = keymap_get_ascii_character(pressed_key);
        uint8_t byte_to_send = lcd_ascii_to_byte(char_pressed);
       
        char buf[100];
        uint8_t buf_length = snprintf(buf, 99, "%c", char_pressed);
        strncpy(message_dyn, buf, length);

        uint8_t * bytes = lcd_a2b_in_place(message_dyn, length);

        lcd_write_bytes_neo(bytes, length, 0x80);
        free(message_dyn);
        free(bytes);
    }
    else {
        // lcd_write_byte(lcd_ascii_to_byte(']'));
    }
    //free(message_dyn);
    
    return 0;
}    


