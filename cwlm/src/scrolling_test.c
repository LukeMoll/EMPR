#include <stdint.h>
#include "i2c.h"
#include "lcd_buf.h"
#include "lcd.h"
#include <lpc17xx_systick.h>
#include "status.h"

int8_t counter = 0;
int8_t scroll;
uint8_t scr_begin;
char buf[99];
uint16_t length;
uint16_t lcd_buf_long_text_init(char *text_begin, uint16_t text_length, int8_t *scroll);
void lcd_buf_long_text(char *text_begin, uint16_t *screen_begin, uint16_t text_length, int8_t *scroll);
void SysTick_Handler(void);

int main(void) {

    i2c_setup_polling();
    lcd_init();
    lcd_clear_screen();

    status_code(1);
    length = snprintf(buf, 99, "according to all known laws of aviation, there is no way a bee should be able to fly.\r\n");
    scroll = 0;
    scr_begin = lcd_buf_long_text_init(buf, length, &scroll);
    lcd_buf_update;

    status_code(2);
    SYSTICK_InternalInit(100);
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);
    status_code(3);

    while(1);
    return(1);
}
/**
 * Scroll is incremented or decremented outside the function, depending on what you're using as scroll.
 * text_begin is a pointer to the first element of the text, screen_begin is a pointer to where we are now. 
 * During initialisation, screen_begin is set to text_begin (maybe have a different init function?)
 * In this function, screen_begin is incremented by 0x10 * number of scrolldowns - or decremented by 0x10 since number of scrollups.
 * the init basically returns the pointer to screen_begin, sends the first bit of text, makes sure that scroll exist.
*/
uint16_t lcd_buf_long_text_init(char *text_begin, uint16_t text_length, int8_t *scroll) {
    lcd_buf_write_string_multi(text_begin, 32, 0, true);
    return 0;
}

void lcd_buf_long_text(char *text_begin, uint16_t *screen_begin, uint16_t text_length, int8_t *scroll) {

    status_code(5);

    uint16_t charsleft = (text_length - *screen_begin) < 32 ?
        32 : 
        (text_length - *screen_begin); 

    status_code(6);

    if((charsleft > 0) && (*screen_begin != text_begin)) {
        status_code(7);
        screen_begin += (*scroll * 0x10);
        status_code(8);
    }
    scroll = 0;
    status_code(9);
    //sticks here, unsure why.
    lcd_buf_write_string_multi((text_begin + *screen_begin), charsleft, 0, true);
    status_code(10);      
}

int8_t i = 0;
void SysTick_Handler(void) {
    SYSTICK_ClearCounterFlag();
    counter ++;
    if(counter >= 10) {
        scroll++;
        counter = 0;
        i++;
        status_code(i);
    }
    lcd_buf_long_text(buf, &scr_begin, length, &scroll);
    lcd_buf_update();
}