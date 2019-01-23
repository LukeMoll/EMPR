#include "i2c.h"
#include "lcd_charmap.h"
#include "lcd.h"
#include <stdlib.h>
#include <stdbool.h>

#include "lcd_buf.h"

static uint8_t lcd_memory_copy[32] = {
    0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0,
    0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0
};

static uint8_t lcd_buffer[32] = {
    0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0,
    0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0
};

/**
 * Updates a single byte in the buffer, to be written on the next update tick
 *  byte: LCD character byte from lcd_ascii_to_byte or the like
 *  index: 0-based index of which character to address. First line is 0-15, second line is 16-31.
 */
void lcd_buf_write_byte(uint8_t byte, uint8_t index) {
    if(index < 32) {
        lcd_buffer[index] = byte;
    }
}

/**
 * Writes a series of bytes to the buffer, to be written on the next update tick
 *  bytes: array of bytes to write. As lcd_buf_write_byte
 *  length: number of bytes to write. Does NOT need null termination
 *  base_index: index of the buffer (as lcd_buf_write_byte) that the first byte will be written to.
 * 
 * ! Does not support newline address skipping !
 */
void lcd_buf_write_bytes(uint8_t * bytes, uint8_t length, uint8_t base_index) {
    uint8_t i;
    for(i=0; i < length && (base_index + i) < 32; i++) {
        lcd_buffer[base_index + i] = bytes[i];
    }
}

/**
 * Writes all changes from the buffer to the LCD.  
 */
void lcd_buf_update(void) {
    uint8_t i;
    for(i=0; i < 32; i++) {
        if(lcd_memory_copy[i] != lcd_buffer[i]) {
            uint8_t addr = i < 16? (0x80 + i) : (0x98 + i);
            // 0x98 just works for the first line ¯\_(ツ)_/¯
            lcd_write_byte(lcd_buffer[i], addr);
            lcd_memory_copy[i] = lcd_buffer[i];
        }
    }
}

/**
 * Forcibly writes the entire buffer to the LCD, regardless if there are any changes
 */
void lcd_buf_flush(void) {
    uint8_t i;
    for(i=0; i < 32; i++) {
        uint8_t addr = i < 16? (0x80 + i) : (0x90 + i);
        lcd_write_byte(lcd_buffer[i], addr);
        lcd_memory_copy[i] = lcd_buffer[i];
    }
}

/**
 * Clears the buffer so the screen will be cleared on next _update or _flush
 */
void lcd_buf_clear_screen() {
    uint8_t i;
    for(i=0; i < 32; i++) {
        lcd_buffer[i] = 0xa0;
    }
}

/**
 * Converts and writes an ASCII string to the buffer
 *  string: ASCII string
 *  length: number of non-null characters in the string
 *  base_index: zero-based index of the buffer (as lcd_buf_write_byte)
 */
void lcd_buf_write_string(char * string, uint8_t length, uint8_t base_index) {
    uint8_t * bytes = lcd_a2b(string, length);
    lcd_buf_write_bytes(bytes, length, base_index);
    free(bytes);
}

/**
 * Same as lcd_buf_write_string but with newline address skipping (with optional character clearing)
 *  string: ASCII string
 *  length: number of non-null characters in the string
 *  base_index: zero-based index of the buffer (as lcd_buf_write_byte)
 *  clear_skipped: whether to set LCD between the newline character and the start of the nextline to blanks (true)
 *                 or to ignore them and just skip the address (false)
 */
void lcd_buf_write_string_multi(char * string, uint8_t length, uint8_t base_index, bool clear_skipped) {
    uint8_t * bytes = lcd_a2b(string, length); // this involves MALLOC; need to free after!
    
    uint8_t i, index;
    index = base_index; 
    // i is the position in bytes
    // index is the position on the LCD
    for(i=0; i<length; i++) {
        if(bytes[i] == CHR_BLANK_NEWLINE && index < 16){ // ignore newline characters if we're already on the second line (just prints space)
            if(clear_skipped) {
                for(; index < 16; index++) { // write blanks until index == 16
                    lcd_buf_write_byte(CHR_BLANK_SPACE, index);
                }
            }
            else {
                index = 16;
            }
            // skip this byte and don't write it
        }
        else {
            lcd_buf_write_byte(bytes[i], index++);
        }
    }

    free(bytes);    
}

/**
 * Scroll is incremented or decremented outside the function, depending on what you're using as scroll.
 * text_begin is a pointer to the first element of the text, screen_begin is a pointer to where we are now. 
 * During initialisation, screen_begin is set to text_begin (maybe have a different init function?)
 * In this function, screen_begin is incremented by 0x10 * number of scrolldowns - or decremented by 0x10 since number of scrollups.
 * the init basically returns the pointer to text begin, sends the first bit of text, makes sure that scroll exist.
*/
char * lcd_buf_long_text_init(char * text_begin, uint8_t text_length, int8_t * scroll) {
    lcd_buf_write_string_multi(text_begin, 32, 0, true);
    return text_begin;
}

void lcd_buf_long_text(char * text_begin, char * screen_begin, uint8_t text_length, int8_t * scroll) {
    uint32_t text_end = &text_begin + text_length;
    uint8_t screen_length = 0x20;
    uint8_t charsleft = ((text_end - screen_begin) & 0x1F) + 1; //masks any length longer than 32, keeps anything smoller - except it doesn't, but that's what I need it to do.
    if((charsleft > 0) && (screen_begin != text_begin)) {
        &screen_begin += (0x10 * scroll);
    }
    scroll = 0;
    lcd_buf_write_string_multi(screen_begin, chars_left, 0, true);
    buf_update();       
}