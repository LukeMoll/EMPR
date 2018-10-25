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

void systick_stage1();
void systick_stage2();
void systick_stage3();
void SysTick_Handler(void);

uint8_t pressed_key;
uint16_t state;
uint8_t stage;
uint8_t millisecs;

int main() {

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
        case 0: i2c_detect(); millisecs++;
        case 20: millisecs = 0; stage++; 
        default: millisecs++;
    }
}

void systick_stage2() {
    
    //for some reason, only reaches cases 20 and default, unsure why.
    status_code(millisecs);
    switch(millisecs) {
        case 1:
        lcd_clear_screen();
        char hello_world[100];
        uint8_t hello_world_length = snprintf(hello_world, 99, "hello, world!");
        lcd_buf_write_string(hello_world, hello_world_length, 0);
        
        case 20:
        lcd_clear_screen();
        char henlo_world[100];
        uint8_t henlo_world_length = snprintf(henlo_world, 99, "henlo \n world");
        lcd_buf_write_string(henlo_world, henlo_world_length, 0);

        case 40: lcd_clear_screen(); millisecs = 0; stage++;
        default: millisecs++;
    }

}

void systick_stage3() {
    
    state = keypad_read();
    
    if(read_diff(&pressed_key, state) != 0) {

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

    switch (stage) {
        case 1 : systick_stage1();
        case 2 : systick_stage2();
        case 3 : systick_stage3();
    }    
} 