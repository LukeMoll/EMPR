#include <lpc17xx_timer.h>
#include <lpc17xx_rit.h>
#include <lpc17xx_rtc.h>

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
void recording_intro(void);
void generate_name(void);
void type_name(void);
void start_recording(char buf[17]);

// #define DISABLE false
// #define ENABLE true

/**
 * keypad values: modified in RIT_IRQHandler (If they need to be modified). Can be accessed by any function 
 */
uint8_t pressed_key;    //which key has been pressed - meaningless on it's own, need to use the keymap functions with it
uint16_t keypad_state; //has a key been pressed on the keypad?
uint16_t key_val;   //used to get the keypad_state

/**
 * scrolling: scrolling modifies the top line of the screen (bottom line remains the same)
 * if a function requires scrolling, then scrolling_active is ENABLEd. otherwise, it is DISABLEd. (default = DISABLE)
 * if scrolling happens, then the function assigns the location to the first character of the text to text_begin
 * when the key '2' is pressed, the pointer value is incremented by 16
 * when the key '8' is pressed, the pointer value is decremented by 16
*/
// bool scrolling_active 
// char *text_begin  
void (*next_func)(void); 

/*
*configure everything, start the loop
**/
int main(void) {

    // scrolling_active = DISABLE;

    i2c_setup_polling();
    lcd_init();
    lcd_buf_flush();

    RIT_Init(LPC_RIT);
    RIT_TimerConfig(LPC_RIT, 100);
    NVIC_EnableIRQ(RIT_IRQn);

    RTC_Init(LPC_RTC);
    RTC_SetTime(LPC_RTC, RTC_TIMETYPE_SECOND, 0);
    RTC_Cmd(LPC_RTC, ENABLE);

    intro_screen();
    while(1) {
        //makes sure that the next function happens
        (*next_func)();
    }
    return 1;
    
}

/*
* gives an intro screen, press any key to leave
* scrolling is disabled
**/

void intro_screen() {
    // scrolling_active = DISABLE;
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
/*
* choose between browser mode, or recording mode 
* scrolling is DISABLED cause there are only two options, so no need for is
**/

void choose_mode() {

    // scrolling_active = DISABLE;
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
                    next_func = &recording_intro;
                    break;
                default: 
                    break;
            }
            return;
            
        }
    }
    return;
}

char names[3][6] = {"name1", "name2", "name3"};
/**
* browser mode (not fully implemented, biut I have a fairly solid idea of what's going on), can view files, and choose to playback, or find out info.
* A is info
* B is playback
* D (back) brings us back to choose_mode
* Scrolling is ENABLED
*/
void browser(void) {

    // scrolling_active = ENABLE;
    //have a pointer to the array that is a list of names
    lcd_buf_clear_screen();
    lcd_buf_write_string("henlo      world", 16, 16); //replace this with the info string
    //info char: UPPER_I_DOTFUL?
    //playback char: CHR_ARROW_RIGHT
    //'back' char : CHR_ARROW_LEFT
    while(1) {
        if(keypad_state) { 
            switch(keymap_get_ascii_character(pressed_key)) {
                    break;
                case 'A': //info
                    ;
                    break;
                case 'B': //playback
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

/**
* recording function (not yet implemented)
* D (back) brings us back to choose mode
*/
void recording_intro(void) {
    // status_code(3); // TODO: put this back
    lcd_buf_clear_screen();
    lcd_buf_write_string_multi("recording\n", 9, 0, true);
    char symbol = '7';
    keypad_state = false;
    while(1) {
        if(keypad_state) {
            symbol = keymap_get_ascii_character(pressed_key);
            switch(symbol) {
                case '1' :
                    ;
                    next_func = &generate_name;
                    break;
                case '2' :
                    ;
                    next_func = &type_name;
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


/**
 * automatically generates a name (based on rtc?)
 * no scrolling
 * D is back, A in 'enter'
*/
void generate_name(void) {
    lcd_buf_clear_screen();
    uint32_t time = RTC_GetTime(LPC_RTC, RTC_TIMETYPE_SECOND);
    char buf[17];
    uint8_t buf_length = snprintf(buf, 16, "%015x", time); //Can change this later, for now just wanted to get RTC working
    lcd_buf_write_string(buf, buf_length, 0);
    while(1) {
        if(keypad_state) {
            switch(keymap_get_ascii_character(pressed_key)) {
                case 'A':
                    ;
                    start_recording(buf);
                    break;
                case 'D' :
                    ;
                    next_func = &recording_intro;
                    break;
                default:
                    break;
            }
            return;

        }

    }

}
void type_name(void) {
    lcd_buf_clear_screen();
    lcd_buf_write_string_multi("use the numpad\nPress any key", 28, 0, true);
    keypad_state = false;
    while(! keypad_state){};
    lcd_buf_clear_screen();
    uint8_t index = 0;
    keypad_state = false;
    char buf[17];
    sprintf(buf, "                ");
    while(index < 16) {
        if(keypad_state) {
            keypad_state = false;
            char chara = keymap_get_ascii_character(pressed_key);
            switch(chara) {
                case 'A':
                    ;
                    start_recording(buf); //calls it rather than function pointer, because it needs an argument, and then it can do it's thing and return
                    return;
                    break;
                case 'D':
                    ;
                    next_func = &recording_intro;
                    return;
                    break;
                case 'B':
                ;
                break;
                    case 'C':
                    ;
                    break;
                case '#':
                    ;
                    break;
                case '*':
                    ;
                    break;
                default:
                    ;
                    buf[index] = chara;
                    lcd_buf_write_string(buf, 17, 0);
                    //I hate myself that this in any way works
                    index++;
                    break;
            }
        }
    }
    next_func = &start_recording;
    return;
}

void start_recording(char buf[17]) {
    //TODO: record sound, save it to SD card, with name
    lcd_buf_clear_screen();
    lcd_buf_write_string("we're recording", 15, 0);
    lcd_buf_write_string(buf, 17, 16);
    while(1){};
    return;
}


void RIT_IRQHandler(void) {
    RIT_GetIntStatus(LPC_RIT);  // clear interrupt flag
    key_val = keypad_read(); 
    lcd_buf_update();
    keypad_state = keypad_read_diff(&pressed_key, key_val);
    status_code(keypad_state%15);
}





