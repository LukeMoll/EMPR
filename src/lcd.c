#include "lcd_charmap.h"
#include "i2c.h"
#include "lcd.h"

void lcd_init() {
    // init sequence: one control byte and ten instructions (as in lecture slides, see PCF2119x datasheet Table 12)
    i2c_write_multiple_bytes(LCD_I2C_ADDR, (uint8_t[]){
        0x00, // control byte: instruction register, last control byte of this i2c transfer
        0x34, // 0x34 = function_set: 2x4 bits, 2lx16c, 1:9mplx mode, basic + standard instruction set
        0x0c, // 0x0c = display_ctl: display on, cursor off, blink off
        0x06, // 0x06 = curs_disp_suint8_t * chars, uint8_t length, uint8_t base_addresshift: move cursor, right shift.
        0x35, // 0x35 = function_set: 2x4 bits, 2lx16c, 1:9mplx mode, basic + extended instruction set
        0x04, // here be ¯\_(ツ)_/¯s
        0x10, // ...
        0x42, // ...
        0x9f, // ...uint8_t * chars, uint8_t length, uint8_t base_address
        0x34, // ...
        0x02  // last byte of init sequence
        }, 11); 

    lcd_clear_screen();
}

void lcd_clear_screen() {

    uint8_t i;
    for(i=0x80; i<0x90; i++) {
    i2c_write_multiple_bytes(LCD_I2C_ADDR, (uint8_t[]){0x00,i}, 2);
    i2c_write_multiple_bytes(LCD_I2C_ADDR, (uint8_t[]){0x40,0xa0}, 2); //writes a blank space at i  
    }
    //clears line 1

    for(i=0xc0; i<0xd0; i++) {
    i2c_write_multiple_bytes(LCD_I2C_ADDR, (uint8_t[]){0x00,i}, 2);
    i2c_write_multiple_bytes(LCD_I2C_ADDR, (uint8_t[]){0x40,0xa0}, 2); //writes a blank space at i  
    }
    //clears line 2
    
}

void lcd_write_bytes(uint8_t * bytes, uint8_t length, uint8_t base_address) {

    uint8_t i, j;
    j = 0;
    for(i=0; i <= length; i++) {
        if(bytes[i] == 0x91) {
            base_address = 0xc0 - (i+2); //sets base address to first character on line 2 minus i
            continue;
        }
        //handles newline
        i2c_write_multiple_bytes(LCD_I2C_ADDR, (uint8_t[]){0x00,base_address + i}, 2); // sets cursor address to base address
        i2c_write_multiple_bytes(LCD_I2C_ADDR, (uint8_t[]){0x40,bytes[j]}, 2);
        j++;
    }   
}

#include "status.h"

uint8_t lcd_ascii_to_byte(char ascii) {
    if( (0x41 <= ascii && ascii <= 0x5A) || // uppercase latin
        (0x61 <= ascii && ascii <= 0x7A) || // lowercase latin
        (0x20 <= ascii && ascii <= 0x3F) ) { // punctuation
        return ((uint8_t) ascii) + 0x80; // offset is the same :)
    }
    else if(0x11 <= ascii && ascii <= 0x14) {
        return ascii - 0x11;
    }
    else {
        switch (ascii) {
            case '@': return 0x80;
            case '[': return 0x8A;
            case ']': return 0x54;
            case '_': return 0x5A;
            case '|': return 0x55;
            case '\a':return 0x4E;
            case '\n':return 0x91;
            default: 
            // return CHR_BLANK_UNKNOWN;
            return CHR_INVERTED_QUESTION_MARK;
        }
    }
}


uint8_t * lcd_a2b_in_place(char * chars, uint8_t length) {
    // this is why I shouldn't be allowed near C     ~ldm
    uint8_t * bytes = (uint8_t *) chars;

    uint8_t i;
    for(i=0; i < length; i++) {
        bytes[i] = lcd_ascii_to_byte(chars[i]);;
    }
    return bytes;
}