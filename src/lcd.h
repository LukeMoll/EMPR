#ifndef EMPR_LCD
#define EMPR_LCD
#define LCD_I2C_ADDR 0x3b
void lcd_init();
void lcd_clear_screen();
void lcd_write_bytes(uint8_t * bytes, uint8_t length, uint8_t base_address);
void lcd_write_bytes_neo(uint8_t * bytes, uint8_t length, uint8_t base_address);
uint8_t lcd_ascii_to_byte(char ascii);
uint8_t * lcd_a2b_in_place(char * chars, uint8_t length);
#endif