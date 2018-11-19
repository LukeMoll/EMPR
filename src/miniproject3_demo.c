#include <lpc17xx_pinsel.h>
#include <lpc_types.h>
#include <lpc17xx_systick.h>

#include "dac.h"
#include "adc.h"
#include "pwm.h"
#include "status.h"
#include "keypad.h"

#define PWM_CHANNEL 3
#define PWM_ENDVAL 0
#define PWM_PERIOD 20000
#define PWM_PRESCALE 1

uint32_t pwm_length = PWM_ENDVAL;

void SysTick_Handler(void);
uint8_t tick = 0;
uint8_t state

void Stage2(void);
void Stage3(void);
void Stage4_setup(void);
void Stage4(void);

//got the stage 3, 4_setup and 4 working
//haven't yet finished the tick handler (with keypad_read)
//haven't yet made it pretty

int main(void) {
    
    adc_setup(MP3_ADC_PORT, MP3_ADC_PIN, MP3_ADC_CHANNEL);

    dac_init(DAC_PORT, DAC_PIN, DAC_FUNCNUM);
    DAC_UpdateValue(LPC_DAC, 0xFFC);

    SYSTICK_InternalInit(1);
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);

    while(1) {} // stop the main thread from terminating

    return 1;
}

void SysTick_Handler(void) {
    SYSTICK_ClearCounterFlag();

    state = keypad_read();
    if(state) {
        tick++
    }
    // switch(state) {
    //     case(1) {
    //         Stage2(void);
    //     }
    //     case(2) {

    //     }
    // }
}

void Stage2(void) {
    status_code(1)
}

void Stage3(void) {
    uint16_t data_plain = adc_read(MP3_ADC_CHANNEL);
    float voltage = to_voltage(data_plain);
    
    uint16_t dac_value = voltage_to_dac(voltage);
    DAC_UpdateValue(LPC_DAC, dac_value);
}

void Stage4_setup(void) {
    PWM_setup_timer(PWM_CHANNEL, PWM_PRESCALE);
    PWM_setup_period(PWM_PERIOD);
    PWM_setup_single_edge(PWM_CHANNEL, length);
    PWM_start();
    tick++;
}

void Stage4(void) {
    switch(length) {
        case PWM_PERIOD :
            length = 0;
            break;
        default:
            length += 4;
            break;
    }

    PWM_setup_single_edge(PWM_CHANNEL, length);
}

