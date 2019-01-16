// #include <lpc17xx_spi.h>
#include <lpc17xx_systick.h>
// #include <lpc17xx_pinsel.h>
// #include <lpc_types.h>
#include <stddef.h>
// #include <lpc17xx_libcfg.h>

// #include "pins.h"
#include "status.h"

#define PORT 0
#define FUNCNUM 3
#define MOSI_PIN 18
#define MISO_PIN 17
#define SCK_PIN 15

void SystickHandler(void);
uint16_t data = 0x53;

int main(void) {

    //do all the spi setup
    //pinsel setup

    //MOSI SETUP

    PINSEL_CFG_Type PinCfg_Mosi;
    PinCfg_Mosi.Funcnum = FUNCNUM;
    PinCfg_Mosi.OpenDrain = PINSEL_PINMODE_NORMAL;
    PinCfg_Mosi.Pinmode = PINSEL_PINMODE_PULLUP;
    PinCfg_Mosi.Portnum = PORT;
    PinCfg_Mosi.Pinnum = MOSI_PIN;

    PINSEL_ConfigPin(&PinCfg_Mosi);

    //MISO SETUP

    PINSEL_CFG_Type PinCfg_Miso;
    PinCfg_Miso.Funcnum = FUNCNUM;
    PinCfg_Miso.OpenDrain = PINSEL_PINMODE_NORMAL;
    PinCfg_Miso.Pinmode = PINSEL_PINMODE_PULLUP;
    PinCfg_Miso.Portnum = PORT;
    PinCfg_Miso.Pinnum = MISO_PIN;

    PINSEL_ConfigPin(&PinCfg_Miso);

    //SCK setup

    PINSEL_CFG_Type PinCfg_Sck;
    PinCfg_Sck.Funcnum = FUNCNUM;
    PinCfg_Sck.OpenDrain = PINSEL_PINMODE_NORMAL;
    PinCfg_Sck.Pinmode = PINSEL_PINMODE_PULLUP;
    PinCfg_Sck.Portnum = PORT;
    PinCfg_Sck.Pinnum = SCK_PIN;

    PINSEL_ConfigPin(&PinCfg_Sck);

    //initialise probably
    
    SPI_CFG_Type Spi_Cfg;
    // SPI_ConfigStructInit(&SpiCfg);
    Spi_Cfg.CPHA = SPI_CPHA_SECOND;
    Spi_Cfg.CPOL = SPI_CPOL_LO;
    Spi_Cfg.ClockRate = 10000;
    Spi_Cfg.DataOrder = SPI_DATA_MSB_FIRST;
    // Spi_Cfg.Databit = SPI_DATABIT_SIZE;
    Spi_Cfg.Mode = SPI_MASTER_MODE;


    SPI_Init(LPC_SPI, &Spi_Cfg);*/

    // int16_t Tx_Buf = 0;
    // int16_t Rx_Buf = 0x53;

    // SPI_DATA_SETUP_Type xferConfig;
    // xferConfig.tx_data = &Tx_Buf;
    // xferConfig.rx_data = &Rx_Buf;
    // xferConfig.length = 0x10;
    // SPI_ReadWrite(LPC_SPI, &xferConfig, SPI_TRANSFER_POLLING);

    
    //try sending something to the slave (MOSI)

    SYSTICK_InternalInit(100);
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);
    status_code(15);

    while(1) {
        // SPI_SendData(LPC_SPI, data);
        // size_t i = 0;
        // status_code(4);
        // return 1;
        // // while(i < 10000000) {
        // //     i++;
        // // }
        // // status_code(5);
    }

    return 1;
    
}

size_t counter = 0, i=0;
void Systick_Handler(void) {

    //for some reason this isn't getting called??
    SYSTICK_ClearCounterFlag();
    counter++;
    status_code(counter);
    if(counter >= 10) {
        SPI_SendData(LPC_SPI, data);
        counter = 0;
    } 
    // SYSTICK_ClearCounterFlag();
    // counter++;
    // if(counter >= 10) { // in case we miss an iteration or something
    //     counter = 0; // reset counter
    //     if(i < 16) {
    //         status_code(i);
    //         i++;
    //     }
    //     else {
    //         status_code(0);
    //         SYSTICK_IntCmd(DISABLE);
    //         SYSTICK_Cmd(DISABLE);
    //     }
        
    }
}


