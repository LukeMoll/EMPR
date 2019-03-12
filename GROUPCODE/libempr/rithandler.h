#ifndef EMPR_RITHANDLER
#define EMPR_RITHANDLER

    #include <stdint.h>
    #include <lpc17xx_rit.h>
    #include <lpc17xx_clkpwr.h>

    void RIT_setHandler(void (*h)(void));
    void (*RIT_getHandler(void))(void);
    uint32_t RIT_TimerConfig_us(LPC_RIT_TypeDef *RITx, uint32_t microseconds);
    void RIT_IRQHandler(void);

#endif