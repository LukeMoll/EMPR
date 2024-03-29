#include <lpc17xx_timer.h>
#include <lpc17xx_systick.h>
#include <lpc17xx_rtc.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <libempr/i2c.h>
#include <libempr/lcd.h>
#include <libempr/lcd_buf.h>
#include <libempr/expander.h>
#include <libempr/status.h>
#include <libempr/serial.h>
#include <libempr/keypad.h>
#include <libempr/audioplayback.h>
#include <libempr/audiorec.h>
#include <libempr/spi.h>
#include "ff.h"


/*disables all the rtc stuff to make testing other functionalities easier*/
#define RTC_ENABLED true
#define GOT_SD_WORKING true

void SysTick_Handler(void);
void intro_screen();
void choose_mode();
void browser(void);
void recording_intro(void);
void generate_name(void);
void type_name(void);
void start_recording(char buf[17]);
void info(uint8_t files_index);
void playback(uint8_t index);
void two_bot();


/**
 * keypad values: modified in SysTickHandler(void) (If they need to be modified). Can be accessed by any function 
 */
char pressed_key = 0;

uint8_t memory_size = 16; //change this later when we know how many files we can have
uint8_t line_size = 16;

void (*next_func)(void); 
void (*previous_func)(void);

/**
 * scrolling: scrolling modifies the top line of the screen (bottom line remains the same)
 * if a function requires scrolling, then scrolling_active is true. otherwise, it is false. (default = false)
 * if scrolling happens, then the function assigns the location to the first character of the text to text_begin
 * when the key '2' is pressed, the pointer value is incremented by 16
 * when the key '8' is pressed, the pointer value is decremented by 16
*/
bool scrolling_active = false;
int8_t scrolling_index = false;

uint8_t twotal = 0;

/*this is useful for SD card stuff*/

#define SD_PATH ""
FILINFO files[255];

/*
*configure everything, start the loop
**/
int main(void) {

    scrolling_active = false;
    i2c_setup_polling();
    lcd_init();
    lcd_buf_flush();

    serial_init();

#if RTC_ENABLED
    lcd_buf_write_string("pls send dates", 14, 1);
    lcd_buf_update();
    uint8_t buf[5] = {0, 0, 0, 0, 0};
	serial_send(0x02, NULL, 0);
	serial_recv(0x02, buf, 5);
	// set RTC
    RTC_Init(LPC_RTC);
    RTC_SetTime(LPC_RTC, RTC_TIMETYPE_MONTH, buf[0]);
    RTC_SetTime(LPC_RTC, RTC_TIMETYPE_DAYOFMONTH, buf[1]);
    RTC_SetTime(LPC_RTC, RTC_TIMETYPE_HOUR, buf[2]);
    RTC_SetTime(LPC_RTC, RTC_TIMETYPE_MINUTE, buf[3]);
    RTC_SetTime(LPC_RTC, RTC_TIMETYPE_SECOND, buf[4]);
    RTC_Cmd(LPC_RTC, ENABLE);
#endif

    SYSTICK_InternalInit(100);
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);

#if GOT_SD_WORKING
    spi_init();
    FRESULT res;

	FATFS fs;

	res = f_mount(&fs, "", 1);
  
    if(res != FR_OK)
        serial_printf("sd card mount failed\r\n");
#endif

    intro_screen();

    while(1) {
        //makes sure that the next function happens
        (*next_func)();
    }
    return 1;
    
}

/*
* gives an intro screen, press any key to leave
* scrolling_active is false
**/

void intro_screen() {
    // scrolling_active = false;
    lcd_buf_clear_screen();
    lcd_buf_write_string_multi("Press Any Key", 13, 0, true);
    while(1) {  //for now, it's a press any key, can change to wait later
        if(pressed_key) {
            next_func = &choose_mode;
            lcd_buf_clear_screen();
            pressed_key = 0;
            return;
            
        }
    }
}
/**
* choose between browser mode, or recording mode 
* D is back
* scrolling_active is false
*/

