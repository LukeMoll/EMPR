#include <lpc17xx_systick.h>
#include <lpc17xx_spi.h>
#include <lpc17xx_pinsel.h>
#include <lpc_types.h>

#include "pins.h"
#include "status.h"

#define PORT 0
#define FUNCNUM 3
#define MOSI_PIN 18
#define MISO_PIN 17
#define SCK_PIN 15

void SysTick_Handler(void);

int main(void) {

    SYSTICK_InternalInit(100);
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);

    PINSEL_CFG_Type PinCfg_Mosi;
    PinCfg_Mosi.Funcnum = FUNCNUM;
    PinCfg_Mosi.OpenDrain = PINSEL_PINMODE_NORMAL;
    PinCfg_Mosi.Pinmode = PINSEL_PINMODE_PULLUP;
    PinCfg_Mosi.Portnum = PORT;
    PinCfg_Mosi.Pinnum = MOSI_PIN;

    PINSEL_ConfigPin(&PinCfg_Mosi);

    PINSEL_CFG_Type PinCfg_Miso;
    PinCfg_Miso.Funcnum = FUNCNUM;
    PinCfg_Miso.OpenDrain = PINSEL_PINMODE_NORMAL;
    PinCfg_Miso.Pinmode = PINSEL_PINMODE_PULLUP;
    PinCfg_Miso.Portnum = PORT;
    PinCfg_Miso.Pinnum = MISO_PIN;

    PINSEL_CFG_Type PinCfg_Sck;
    PinCfg_Sck.Funcnum = FUNCNUM;
    PinCfg_Sck.OpenDrain = PINSEL_PINMODE_NORMAL;
    PinCfg_Sck.Pinmode = PINSEL_PINMODE_PULLUP;
    PinCfg_Sck.Portnum = PORT;
    PinCfg_Sck.Pinnum = SCK_PIN;

    PINSEL_ConfigPin(&PinCfg_Miso);

    SPI_CFG_Type Spi_Cfg;
    Spi_Cfg.CPHA = SPI_CPHA_SECOND;
    Spi_Cfg.CPOL = SPI_CPOL_LO;
    Spi_Cfg.ClockRate = 1000000;
    Spi_Cfg.DataOrder = SPI_DATA_MSB_FIRST;
    Spi_Cfg.Mode = SPI_MASTER_MODE;

    SPI_Init(LPC_SPI, &Spi_Cfg);

    while(1);
    return 1;
}

uint8_t counter = 0;
uint8_t i = 0;

void SysTick_Handler(void) {
    SYSTICK_ClearCounterFlag();
    counter++;
    if(counter >= 10) { // in case we miss an iteration or something
        counter = 0; // reset counter
        if(i < 16) {
            status_code(i);
            i++;
            SPI_SendData(LPC_SPI, 0x53);
        }
        else {
            i = 0;
            }        
        }
        
    
}
