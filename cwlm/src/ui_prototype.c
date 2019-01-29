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
void choose_mode();
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
    status_code(11);
    while(1) {
        (*next_func)();
    }
    return 1;
    
}

void intro_screen() {
    lcd_buf_write_string_multi("welcome,\nwelcome!", 18, 0, true);
    while(1) {  //for now, it's a press any key, can change to wait later
        if(keypad_state) {
            next_func = &choose_mode;
            lcd_buf_clear_screen();
            keypad_state = 0;
            return;
            
        }
    }
}

void choose_mode() {
    lcd_buf_clear_screen();
    lcd_buf_write_string_multi("1.Browser\n2.Recording", 23, 0, true);
    char keypad_num;
    while(1){
        if(keypad_state) {
            keypad_num = keymap_get_ascii_character(pressed_key);
            switch(keypad_num) {
                case '1':
                    ;
                    next_func = &browser;
                    break;
                case '2':
                    ;
                    next_func = &recording;
                    break;
                // case 'D':
                //     ;
                //     next_func = &intro_screen;
                //     break;
                default: 
                    break;
            }
            return;
            
        }
    }
    return;
}

char names[3][6] = {"name1", "name2", "name3"};

void browser(void) {
    lcd_buf_clear_screen();
    lcd_buf_write_string("henlo      world", 16, 16); //replace this with the info string
    while(1) {
        if(keypad_state) { 
            switch(keymap_get_ascii_character(pressed_key)) {
                    break;
                case 'A':
                    ;
                    break;
                case 'B':
                    ;
                    break;
                case 'D': //goes back to the previous function
                    ;
                    next_func = &choose_mode; // worth having a previous function pointer as well? that way we can break D out of each individual function?
                    return;
                    break;
                default:
                    break;
            }
        }
    }
    // while(keymap_get_ascii_character(pressed_key) != 'D') {

    // }
    // next_func = &choose_mode;
    return;
}

void recording(void) {
    lcd_buf_clear_screen();
    lcd_buf_write_string_multi("recording\n", 9, 0, true);
    status_code(9);
    while(1) {
        if(keypad_state) {
            switch(keymap_get_ascii_character(pressed_key)) {
                case '2' :
                    ;
                    break;
                case '8' :
                    ;
                    break;
                case 'A':
                    ;
                    break;
                case 'B':
                    ;
                    break;
                case 'D':
                    ;
                    next_func = &choose_mode;
                    break;
                default:
                    break;
            }
            return;
        }
    }
}


void RIT_IRQHandler(void) {
    RIT_GetIntStatus(LPC_RIT);
    key_val = keypad_read();
    lcd_buf_update();
    keypad_state = keypad_read_diff(&pressed_key, key_val);
}





