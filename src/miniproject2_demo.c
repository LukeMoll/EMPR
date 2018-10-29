#include "i2c.h"
#include "serial.h"
#include "expander.h"
#include "keypad.h"
#include "keymap.h"
#include "lcd.h"
#include "lcd_buf.h"
#include <lpc17xx_systick.h>

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "status.h"

void systick_stage1();
void systick_stage2();
void systick_stage3();
void SysTick_Handler();

uint8_t pressed_key;
uint16_t keypad_state;
uint8_t stage;
uint8_t millisecs;


int main(void) {

    stage = 1;
    millisecs = 0;

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

void systick_stage1() {
    switch(millisecs) {
        case 0: 
            i2c_detect();
            lcd_buf_write_string_multi("Detecting i2c\ndevices   ", 24, 0, true);
            millisecs++;
        break;
        case 20: 
            millisecs = 0;
            stage++; 
        break;
        default:
            ;
            char elipses[3];
            switch((millisecs >> 2) % 4) {
                case 0:
                    strncpy(elipses, "   ", 3);
                break;
                case 1:
                    strncpy(elipses, ".  ", 3);
                break;
                case 2:
                    strncpy(elipses, ".. ", 3);
                break;
                case 3:
                    strncpy(elipses, "...", 3);
                break;
            }
            lcd_buf_write_string(elipses, 3, 23);
            millisecs++;
    }
}

void systick_stage2() {
    
    // status_code(millisecs);
    switch(millisecs) {
        case 1:
            ; // needed if the first line after a `case` statement is a declaration
            char hello_world[12] = "hello, world";
            lcd_buf_clear_screen();
            lcd_buf_write_string(hello_world, 12, 0);
            millisecs++;
        break;

        case 20:
            lcd_buf_write_string_multi("henlo\nworld", 12, 0, true);
            millisecs++;
        break;

        case 40: 
            lcd_clear_screen();
            millisecs = 0;
            stage++;
            lcd_buf_write_string("Press any key", 13, 16);
        break;

        default:
            millisecs++;
    }

}

void systick_stage3() {
    
    keypad_state = keypad_read();
    
    if(read_diff(&pressed_key, keypad_state) != 0) {

            char char_pressed = keymap_get_ascii_character(pressed_key);

            char buf[100];
            uint8_t buf_length = snprintf(buf, 99, "%c", char_pressed);

            lcd_buf_write_string(buf, buf_length, 0);

    }
    else {
        
    }

}


void SysTick_Handler(void) {

    lcd_buf_update();

    // char buf[100];
    // write_usb_serial_blocking(buf, snprintf(buf, 99, "SysTick: t%04d\tstage %02d\r\n", millisecs, stage));
    switch (stage) {
        case 1:
            systick_stage1();
        break;
        case 2:
            systick_stage2();
        break;
        case 3:
            systick_stage3();
        break;
    }    
} 