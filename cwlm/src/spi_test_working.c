#include <lpc17xx_spi.h>
#include <lpc17xx_pinsel.h>
#include <lpc17xx_gpio.h>
#include "status.h"

/**
 * Attach probes to P13 (SCK) and P11 (MOSI)
 */

// /CS on 0.16 (MBed pin 14)
#define CS_PORT_NUM 0
#define CS_PIN_NUM 16

SPI_CFG_Type SPI_ConfigStruct;

#define SPI_DATABIT_SIZE                10
#define BUFFER_SIZE                     0x40
#if (SPI_DATABIT_SIZE == 8)
uint8_t Tx_Buf[BUFFER_SIZE];
uint8_t Rx_Buf[BUFFER_SIZE];
#else
uint16_t Tx_Buf[BUFFER_SIZE];
uint16_t Rx_Buf[BUFFER_SIZE];
#endif

void CS_Init(void);
void CS_Force(int32_t state);
void Buffer_Init(void);
void Error_Loop(void);
void Buffer_Verify(void);

void CS_Init(void) {
        GPIO_SetDir(CS_PORT_NUM, (1<<CS_PIN_NUM), 1);
        GPIO_SetValue(CS_PORT_NUM, (1<<CS_PIN_NUM));
}

void CS_Force(int32_t state) {
        if (state){
                GPIO_SetValue(CS_PORT_NUM, (1<<CS_PIN_NUM));
        }else{
                GPIO_ClearValue(CS_PORT_NUM, (1<<CS_PIN_NUM));
        }
}

void Buffer_Init(void) {
        uint8_t i;
#if (SPI_DATABIT_SIZE == 8)
        for (i = 0; i < BUFFER_SIZE; i++) {
                Tx_Buf[i] = i;
                Rx_Buf[i] = 0;
        }
#else
        for (i = 0; i < BUFFER_SIZE/2; i++) {
                Tx_Buf[i] = i;
                Rx_Buf[i] = 0;
        }
#endif
}

void Error_Loop(void) {
        /* Loop forever */
        while (1);
}

void Buffer_Verify(void) {
        uint8_t i;
#if (DATA_SIZE==8)
        uint8_t *src_addr = (uint8_t *) &Tx_Buf[0];
        uint8_t *dest_addr = (uint8_t *) &Rx_Buf[0];
        for ( i = 0; i < SPI_DATABIT_SIZE; i++ )
        {
                if ( *src_addr++ != *dest_addr++ )
                {
                        /* Call Error Loop */
                        status_code(11);
                        Error_Loop();
                }
        }
#else
        uint16_t *src_addr = (uint16_t *) &Tx_Buf[0];
        uint16_t *dest_addr = (uint16_t *) &Rx_Buf[0];
        for ( i = 0; i < SPI_DATABIT_SIZE/2; i++ )
        {
                if ( *src_addr++ != *dest_addr++ )
                {
                        /* Call Error Loop */
                        status_code(11);
                        Error_Loop();
                }
        }
#endif
}

int main(void)
{
        PINSEL_CFG_Type PinCfg;
        SPI_DATA_SETUP_Type xferConfig;
        uint32_t tmp;

        /*
         * Initialize SPI pin connect
         * P0.15 - SCK;
         * P0.16 - SSEL - used as GPIO
         * P0.17 - MISO
         * P0.18 - MOSI
         */
        PinCfg.Funcnum = 3;
        PinCfg.OpenDrain = 0;
        PinCfg.Pinmode = 0;
        PinCfg.Portnum = 0;
        PinCfg.Pinnum = 15;
        PINSEL_ConfigPin(&PinCfg);
        PinCfg.Pinnum = 17;
        PINSEL_ConfigPin(&PinCfg);
        PinCfg.Pinnum = 18;
        PINSEL_ConfigPin(&PinCfg);
        PinCfg.Pinnum = 16;
        PinCfg.Funcnum = 0;
        PINSEL_ConfigPin(&PinCfg);


        SPI_ConfigStruct.CPHA = SPI_CPHA_SECOND;
        SPI_ConfigStruct.CPOL = SPI_CPOL_LO;
        // SPI_ConfigStruct.ClockRate = 2000000;
        SPI_ConfigStruct.ClockRate = 10000;
        SPI_ConfigStruct.DataOrder = SPI_DATA_MSB_FIRST;
        SPI_ConfigStruct.Databit = SPI_DATABIT_SIZE;
        SPI_ConfigStruct.Mode = SPI_MASTER_MODE;
        // Initialize SPI peripheral with parameter given in structure above
        SPI_Init(LPC_SPI, &SPI_ConfigStruct);

        status_code(1);

        Buffer_Init();
        CS_Init();
        CS_Force(0);

        status_code(2);

        // delay for a while
        for (tmp = 10000; tmp; tmp--);
        status_code(3);

        xferConfig.tx_data = Tx_Buf;
        xferConfig.rx_data = Rx_Buf;
        xferConfig.length = BUFFER_SIZE;
        SPI_ReadWrite(LPC_SPI, &xferConfig, SPI_TRANSFER_POLLING);
        status_code(4);
        // delay for a while
        for (tmp = 10000; tmp; tmp--);
        CS_Force(1);
        status_code(5);

        // Verify buffer after transferring
        Buffer_Verify();
        SPI_DeInit(LPC_SPI);
        status_code(6);
    /* Loop forever */
    while(1);
    return 1;
}