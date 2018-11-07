#include <lpc17xx_adc.h>
#include <lpc17xx_pinsel.h>
#include <lpc_types.h>
#include <stdio.h>
#include <lpc17xx_systick.h>
#include "serial.h"
#include "i2c.h"
#include "lcd.h"
#include "lcd_buf.h"
#include "adc.h"

void Systick_Handler(void);

int main(void) {
    serial_init();

    adc_setup(MP3_ADC_PORT, MP3_ADC_PIN, MP3_ADC_CHANNEL);

    i2c_setup_polling();
    lcd_init();
    lcd_buf_flush();
    
    SYSTICK_InternalInit(100);
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);

    while(1) {} // stop the main thread from terminating

    return 1;
}

void SysTick_Handler(void) {
    SYSTICK_ClearCounterFlag();
    uint16_t data_plain = adc_read(MP3_ADC_CHANNEL);
    float voltage = to_voltage(data_plain);
    // char buf[100];
    char lcd_buf[16];
    uint8_t lcd_len = snprintf(lcd_buf, 16, "%06.4f",voltage);

    lcd_buf_write_string(lcd_buf, lcd_len, 0);
    lcd_buf_update();
    // write_usb_serial_blocking(buf, snprintf(buf, 99, "%fV\r\n", voltage));
}