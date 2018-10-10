#include "i2c.h"
#include "status.h"
#include <lpc_types.h>
#include <lpc17xx_systick.h>

int main(void);
void busy_wait(uint8_t duration);
void lcd_init(); // TODO: remove and put in header file ready for modularisation

#define LCD_I2C_ADDR 0x3b

int main(void) {

    //weird initialition thingy which we can put into a function later yeah idk what this is either
    i2c_setup_polling();
    lcd_init();

    uint8_t chars[] = { // "Hello World!"
        0xC8,
        0x65,
        0x6C,
        0x6C,
        0x6F,
        0x91,
        0xD7,
        0x6F,
        0x72,
        0x6C,
        0x64,
        0xA1
    };
    uint8_t i, length = 12;
    for(i=0; i < length; i++) {
        i2c_write_multiple_bytes(LCD_I2C_ADDR, (uint8_t[]){0x00,0x80 + i}, 2); // sets cursor address (80 is first char)
        i2c_write_multiple_bytes(LCD_I2C_ADDR, (uint8_t[]){0x40,chars[i]}, 2); // sets addresss in character set to read into cursor address
    }

    return 0;
}

void lcd_init() {
    // init sequence: one control byte and ten instructions (as in lecture slides, see PCF2119x datasheet Table 12)
    i2c_write_multiple_bytes(LCD_I2C_ADDR, (uint8_t[]){
        0x00, // control byte: instruction register, last control byte of this i2c transfer
        0x34, // 0x34 = function_set: 2x4 bits, 2lx16c, 1:9mplx mode, basic + standard instruction set
        0x0c, // 0x0c = display_ctl: display on, cursor off, blink off
        0x06, // 0x06 = curs_disp_shift: move cursor, right shift.
        0x35, // 0x35 = function_set: 2x4 bits, 2lx16c, 1:9mplx mode, basic + extended instruction set
        0x04, // here be ¯\_(ツ)_/¯s
        0x10, // ...
        0x42, // ...
        0x9f, // ...
        0x34, // ...
        0x02  // last byte of init sequence
        }, 11); 

    i2c_write_multiple_bytes(LCD_I2C_ADDR, (uint8_t[]){0x00, 0x01}, 2);
    // clear the screen (puts right arrow in all positions), need to wait 166 Fosc cycles.

    busy_wait(1000); // can possibly be reduced, and ideally allow systick to be used for something else
}

uint8_t waiting = 0;
uint8_t duration;
uint8_t cycles = 0;

void busy_wait(uint8_t msToWait){
    SYSTICK_InternalInit(100);
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);
    duration = msToWait;
    cycles = 0;
    waiting = 1;
    while(waiting) {}

    SYSTICK_IntCmd(DISABLE);
    SYSTICK_Cmd(DISABLE);
}

void SysTick_Handler(void) {
    cycles++;
    if((cycles * 10) > duration) {
        waiting = 0;
    }
}
