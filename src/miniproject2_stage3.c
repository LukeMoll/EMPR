#include "i2c.h"
#include "serial.h"
#include "expander.h"
#include "keypad.h"
#include "keymap.h"
#include "lcd.h"
#include "lcd_buf.h"

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

    char * message_dyn = malloc(length); 
    
    if(read_diff(&pressed_key, state) != 0) {

        //this works:

        char char_pressed = keymap_get_ascii_character(pressed_key);
       


        char buf[100];
        uint8_t buf_length = snprintf(buf, 99, "%c", char_pressed);
        
        write_usb_serial_blocking(buf, buf_length);

        lcd_buf_write_string(buf, buf_length, 0);
        lcd_buf_update();

    }
    else {
        
    }
    
    
    return 0;
}    


