#include <lpc17xx_adc.h>
#include <lpc17xx_pinsel.h>
#include <lpc_types.h>
#include <stdio.h>
#include <lpc17xx_systick.h>
#include <stdlib.h>

#include "serial.h"
#include "i2c.h"
#include "lcd.h"
#include "lcd_buf.h"
#include "adc.h"
#include "keypad.h"
#include "keymap.h"

#define DISTANCE_MEASUREMENT 1
#define VOLTAGE_MEASUREMENT 2

void Systick_Handler(void);
float voltage_to_cm(float voltage);

uint8_t measurement_type = 0;
float voltage;
uint8_t lcd_len;
char lcd_buf[33];

int main(void) {
    serial_init();

    adc_setup(MP3_ADC_PORT, MP3_ADC_PIN, MP3_ADC_CHANNEL);

    i2c_setup_polling();
    lcd_init();
    lcd_buf_flush();

    lcd_buf_write_string("1: Distance", 11, 0);
    lcd_buf_write_string("2: Voltage", 10, 16);
    lcd_buf_update();
    status_code(4);
    
    SYSTICK_InternalInit(100);
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);

    while(1) {}; // stop the main thread from terminating

    return 1;
}

void SysTick_Handler(void) {

    SYSTICK_ClearCounterFlag();

    uint16_t data_plain = adc_read(MP3_ADC_CHANNEL);
    voltage = to_voltage(data_plain);
    uint16_t keypad_state = keypad_read();
    uint8_t pressedKey;

    switch(measurement_type) {

        case DISTANCE_MEASUREMENT: 
            ;
            if(keypad_read_diff(&pressedKey, keypad_state) != 0) {
                char read_char[1];
                read_char[0] = keymap_get_ascii_character(pressedKey);
                measurement_type = atoi(read_char);
                status_code(measurement_type);
            }
            else {
                float cms = voltage_to_cm(voltage);
                lcd_len = snprintf(lcd_buf, 32, "Distance:\n%06.4f cm ",cms);

                lcd_buf_write_string_multi(lcd_buf, lcd_len, 0, true);
            }
            break;

        case VOLTAGE_MEASUREMENT:
            ;
            if(keypad_read_diff(&pressedKey, keypad_state) != 0) {
                char read_char[1];
                read_char[0] = keymap_get_ascii_character(pressedKey);
                measurement_type = atoi(read_char);
                status_code(measurement_type);
            }
            else {
                lcd_len = snprintf(lcd_buf, 32, "Voltage: \n%06.4f V",voltage);

                lcd_buf_write_string_multi(lcd_buf, lcd_len, 0, true);
            }
            break;

        default:
            //whenever someone either doesn't press a key
            //or they press something that isn't 1 or 2
            ;
            
            if(keypad_read_diff(&pressedKey, keypad_state) != 0) {
                char read_char[1];
                read_char[0] = keymap_get_ascii_character(pressedKey);
                measurement_type = atoi(read_char);
                status_code(measurement_type);
            }
            else {
                measurement_type = 0;
                status_code(6);
            }

            break;
        }
    lcd_buf_update();
    
}

float voltage_to_cm(float voltage) {
    //figure out the function, convert it to cm
    //maybe just use a lookup table
    return (voltage * 3);
}

