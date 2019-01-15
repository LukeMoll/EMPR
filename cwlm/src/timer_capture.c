/***********************************************************************/
#include <lpc17xx_timer.h>
#include <lpc17xx_libcfg.h>
#include <lpc17xx_pinsel.h>
#include <lpc17xx_gpio.h>

/* Example group ----------------------------------------------------------- */
/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"********************************************************************************\n\r"
"Hello NXP Semiconductors \n\r"
"Timer Match interrupt demo \n\r"
"\t - MCU: LPC17xx \n\r"
"\t - Core: ARM Cortex-M3 \n\r"
"\t - Communicate via: UART0 -  115200 bps \n\r"
" Use timer 0 to take a snapshot of the timer value when an input signal \n\r"
" on CAP0.0 transitions \n\r"
"********************************************************************************\n\r";

// UART Configuration structure variable
UART_CFG_Type UARTConfigStruct;
uint32_t MatchCount;

//timer init
TIM_TIMERCFG_Type TIM_ConfigStruct;
TIM_MATCHCFG_Type TIM_MatchConfigStruct ;
TIM_CAPTURECFG_Type TIM_CaptureConfigStruct;
uint8_t volatile timer0_flag = FALSE, timer1_flag = FALSE;

/************************** PRIVATE FUNCTIONS *************************/
/* Interrupt service routines */
void TIMER0_IRQHandler(void);
void TIMER1_IRQHandler(void);

void print_menu(void);
/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************/
void TIMER0_IRQHandler(void)
{
        if (TIM_GetIntCaptureStatus(LPC_TIM0,0))
        {
                TIM_ClearIntCapturePending(LPC_TIM0,0);
                _DBG("Time capture: ");
                _DBH32(TIM_GetCaptureValue(LPC_TIM0,0));_DBG_("");
        }
}

/*********************************************************************/
void TIMER1_IRQHandler(void)
{
        if (TIM_GetIntCaptureStatus(LPC_TIM1,0))
        {
                TIM_ClearIntCapturePending(LPC_TIM1,0);
                _DBG("Time capture: ");
                _DBH32(TIM_GetCaptureValue(LPC_TIM1,0));_DBG_("");
        }
}

/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************/
void print_menu(void)
{
        _DBG(menu1);
}


/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************/
int c_entry(void)
{
        PINSEL_CFG_Type PinCfg;

        /* Initialize debug via UART0
         * � 115200bps
         * � 8 data bit
         * � No parity
         * � 1 stop bit
         * � No flow control
         */
        debug_frmwrk_init();

        // print welcome screen
        print_menu();

        //Config P1.26 as CAP0.0
        PinCfg.Funcnum = 3;
        PinCfg.OpenDrain = 0;
        PinCfg.Pinmode = 0;
        PinCfg.Portnum = 1;
        PinCfg.Pinnum = 26;
        PINSEL_ConfigPin(&PinCfg);

        // Initialize timer 0, prescale count time of 1000000uS = 1S
        TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
        TIM_ConfigStruct.PrescaleValue  = 1000000;

        // use channel 0, CAPn.0
        TIM_CaptureConfigStruct.CaptureChannel = 0;
        // Enable capture on CAPn.0 rising edge
        TIM_CaptureConfigStruct.RisingEdge = ENABLE;
        // Enable capture on CAPn.0 falling edge
        TIM_CaptureConfigStruct.FallingEdge = ENABLE;
        // Generate capture interrupt
        TIM_CaptureConfigStruct.IntOnCaption = ENABLE;


        // Set configuration for Tim_config and Tim_MatchConfig
        TIM_Init(LPC_TIM0, TIM_TIMER_MODE,&TIM_ConfigStruct);
        TIM_ConfigCapture(LPC_TIM0, &TIM_CaptureConfigStruct);
        TIM_ResetCounter(LPC_TIM0);


        /* preemption = 1, sub-priority = 1 */
        NVIC_SetPriority(TIMER0_IRQn, ((0x01<<3)|0x01));
        /* Enable interrupt for timer 0 */
        NVIC_EnableIRQ(TIMER0_IRQn);
        // To start timer 0
        TIM_Cmd(LPC_TIM0,ENABLE);

        while (1);
        return 1;
}

/* Support required entry point for other toolchain */
int main (void)
{
        return c_entry();
}