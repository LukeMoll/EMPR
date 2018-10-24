#ifndef EMPR_LCD_BUF
#define EMPR_LCD_BUF
void lcd_buf_write_byte(uint8_t byte, uint8_t index);
void lcd_buf_write_bytes(uint8_t * bytes, uint8_t length, uint8_t base_index);
void lcd_buf_update();
void lcd_buf_flush();
void lcd_buf_write_string(char * string, uint8_t length, uint8_t base_index);
#endif