void choose_mode() {

    // scrolling_active = false;
    lcd_buf_clear_screen();
    lcd_buf_write_string_multi("1.Browser\n2.Recording", 23, 0, true);
    char keypad_num;
    while(1){
        if(pressed_key) {
            keypad_num = pressed_key;
            pressed_key = 0;
            switch(keypad_num) {
                case '1':
                    ;
                    next_func = &browser;
                    return;
                    break;
                case '2':
                    ;
                    next_func = &recording_intro;
                    return;
                    break;
                case 'D':
                    ;
                    next_func = &intro_screen;
                    return;
                    break;
                case '#':
                    ;
                    next_func = two_bot;
                    previous_func = &choose_mode;
                    return;
                    break;
                default: 
                    break;
            }
            
        }
    }
    return;
}
/**
* browser mode can view file names, and choose to playback, or find out info.
* A is info
* B is playback
* D (back) brings us back to choose_mode
* scrolling_active is true
*/
void browser(void) {
#if GOT_SD_WORKING
    scrolling_active = false;

    FRESULT res;
    DIR dp;

    if ((res = f_opendir(&dp, SD_PATH)) != FR_OK)
        serial_printf("SD card failed :(");
    FILINFO fno;

    uint8_t no_of_files = 0;
    while ((res = f_readdir(&dp, &fno)) == FR_OK) {
        if (fno.fname[0] == 0)
            break;

        if (fno.fattrib & (AM_DIR | AM_HID | AM_SYS))
            continue;

        memcpy(&files[no_of_files], &fno, sizeof(FILINFO));
        no_of_files++;
    }
    f_closedir(&dp);
    char list_of_text[no_of_files][16];
    for(size_t i = 0; i < no_of_files; i++) {
        sprintf(list_of_text[i], "%-16s", files[i].fname);
        //strncpy(list_of_text[i], files[i].fname, 12);
    }

#else
    char list_of_text[3][16] = {"aaaaaaaaaaaaaaaa", "bbbbbbbbbbbbbbbb", "cccccccccccccccc"};
    uint8_t no_of_files = 3;
#endif
    scrolling_active = true;
    //have a pointer to the array that is a list of names
    scrolling_index = 0;
    lcd_buf_clear_screen();
    lcd_buf_write_string(list_of_text[scrolling_index], 16, 0); 
    lcd_buf_write_string("A:$ B:>      D:< ", 16, 16);
    char *current_name;
    char keypad_num;

    while(1) {
        current_name = list_of_text[scrolling_index%no_of_files];
        status_code(scrolling_index%no_of_files);
        lcd_buf_write_string(current_name, 16, 0);
        if(pressed_key) { 
            keypad_num = pressed_key;
            pressed_key = 0;
            switch(keypad_num) {
                    break;
                case 'A': //info
                    ;
                    info(scrolling_index%no_of_files);
                    break;
                case 'B': //playback
                    ;
                    playback(scrolling_index%no_of_files);
                    break;
                case 'D': //goes back to the previous function
                    ;
                    next_func = &choose_mode; // worth having a previous function pointer as well? that way we can break D out of each individual function?
                    return;
                    break;
                case '#':
                    ;
                    previous_func = &browser;
                    next_func = &two_bot;
                    return;
                    break;

                default:
                    break;
            }
        }
    }
}

