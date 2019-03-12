#ifndef EMPR_LCD_BUF
#define EMPR_LCD_BUF
#include <stdbool.h>
void lcd_buf_write_byte(uint8_t byte, uint8_t index);
void lcd_buf_write_bytes(uint8_t * bytes, uint8_t length, uint8_t base_index);
void lcd_buf_update();
void lcd_buf_flush();
void lcd_buf_clear_screen();
void lcd_buf_write_string(char * string, uint8_t length, uint8_t base_index);
void lcd_buf_write_string_multi(char * string, uint8_t length, uint8_t base_index, bool clear_skipped);
#endif