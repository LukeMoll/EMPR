#include <lpc17xx_rit.h>
#include <lpc17xx_clkpwr.h> // https://youtu.be/h-mUGj41hWA
#include <lpc17xx_gpio.h>
#include <stdbool.h>
#include <libempr/serial.h>
#include <libempr/status.h>
#include <libempr/adc.h>
#include <stddef.h>

uint32_t RIT_TimerConfig_us(LPC_RIT_TypeDef *RITx, uint32_t microseconds);

void init();

void main(void) {
    init();
    while(1) {};
    return;
}


#define BUF_LEN 2048
uint16_t buf[BUF_LEN];
void init(){

    serial_init();
    serial_puts("Hello!\r\n");
    adc_setup(ADC_PORT, ADC_PIN, ADC_FUNCNUM, ADC_CHANNEL);

    RIT_Init(LPC_RIT);
    RIT_TimerConfig_us(LPC_RIT, 125);
    NVIC_EnableIRQ(RIT_IRQn);
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

void dump_buf(uint16_t *buf, size_t len) {
    NVIC_DisableIRQ(RIT_IRQn);
    size_t i;
    serial_printf("Buffer dump:\r\nIndex\tValue\r\n");
    for(i=0; i<len; i++) {
        serial_printf("%d\t%d\r\n", i, buf[i]);
    }
    NVIC_EnableIRQ(RIT_IRQn);
}

static uint16_t min = 0xFFFF, max = 0;
static uint32_t sum = 0;
static uint16_t seq = 0;
void RIT_IRQHandler(void) {
	if(RIT_GetIntStatus(LPC_RIT) == SET) {
        // status_code(0xf);
    }
    else {
        // status_code(1);
    }

    buf[seq] = adc_read(ADC_CHANNEL);
    if(buf[seq] < min) {min = buf[seq];}
    else if (buf[seq] > max) {max = buf[seq];};
    sum += buf[seq];

    if(++seq >= BUF_LEN) {
        serial_printf("%d samples taken.\r\n\tmin: %d\tmax: %d\r\n\tmean: %d\r\n\n", seq, min, max, sum >> 11);
        dump_buf(buf, BUF_LEN);
        min = 0;
        max = 0;
        sum = 0;
        seq = 0;
    }

}