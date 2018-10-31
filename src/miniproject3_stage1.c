#include <lpc17xx_adc.h>
#include <lpc17xx_pinsel.h>
#include <lpc_types.h>
#include <stdio.h>
#include <lpc17xx_systick.h>
#include "serial.h"
#include "i2c.h"
#include "lcd.h"
#include "lcd_buf.h"

#define MP3_ADC_PORT 0
#define MP3_ADC_PIN 23
#define MP3_ADC_CHANNEL 0
#define MP3_ADC_VREF 3.3

void adc_setup(uint8_t port, uint8_t pin, uint8_t channel);
uint16_t adc_read(uint8_t channel);
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

void adc_setup(uint8_t port, uint8_t pin, uint8_t channel) {
    // Set up the input pin
    PINSEL_CFG_Type PinCfg;
    PinCfg.Funcnum = 1; // might need to parameterise this in future, ADC is different funcs on different pins :/
    PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    PinCfg.Pinmode = PINSEL_PINMODE_TRISTATE; 
    PinCfg.Portnum = port;
    PinCfg.Pinnum =  pin;

    PINSEL_ConfigPin(&PinCfg);

    ADC_Init(LPC_ADC, 200000);
    
    ADC_StartCmd(LPC_ADC, ADC_START_NOW);
    ADC_ChannelCmd(LPC_ADC, channel, ENABLE);
}

uint16_t adc_read(uint8_t channel) {
    /**
     * ADC seems to have strange voltage response with high resistances (1000's of ohms)
     */
    ADC_StartCmd(LPC_ADC, ADC_START_NOW); // might be needed for multiple reads?
    // FlagStatus status_chan = // can assign the status if you want
    ADC_ChannelGetStatus(LPC_ADC, channel, 1); // this is needed here, for some reason ¯\_(ツ)_/¯
    return ADC_GetData(channel);
}

void SysTick_Handler(void) {
    SYSTICK_ClearCounterFlag();
    uint16_t data_plain = adc_read(MP3_ADC_CHANNEL);
    float voltage = (((float) data_plain) * MP3_ADC_VREF) / 4095.0;
    char buf[100];
    char lcd_buf[16];
    uint8_t lcd_len = snprintf(lcd_buf, 16, "%06.4f",voltage);

    lcd_buf_write_string(lcd_buf, lcd_len, 0);
    lcd_buf_update();
    // write_usb_serial_blocking(buf, snprintf(buf, 99, "%fV\r\n", voltage));
}