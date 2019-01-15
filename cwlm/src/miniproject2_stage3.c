#include "i2c.h"
#include "serial.h"
#include "expander.h"
#include "keypad.h"
#include "keymap.h"
#include "lcd.h"
#include "lcd_buf.h"
#include "lpc17xx_systick.h"

#include <stdio.h>
#include <status.h>

void SysTick_Handler(void);
uint8_t pressed_key;
uint16_t state;

int main(void) {

    i2c_setup_polling();
    serial_init();
    lcd_init();
    lcd_clear_screen();
    
    SYSTICK_InternalInit(100);
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);
    
    
    
    
    while(1){}

    return 0;
}    

void SysTick_Handler(void){
    state = keypad_read();
     
    if(keypad_read_diff(&pressed_key, state) != 0) {

        char char_pressed = keymap_get_ascii_character(pressed_key);

        char buf[100];
        uint8_t buf_length = snprintf(buf, 99, "%c", char_pressed);
        
        write_usb_serial_blocking(buf, buf_length);

        lcd_buf_write_string(buf, buf_length, 0);

    }
    else {
        
    }

    lcd_buf_update();
}    
    