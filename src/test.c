#include "serial.h"
// #include "status.h"
#include <lpc17xx_systick.h>

#include <stdio.h>

#include <lpc17xx_timer.h>
#include <lpc17xx_rit.h>

void RIT_IRQHandler(void);
void SysTick_Handler(void);
uint16_t counter;

int main(void) {
    // i2c_setup_polling();
    serial_init();
    // lcd_init();
    write_usb_serial_blocking("begin\r\n", 8);
    counter = 0;
    // status_code(1);
    NVIC_EnableIRQ(RIT_IRQn);
    RIT_Init(LPC_RIT);
    // status_code(2);
    RIT_TimerConfig(LPC_RIT, 1000);
    // status_code(3);
    while(1){};
    return 0;

    // SYSTICK_InternalInit(100);
    // SYSTICK_IntCmd(ENABLE);
    // SYSTICK_Cmd(ENABLE);

    // TIM_TIMERCFG_Type TIMCfg;
    // TIMCfg.PrescaleOption = TIM_PRESCALE_USVAL;
    // TIMCfg.PrescaleValue = 1000;

    //Capture basically gives the value at the timer whenever a certain event happens. 
    //Match generates an interrupt whenever the value in the timer matches the match value, optionally resets the timer

    // TIM_CAPTURECFG_Type CaptureCfg;
    // CaptureCfg.CaptureChannel = 0;
    // CaptureCfg.FallingEdge = ENABLE;
    // CaptureCfg.IntOnCaption = DISABLE;
    // CaptureCfg.RisingEdge = ENABLE;
    
    // write_usb_serial_blocking("begin\r\n", 8);

    // TIM_ConfigStructInit(TIM_TIMER_MODE, &TIMCfg);
    // TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &TIMCfg);
    // TIM_ConfigCapture(LPC_TIM0, &CaptureCfg);
    // TIM_Cmd(LPC_TIM0, ENABLE);

    // haven't tested this yet, but I think I'm shooting in the right direction with maybe a hint of light rather than randomly in the dark
    // while(1) {};

    // return 0;
}    


// void SysTick_Handler(void) {
//     SYSTICK_ClearCounterFlag();
//     uint32_t val = TIM_GetCaptureValue(LPC_TIM0, TIM_COUNTER_INCAP0);
//     char buf[100];
//     write_usb_serial_blocking(buf,
//         snprintf(buf, 99, "%ld\t%lx\r\n", val, val)
//     );

// }
void RIT_IRQHandler(void) {

    //calls interrupt fine, but then doesn't clear the interrupt flag, which is the issue.
    //RIT_TimerClearCommand should clear that flag (best guess) but appears to not exist?? for some reason??
    //so basically RIT_Interrupt_Handler gets called constantly after the first 1000 milliseconds
    RIT_TimerClearCmd(LPC_RIT, 1);

    if(counter==60069){
        char buf[100];
        write_usb_serial_blocking(buf, snprintf(buf,99, "counter = %d\r\n", counter));
        counter = 0;
    }
    else {
        counter++;
    }

    
}