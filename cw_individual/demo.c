#include <libempr/serial.h>
#include "ff.h"
#include "audioplayback.h>"
#include <libempr/i2c.h>
#include <libempr/lcd.h>
#include <libempr/lcd_buf.h>
#include <libempr/lcd_keymap.h>
#include <libempr/keypad.h>

#define COMMAND 0x05 //serial command

void Systick_Handler(void);
void intro_screen();

void (*next_func)(void);

char pressed_key = 0;

bool scrolling_active = false;
int8_t scrolling_index = 0;


void main() {
    i2c_setup_polling();
    lcd_init();
    lcd_buf_flush();

    serial_init();

    SYSTICK_InternalInit(100);
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);

    spi_init();
    FATFS fs;
    f_mount(&fs, "", 1);

    intro_screen();

    while(1) {
        (*next_func)();
    }
}

intro_screen() {
    lcd_buf_clear_screen();
    lcd_buf_write_string_multi("Press Any Key\nCW Demo", 13, 0, true);
    while(1) {  //for now, it's a press any key, can change to wait later
        if(pressed_key) {
            next_func = &browser;
            lcd_buf_clear_screen();
            pressed_key = 0;
            return;
            
        }
    }
}

void browser(void) {
    FILINFO files[255];
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

    scrolling_active = true;
    //have a pointer to the array that is a list of names
    scrolling_index = 0;
    lcd_buf_clear_screen();
    lcd_buf_write_string(list_of_text[scrolling_index], 16, 0); 
    lcd_buf_write_string("    B:>         ", 16, 16);
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
                case 'B': //playback
                    ;
                    playback(scrolling_index%no_of_files);
                    break;
                default:
                    break;
            }
        }
    }
}

void playback(uint8_t index) {
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
    uint8_t timelen = toread * 0.00025; //length of clip in seconds
    serial_send(COMMAND, &timelen, 1); //send length over serial
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

