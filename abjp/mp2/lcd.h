#include <stdint.h>

#ifndef _LCD_H
#define _LCD_H

void lcd_init();

void lcd_clear_display();
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_putc(char c);
void lcd_puts(char *s);

#endif /* _LCD_H */
