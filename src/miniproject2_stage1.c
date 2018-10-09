#include <lpc17xx_i2c.h>
#include <lpc17xx_pinsel.h>
#include "status.h"

#define I2C_CLOCKRATE_HZ 1000
#define I2CDEV

int main(void);

int main(void) {
    // init I2C pinmode
    PINSEL_CFG_Type PinCfg;
    
    PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
    PinCfg.Funcnum = 3;
    PinCfg.Portnum = 0;
    PinCfg.Pinnum = 0; // SDA
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = 1; // SCL
    PINSEL_ConfigPin(&PinCfg);


    I2C_Init(LPC_I2C1, I2C_CLOCKRATE_HZ);
    I2C_Cmd(LPC_I2C1, ENABLE); // Necessary for POLLING operation
    // I2C_IntCmd(LPC_I2C1, ENABLE); // only use this for INTERRUPT operation

    I2C_M_SETUP_Type transferCfg;
    transferCfg.sl_addr7bit = 0x69;

    // uint8_t data = 0;
    // transferCfg.tx_data = &data;
    uint8_t data[1] = {0};
    transferCfg.tx_data = data;
    transferCfg.tx_length = 1;

    uint8_t recieve[1] = {0};
    // transferCfg.rx_data = recieve;
    transferCfg.rx_data = NULL;
    transferCfg.rx_length = 0;

    transferCfg.retransmissions_max = 3; // might be useful

    status_code(4);
    Status s = I2C_MasterTransferData(LPC_I2C1, &transferCfg, I2C_TRANSFER_POLLING);
    status_code(1);
    if (s == ERROR){
        status_code(2);
    }
    else {
        status_code(0);
    }

    return 0;
}