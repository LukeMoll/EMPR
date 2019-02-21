#include <lpc17xx_timer.h>
#include <lpc17xx_rit.h>
#include <lpc17xx_rtc.h>

#include <stdlib.h>
#include <string.h>

#include "i2c.h"
#include "lcd.h"
#include "lcd_buf.h"
#include "expander.h"
#include "keypad.h"
#include "keymap.h"
#include "status.h"
#include "serial.h"

void RIT_IRQHandler(void);
void intro_screen();
void choose_mode();
void browser(void);
void recording_intro(void);
void generate_name(void);
void type_name(void);
void start_recording(char buf[17]);
void info(char title[16]);
void playback(char title[16]);

// #define DISABLE false
// #define ENABLE true

/**
 * keypad values: modified in RIT_IRQHandler (If they need to be modified). Can be accessed by any function 
 */

uint8_t pressed_key;    //which key has been pressed - meaningless on it's own, need to use the keymap functions with it
uint16_t keypad_state; //has a key been pressed on the keypad?
uint16_t key_val;   //used to get the keypad_state

uint8_t memory_size = 16; //change this later when we know how many files we can have
uint8_t line_size = 16;
// char list_of_text[memory_size][line_size]; //pointer to the first char of the first line

void (*next_func)(void); 

/**
 * scrolling: scrolling modifies the top line of the screen (bottom line remains the same)
 * if a function requires scrolling, then scrolling_active is ENABLEd. otherwise, it is DISABLEd. (default = DISABLE)
 * if scrolling happens, then the function assigns the location to the first character of the text to text_begin
 * when the key '2' is pressed, the pointer value is incremented by 16
 * when the key '8' is pressed, the pointer value is decremented by 16
*/
// bool scrolling_active 
// char char buf[3];


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

    serial_init();

    RTC_Init(LPC_RTC);

    /**
     * TODO: write a script that sends 
     * % date +"%m" > /dev/ttyACM0
     * % date +"%d" > /dev/ttyACM0
     * %char buf[3];
     * % date +"%M" > /dev/ttyACM0
     * % date +"%S" > /dev/ttyACM0
     * currently only works if you enter things via screen (unsure why)
    */

    
    uint8_t buf;

    // status_code(1);
    // uint8_t month = read_usb_serial_blocking(buf, 2);

    //atoi is being screwy, unsure how to fix

    status_code(1);
    uint8_t month = 0;
    while(!month) {
        month = read_usb_serial_blocking(&buf, 1);
    }
    RTC_SetTime(LPC_RTC, RTC_TIMETYPE_MONTH, buf);

    lcd_buf_write_string("please send day", 15, 0);
    /**
    lcd_buf_update();
    uint8_t day = 0;
    while(!month) {
        day = read_usb_serial_blocking(buf, 2);
    }
    RTC_SetTime(LPC_RTC, RTC_TIMETYPE_DAYOFMONTH, atoi(buf));

    uint8_t hour = 0;
    while(!hour) {
        hour = read_usb_serial_blocking(buf, 2);
    }

    RTC_SetTime(LPC_RTC, RTC_TIMETYPE_HOUR, atoi(buf));

    uint8_t minute = 0;
    while(!minute) {
        minute = read_usb_serial_blocking(buf, 2);
    }

    RTC_SetTime(LPC_RTC, RTC_TIMETYPE_MINUTE, atoi(buf));
    uint8_t sec = 0;
    while(!sec) {
        sec = read_usb_serial_blocking(buf, 2);
    }
    status_code(atoi(buf));


    RTC_Cmd(LPC_RTC, ENABLE);

    RTC_SetTime(LPC_RTC, RTC_TIMETYPE_SECOND, atoi(buf));
    */

    intro_screen();
    status_code(16);

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

    //list_of_text = list_of_titles
    // scrolling_active = ENABLE;
    //have a pointer to the array that is a list of names
    
    /**
    *FRESULT f_readdir (
	DIR* dp,			 Pointer to the open directory object 
	FILINFO* fno		 Pointer to file information to return 
   )
                    */
    //the info we wanna return is names
    uint8_t index = 0;
    lcd_buf_clear_screen();
    lcd_buf_write_string("henlo      world", 16, 16); //replace this with the info string
    //info char: UPPER_I_DOTFUL?
    //playback char: CHR_ARROW_RIGHT
    //'back' char : CHR_ARROW_LEFT

    char *current_name;

    while(1) {
        // current_name = list_of_text[index];
        // lcd_buf_write_string(current_name, 16, 0);
        if(keypad_state) { 
            switch(keymap_get_ascii_character(pressed_key)) {
                    break;
                case 'A': //info
                    ;
                    info(current_name);
                    /**
                    *FRESULT f_readdir (
	                DIR* dp,			 Pointer to the open directory object 
	                FILINFO* fno		 Pointer to file information to return 
                    )
                    */
                    break;
                case 'B': //playback
                    ;
                    playback(current_name);
                    //open the file into a buffer
                    //send that file to the audioboard
                    break;
                case 'D': //goes back to the previous function
                    ;
                    next_func = &choose_mode; // worth having a previous function pointer as well? that way we can break D out of each individual function?
                    return;
                    break;
                case '2':
                    ;
                    if(index<memory_size) {
                        index++;
                    };
                    break;
                case '8':
                    ;
                    if(index>0) {
                        index--;
                    };
                    break;
                default:
                    break;
            }
        }
    }
    // while(keymap_get_ascii_character(pressed_key) != 'D') {

    // }
    // next_func = &choose_mode;
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
 * RTC doesn't fully work yet
