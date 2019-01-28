#include <lpc17xx_timer.h>
#include <lpc17xx_rit.h>

#include "i2c.h"
#include "lcd.h"
#include "lcd_buf.h"
#include "expander.h"
#include "keypad.h"
#include "keymap.h"
#include "status.h"

void RIT_IRQHandler(void);
void intro_screen();
uint8_t choose_mode();
void browser(void);
void recording(void);

uint8_t pressed_key;
uint16_t keypad_state;
uint16_t key_val;
void (*next_func)(void);


int main(void) {

    i2c_setup_polling();
    lcd_init();
    lcd_buf_flush();

    RIT_Init(LPC_RIT);
    RIT_TimerConfig(LPC_RIT, 100);
    NVIC_EnableIRQ(RIT_IRQn);

    intro_screen();
    choose_mode();
    status_code(11);
    (*next_func)();
    status_code(12);
    return 1;
    
}

void intro_screen() {
    lcd_buf_write_string_multi("welcome,\nwelcome!", 18, 0, true);
    while(1) {  //for now, it's a press any key, can change to wait later
        if(keypad_state) {
            lcd_buf_clear_screen();
            keypad_state = 0;
            return;
            
        }
    }
}

uint8_t choose_mode() {
    lcd_buf_write_string_multi("1.Browser\n2.Recording", 23, 0, true);
    uint8_t keypad_num;
    while(1){
        if(keypad_state) {
            keypad_num = keymap_get_number(pressed_key);
            status_code(keypad_num);
            switch(keypad_num) {
                case 1:
                    ;
                    status_code(5);
                    next_func = &browser;
                    break;
                case 2:
                    ;
                    status_code(6);
                    next_func = &recording;
                    break;
                default: 
                    status_code(7);
                    break;
            }
            return keypad_num;
            
        }
    }
    return(3);
}

void browser(void) {
    lcd_buf_clear_screen();
    lcd_buf_write_string_multi("browser\n", 7, 0, true);
    status_code(8);
    return;
}

void recording(void) {
    lcd_buf_clear_screen();
    lcd_buf_write_string_multi("recording\n", 9, 0, true);
    status_code(9);
    return;
}


void RIT_IRQHandler(void) {
    RIT_GetIntStatus(LPC_RIT);
    key_val = keypad_read();
    lcd_buf_update();
    keypad_state = keypad_read_diff(&pressed_key, key_val);
}