/**
* Allows the user to select the way the recorded file will be named 
* D (back) brings us back to choose mode
* scrolling_active is false
*/
void recording_intro(void) {
    scrolling_active = false;
    lcd_buf_clear_screen();
    lcd_buf_write_string_multi("1: gen name\n2: type name", 25, 0, true);
    lcd_buf_update();
    char symbol = '7';
    pressed_key = 0;
    while(1) {
        if(pressed_key) {
            symbol = pressed_key;
            pressed_key = 0;
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
                case '#':
                    ;
                    previous_func = &recording_intro;
                    next_func = two_bot;
                    return;
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
 * D is back, A in 'enter'
 * RTC doesn't fully work yet
*/
void generate_name(void) {
    char keypad_num;
    lcd_buf_clear_screen();
    lcd_buf_update();
#if RTC_ENABLED
    uint32_t month = RTC_GetTime(LPC_RTC, RTC_TIMETYPE_MONTH);
    uint32_t day = RTC_GetTime(LPC_RTC, RTC_TIMETYPE_DAYOFMONTH);
    uint32_t hour = RTC_GetTime(LPC_RTC, RTC_TIMETYPE_HOUR);
    uint32_t minute = RTC_GetTime(LPC_RTC, RTC_TIMETYPE_MINUTE);
    char buff[17];
    snprintf(buff, 12, "%02u/%02u-%02u:%02u", month, day, hour, minute); //Can change this later, for now just wanted to get RTC working
    lcd_buf_write_string(buff, 12, 0);
    lcd_buf_update();
    lcd_buf_write_string("    B:>     D:< ", 16, 16);
    pressed_key = 0;
    while(1) {
        if(pressed_key) {
            keypad_num = pressed_key;
            pressed_key = 0;
            switch(keypad_num) {
                case 'B':
                    ;
                    start_recording(buff);
                    return;
                    break;
                case 'D' :
                    ;
                    next_func = &recording_intro;
                    return;
                    break;
                case '#':
                    ;
                    previous_func = &generate_name;
                    next_func = &two_bot;
                    return;
                    break;

                default:
                    break;
            }

        }

    }
#else
    lcd_buf_write_string_multi("functionality\ndisabled", 22, 0, true);
    lcd_buf_update(); 
    pressed_key = 0;
    while(1) {
        if(pressed_key) {
            keypad_num = pressed_key;
            pressed_key = 0;
            switch(keypad_num) {
                case 'D' :
                    ;
                    next_func = &recording_intro;
                    return;
                    break;
                case '#':
                    ;
                    previous_func = &generate_name;
                    next_func = &two_bot;
                    return;
                    break;
                default:
                    break;
            }

        }

    }   
#endif

}
/**
 * Records sound and saves it under a user input name.
 * D is back, B is play
 * scrolling_active is false
*/
void type_name(void) {
    scrolling_active = false;
    lcd_buf_clear_screen();
    lcd_buf_write_string_multi("Use the numpad\nPress any key", 28, 0, true);
    pressed_key = 0;
    while(! pressed_key){};
    lcd_buf_clear_screen();
    lcd_buf_write_string("    B:>     D:< ", 16, 16);
    lcd_buf_update();
    // uint8_t index = 0;
    pressed_key = 0;
    char buf[13];
    sprintf(buf, "                ");
    uint8_t i = 0;
    while(i < 12) {
        if(pressed_key) {
            char chara = pressed_key;
            pressed_key = 0;
            switch(chara) {
                case 'B':
                    ;
                    start_recording(buf); //calls it rather than function pointer, because it needs an argument, and then it can do it's thing and return
                    next_func = &choose_mode;
                    return;
                    break;
                case 'D':
                    ;
                    next_func = &recording_intro;
                    return;
                    break;
                case 'A':
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
                    buf[i] = chara;
                    lcd_buf_write_string(buf, 13, 0);
                    lcd_buf_update();
                    i++;
                    break;
            }
        }
    }
    start_recording(buf);

    next_func = &choose_mode;
    return;
}

/**
 * Given a file name, records sound until told to stop, then saves that to the file
 * B is pause/stop, D is back (cancel)
 * scrolling_active is false
*/

void start_recording(char buf[12]) {

    scrolling_active = false;
    lcd_buf_clear_screen();
    lcd_buf_write_string(buf, 17, 0);
    lcd_buf_write_string("B:= ", 4, 20);
    lcd_buf_write_string("D:< ", 4, 27);
    lcd_buf_update();
    FRESULT res;
    FIL fp;
    if ((f_open(&fp, buf, FA_WRITE | FA_CREATE_ALWAYS)) != FR_OK)
        serial_printf("saving is broke :( (FRESULT)\r\n");
    size_t audiolen = 0x2000;
    uint16_t audiobuf[audiolen];
    recording_init(audiobuf, audiolen, PLAYBACK_8KHZ);
    pressed_key = 0;
    UINT bytes_written;
    while(1) {
        recording_start();
        while(!isrecording()) {};


        //write the buf to the sd card

        if ((res = f_write(&fp, audiobuf, audiolen, &bytes_written)) != FR_OK)
            serial_printf("saving is broke 2.0\r\n");
        f_sync(&fp);

        if(pressed_key = 'D') {
            f_close(&fp);
            break;
        }
    }
        
    return;
}

/**
 * Displays the info of the selected file.
 * D is back
 * scrolling_active is true
*/

void info(uint8_t files_index) {

    scrolling_active = true;
    scrolling_index = 0;
#if GOT_SD_WORKING
    
    FILINFO file_info = files[files_index];
    char info_list [4][16];
    sprintf(info_list[0], "%s    ", file_info.fname);
    sprintf(info_list[1], "size :  %08u", file_info.fsize);
    sprintf(info_list[2], "date :  %08u", file_info.fdate);
    sprintf(info_list[3], "time :  %08u", file_info.ftime);
#else
    char info_list[3][16] = {"dddddddddddddddd", "eeeeeeeeeeeeeeee", "ffffffffffffffff"}; /*change this later when I figure out how to open files*/
#endif
    lcd_buf_clear_screen();
    char *current_string;
    lcd_buf_write_string("            D:< ", 16, 16);
    lcd_buf_update();
    pressed_key = 0;
    while(1) {
        if(pressed_key) {
            char key = pressed_key;
            pressed_key = 0;
            switch(key) {
                case 'D':
                    ;
                    next_func = &browser;
                    //this for some reason breaks, something to do with lcd_buf
                    return;
            }
        }
        current_string = info_list[scrolling_index%3];
        lcd_buf_write_string(current_string, 16, 0);
        lcd_buf_update();
    }
   return;
}

/**
 * Given a file, plays that file
 * D is back
 * scrolling_active is false
*/
#if GOT_SD_WORKING
void playback(uint8_t index) {
    //TODO: test once SD is working
    lcd_buf_clear_screen();
    lcd_buf_write_string("playing back",12,0);
    scrolling_active = false;
    uint32_t len = 0x7000;
    uint8_t bufout[len]; //buffer we read/play into
    uint32_t toread = files[index].fsize;    //size of the file
    uint32_t hasread = 0;   //use this to display time left later
    TCHAR name = files[index].fname;
    FIL current_file;
    f_open(&current_file, name, FA_READ); //not actually title, it needs to have the .wav extension
    bool paused = false;
    playback_init(bufout, 0x7000, PLAYBACK_4KHZ);
    playback_play();
    char keypad_num;
    while(hasread < toread) {
        if(!paused){
            f_read(&current_file, bufout, len, &hasread);
        }
        if(pressed_key) {
            keypad_num = pressed_key;
            switch(keypad_num) {
                case 'B':
                    if(paused) {
                        playback_play();
                        paused = false;
                        break;
                    } else {
                        playback_pause();
                        paused = true;
                        break;
                    }
                case 'D':
                    playback_pause();
                    f_close(&current_file);
                    return;
            }
        }
     //figure out timing stuff

    }
    playback_pause();
    f_close(&current_file);
    return;

}
#endif

void two_bot() {
    twotal++;
    char buf[32];
    sprintf(buf, "oh no! You've been #'d! total:%02u", twotal);
    lcd_buf_write_string_multi(buf, 32, 0, true);
    lcd_buf_update();
    pressed_key = 0;
    while(1) {
        if(pressed_key=='D') {
            lcd_buf_clear_screen();
            lcd_buf_update();
            next_func = previous_func;
            return;
        }
    }
}

/**
 * Systick interupt handler
 * Is called every 100 miliseconds
 * checks if any key is pressed
 * updates the lcd screen
 * if scrolling_active is true, then does all the scrolly stuff
*/
void SysTick_Handler(void) {
    SYSTICK_ClearCounterFlag();  // clear interrupt flag
    lcd_buf_update();
    static char previous_key = 0;
    pressed_key = keypad_get_key();
    if(previous_key == pressed_key) {
        previous_key = pressed_key;
        pressed_key = 0;
    }
    else {
        previous_key = pressed_key;
    }
    
    if(scrolling_active) {
        /*this currently uses the keypad keys 2 and 8. It should, however, be trivial to sitch it to something else*/
        if(pressed_key) {
            char scroll = pressed_key;
           switch(scroll) {
                case '2':
                    ;
                    scrolling_index++; //this shouldn't work why the hell does it work
                    break;
                case '8':
                    ;
                    scrolling_index++;
                    break;
                default: 
                    break;
           }
        }
    }
}