*/
void generate_name(void) {
    lcd_buf_clear_screen();
    uint32_t month = RTC_GetTime(LPC_RTC, RTC_TIMETYPE_MONTH);
    uint32_t day = RTC_GetTime(LPC_RTC, RTC_TIMETYPE_DAYOFMONTH);
    uint32_t hour = RTC_GetTime(LPC_RTC, RTC_TIMETYPE_HOUR);
    uint32_t minute = RTC_GetTime(LPC_RTC, RTC_TIMETYPE_MINUTE);
    uint32_t seconds = RTC_GetTime(LPC_RTC, RTC_TIMETYPE_SECOND);
    char buf[17];
    uint8_t buf_length = snprintf(buf, 16, "%02s/%02s - %02s:%02s:%02s", month, day, hour, minute, seconds); //Can change this later, for now just wanted to get RTC working
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
    //open a file with the name
    //create a buf
    //read from the audioboard into the buf
    //write the buf to the sd card
    while(1){};
    return;
}

info(char title[16]) {
    /**
     * TODO:
     * FIL *current_file
     * fopen(*current_file, title, "r")
     * have an array of types of info
     * iterate through that array and use 
     * FRESULT f_readdir() with the type of info and then read the line that has to do with whatever we want.
     * list_of_text = info_text
     * implement scrolling and back buttons
     * D returns
    */
}

playback(char title[16]) {
    uint16_t bufout[16]; //or wev wordlength is for i2s
    uint32_t toread = 0;    //size of the file, should be able to get it like we do in info
    uint32_t hasread;   //use this to display time left later
    /**
     * FIL *current_file
     * TODO:
     * fopen(*current_file, title, "r");
     * while(hasread < toread) {
     *  fread(*current_file, bufout, toread, &hasread);
     *  i2s_play(bufout, speed);
     *  do something to do with displaying time using hasread
     * }
     * fclose(*current_file);
     * return;
    */
}


void RIT_IRQHandler(void) {
    RIT_GetIntStatus(LPC_RIT);  // clear interrupt flag
    key_val = keypad_read(); 
    lcd_buf_update();
    keypad_state = keypad_read_diff(&pressed_key, key_val);
    status_code(keypad_state%15);
}





