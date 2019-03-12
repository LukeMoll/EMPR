#include <lpc17xx_rit.h>
#include <lpc17xx_clkpwr.h> // https://youtu.be/h-mUGj41hWA
#include <lpc17xx_gpio.h>
#include <stdbool.h>
#include <libempr/serial.h>
#include <libempr/status.h>

#define GPMASK 1<<31

void RIT_TimerConfig_Neo(LPC_RIT_TypeDef *RITx, uint32_t time_interval) {
        uint32_t clock_rate, cmp_value, divr;
        // CHECK_PARAM(PARAM_RITx(RITx));
        // private macro

		// from CLKPWR_GETPCLK
        divr = CLKPWR_GetPCLKSEL(CLKPWR_PCLKSEL_RIT);
        switch (divr) {
        case 0:
        	divr = 4;
        	break;
        case 1:
            divr = 1;
            break;
        case 2:
            divr = 2;
            break;
        case 3:
        	divr = 8;
        	break;
        }

        // Get PCLK value of RIT
        clock_rate = CLKPWR_GetPCLK(CLKPWR_PCLKSEL_RIT);
        // serial_printf("CCLK: %u\r\nDivider: %u\r\nPCLK: %u\r\n", SystemCoreClock, divr, clock_rate);

        /* calculate compare value for RIT to generate interrupt at
         * specified time interval
         * COMPVAL = (RIT_PCLK * time_interval)/1000
         * (with time_interval unit is millisecond)
         */
        cmp_value = (clock_rate /1000) * time_interval;
        RITx->RICOMPVAL = cmp_value;
        // serial_printf("Time interval: %ums\r\nCompare value: %u\r\n", time_interval, cmp_value);

        /* Set timer enable clear bit to clear timer to 0 whenever
         * counter value equals the contents of RICOMPVAL
         */
        RITx->RICTRL |= (1<<1);
}

uint32_t RIT_TimerConfig_us(LPC_RIT_TypeDef *RITx, uint32_t microseconds) {
    	uint32_t clock_rate, cmp_value;
    	// CHECK_PARAM(PARAM_RITx(RITx));

		// remove divider
		CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_RIT, 1);

        // Get PCLK value of RIT
    	clock_rate = CLKPWR_GetPCLK(CLKPWR_PCLKSEL_RIT);

        /* calculate compare value for RIT to generate interrupt at
         * specified time interval
         * COMPVAL = (RIT_PCLK * time_interval)/1000
         * (with time_interval unit is millisecond)
         */
        cmp_value = (clock_rate /1000000) * microseconds;
        RITx->RICOMPVAL = cmp_value;

        /* Set timer enable clear bit to clear timer to 0 whenever
         * counter value equals the contents of RICOMPVAL
         */
        RITx->RICTRL |= (1<<1);
		return cmp_value;
}

void RIT_ClearPCLKDiv() {
	uint32_t bitval = 1;
	serial_printf("Setting RIT PCLK divider value to %d\r\n", bitval);
	CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_RIT, bitval);
}

int main(void) {
        uint32_t time_i = 50;
        serial_init();
		serial_puts("\r\n===================================\r\n");
        // serial_puts("test_fastrit started!\r\n");

		// uint32_t gpmask = 1<<31; // GPIO 1.31, pin 20
		GPIO_SetDir(1, GPMASK, 1);

		status_code(0xf);


        RIT_Init(LPC_RIT);
        // RIT_TimerConfig_Neo(LPC_RIT, time_i);
		uint32_t compval = RIT_TimerConfig_us(LPC_RIT, time_i);
		// RIT_TimerConfig(LPC_RIT, time_i);
        NVIC_EnableIRQ(RIT_IRQn);

		serial_printf("compare value: %u\r\n", compval);

        // serial_printf("Calling RIT_TimerConfig_Neo with\r\n\ttime_interval = %u\r\n", time_i);

        // serial_puts("Enabling IRQ\r\n");
        // serial_puts("ending main, entering busyloop\r\n");
        while(1) {};
        return 0;
}

/**
 * Calling RIT_TimerConfig_Neo with
 *   time_interval = 1000
 * Divider: 0
 * PCLK: 25000000
 * Time interval: 1000ms
 * Compare value: 25000000

 */

void gpio_tick(bool b) {
	if(b) {
		GPIO_ClearValue(1, GPMASK);
	}
	else {
		GPIO_SetValue(1, GPMASK);
	}
}
// uint16_t code = 0;
uint16_t tick = 0;
void RIT_IRQHandler(void) {
    // IntStatus s = 
	RIT_GetIntStatus(LPC_RIT);
	// if(s == SET) {
	// 	status_code(0b0110);
	// }
	// else {
	// 	status_code(0b1001);
	// }
	
 	tick++;
    // status_code(tick >> 12);
	gpio_tick(tick&1 ? true:false);
	// status_code((code++) >> 0);
    // serial_puts("RIT_IRQHandler called!\r\n");

}
