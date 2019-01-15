#include "lcd.h"

#include "i2c.h"

#define LCD_ADDR 0x3b

#define CHAR_AT 0x80
#define CHAR_DOLLAR 0x82
#define CHAR_EMPTY 0x91

static uint8_t row_, col_;

void lcd_init()
{
	uint8_t init[11] = { 0x00, 0x34, 0x0c, 0x06, 0x35, 0x04, 0x10, 0x42, 0x9f, 0x34, 0x02 };
	i2c_send(LCD_ADDR, init, 11);

	lcd_clear_display();

	row_ = 0;
	col_ = 0;
}

void lcd_clear_display()
{
	uint8_t display_off[2] = { 0x00, 0x08 };
	i2c_send(LCD_ADDR, display_off, 2);

	uint8_t set_ddram[2] = { 0x00, 0x80 };
	i2c_send(LCD_ADDR, set_ddram, 2);

	uint8_t write_ddram[2] = { 0x40, CHAR_EMPTY };
	uint8_t i;
	for (i = 0x00; i < 0x80; i++)
		i2c_send(LCD_ADDR, write_ddram, 2);

	uint8_t display_on[2] = { 0x00, 0x0c };
	i2c_send(LCD_ADDR, display_on, 2);

	lcd_set_cursor(0, 0);
}

void lcd_set_cursor(uint8_t row, uint8_t col)
{
	uint8_t row_start;
	if (row == 0)
		row_start = 0x80;
	else 
		row_start = 0xc0;

	uint8_t set_ddram[2] = { 0x00, row_start + col };
	i2c_send(LCD_ADDR, set_ddram, 2);

	row_ = row;
	col_ = col;
}

void lcd_putc(char c) {
	uint8_t byte;

	if (c == '$') {
		byte = CHAR_DOLLAR;
	} else if (c == '@') {
		byte = CHAR_AT;
	} else {
		byte = ((uint8_t)c) + 128;
	}

	uint8_t write_ddram[2] = { 0x40, byte };
	i2c_send(LCD_ADDR, write_ddram, 2);

	if (++col_ >= 16) {
		if (row_ == 0)
			lcd_set_cursor(1, 0);
		else
			lcd_set_cursor(0, 0);
	}
}

void lcd_puts(char *s) {
	while (*s)
		lcd_putc(*s++);
}
