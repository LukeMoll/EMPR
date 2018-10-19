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
    char buf[10];
    uint16_t state = keypad_read();
    sprintf(buf, "[%05d]\r\n", state);
    write_usb_serial_blocking(buf, 10);
    char * fmtstr = format_keypad_state(state);
    write_usb_serial_blocking(fmtstr, 37);
    free(fmtstr);
    uint8_t pressed_key;
    if(read_diff(&pressed_key, state) != 0) {
        char bif[12];
        sprintf(bif, "key = %02d\r\n", pressed_key&0x0F);
        write_usb_serial_blocking(bif, 12);
        char bof[10];
        char key_symbol = get_ascii_character(pressed_key);
        sprintf(bof, "char = %c\r\n", key_symbol);
        write_usb_serial_blocking(bof, 10);

        lcd_clear_screen();
        uint8_t length = 32;
        char * message_dyn = malloc(length); // error check this later
        strncpy(message_dyn, "yay", length);

        uint8_t * bytes = lcd_a2b_in_place(message_dyn, length);

        lcd_write_bytes(message_dyn, length, 0x80);
        free(message_dyn);
        free(bytes);
    }
    else {
        write_usb_serial_blocking("No key\r\n", 9);
    }
    return 0;


}    

