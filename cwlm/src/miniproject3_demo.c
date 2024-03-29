#include <lpc17xx_pinsel.h>
#include <lpc_types.h>
#include <lpc17xx_systick.h>
#include <lpc17xx_dac.h>
#include <stdio.h>

#include "dac.h"
#include "adc.h"
#include "pwm.h"
#include "status.h"
#include "keypad.h"
#include "lcd.h"
#include "lcd_buf.h"
#include "i2c.h"
#include "generatedfuncs.h"
#include "digit.h"

#define PWM_CHANNEL 3
#define PWM_ENDVAL 0
#define PWM_PERIOD 20000
#define PWM_PRESCALE 1

#define DAC_PORT 0
#define DAC_PIN 26
#define DAC_FUNCNUM 2


uint32_t pwm_length = PWM_ENDVAL;

void SysTick_Handler(void);

uint8_t demo_stage = 0;
uint16_t keypad_state;

void Stage2();
void Stage3();
void Stage4_setup();
void Stage4();
uint16_t sine_wave(uint8_t tick, uint8_t amplitudeDenom);


int main(void) {    
    adc_setup(MP3_ADC_PORT, MP3_ADC_PIN, MP3_ADC_CHANNEL);
    
    dac_init(DAC_PORT, DAC_PIN, DAC_FUNCNUM);
    DAC_UpdateValue(LPC_DAC, 0xFFC);
    
    i2c_setup_polling();
    lcd_init();
    lcd_buf_flush();
    digit_update();

    SYSTICK_InternalInit(1);
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);

    while(1) {}; // stop the main thread from terminating

    return 1;
}

void SysTick_Handler(void) {
    SYSTICK_ClearCounterFlag();

    uint8_t pressedKey;
    keypad_state = keypad_read_diff(&pressedKey, keypad_read());
    
    if(keypad_state) {
        // key has been pressed (triggered on keydown)
        demo_stage++;
        digit_writeDecValue(demo_stage, false);
        digit_update();
    }
    // current stage function should be called repeatedly
    switch(demo_stage) {
        case 0:
            lcd_buf_clear_screen();            
            lcd_buf_write_string("Press any key...", 16, 0);
            break;
        case 1:
            status_code(1); 
            lcd_buf_clear_screen();
            lcd_buf_write_string("Stage 2: , 100ms", 16, 0);
            lcd_buf_write_string("DAC sine wave", 13, 16);
            Stage2();
            break;
        
        case 2: 
            status_code(2);
            lcd_buf_clear_screen();
            lcd_buf_write_string("Stage 3: , 100ms", 16, 0);
            lcd_buf_write_string("ADC->DAC mirror", 15, 16);
            Stage3();
            break;
        
        case 3:
            status_code(3);
            Stage4_setup();
            demo_stage++;
        case 4: 
            status_code(4);
            lcd_buf_clear_screen();
            lcd_buf_write_string("Stage 4: ' 200us", 16, 0);
            lcd_buf_write_string("changing PWM", 12, 16);
            Stage4();
            break;
        
        case 5: 
            status_code(5);
            lcd_buf_clear_screen();
            lcd_buf_write_string_multi("program \nterminated", 19, 0, true);
            demo_stage++;
            break;
        case 6:
            // don't keep writing to the screen
            break;
        default:
            ; // it's yaboi declaration-is-not-a-statement :^)))
            status_code(0);
            char inv_buf[17];
            lcd_buf_clear_screen();
            uint8_t inv_length = snprintf(inv_buf, 16, "INVALIDSTAGE %02d", demo_stage);
            lcd_buf_write_string(inv_buf, inv_length, 0);
            break;
        
    }
    lcd_buf_update();
}

uint16_t s2_tick = 0;
uint8_t s2_amp, s2_freq;
void Stage2() {
    if(s2_tick % 1000 == 0) {
        // change amp and freq at 1000 tick intervals
        // while SysTick is being called every 1ms, this should it changes every second
        // in reality it is much slower
        // probably because the handler code takes more than 1ms to execute
        // a freerunning timer would yield more accurate results
        s2_tick = 1;
        s2_amp =  s2_amp  % 4 + 1;
        s2_freq = s2_freq % 4 + 1;
    }
    DAC_UpdateValue(LPC_DAC, sine_wave( (s2_tick++)*s2_freq % 256, s2_amp));
}

void Stage3() {
    uint16_t data_plain = adc_read(MP3_ADC_CHANNEL);
    float voltage = to_voltage(data_plain);
    
    uint16_t dac_value = voltage_to_dac(voltage);
    DAC_UpdateValue(LPC_DAC, dac_value);
}

void Stage4_setup() {
    PWM_setup_timer(PWM_CHANNEL, PWM_PRESCALE);
    PWM_setup_period(PWM_PERIOD);
    PWM_setup_single_edge(PWM_CHANNEL, pwm_length);
    PWM_start();
}

void Stage4() {
    switch(pwm_length) {
        case PWM_PERIOD :
            // after one full cycle, demo should end
            demo_stage++;
            break;
        default:
            pwm_length += 8;
            break;
    }

    PWM_setup_single_edge(PWM_CHANNEL, pwm_length);
}

/**
 * Generates a sine wave from a 8-bit tick
 *  tick: [0,255]
 *  returns: [0,1023]
 */
uint16_t sine_wave(uint8_t tick, uint8_t amplitudeDenom) {
    return GENERATED_SIN_INT16[tick]/(amplitudeDenom*64) + 512;
}