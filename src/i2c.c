#include <lpc17xx_i2c.h>
#include "i2c.h"

#ifndef I2C_CLOCKRATE_HZ
#define I2C_CLOCKRATE_HZ 100000
// 100 kb/s, "standard" i2c frequency
#endif

#define I2CDEV LPC_I2C1
// hardcoded until we parameterise the functions

void i2c_setup_polling() {
    // init I2C pinmode
    PINSEL_CFG_Type PinCfg;
    
    PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
    PinCfg.Funcnum = 3;
    PinCfg.Portnum = 0; // hardcoded to I2C1 (0.0 + 0.1)
    PinCfg.Pinnum = 0; // SDA
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = 1; // SCL
    PINSEL_ConfigPin(&PinCfg);

    I2C_Init(LPC_I2C1, I2C_CLOCKRATE_HZ);
    I2C_Cmd(LPC_I2C1, ENABLE); // Necessary for POLLING operation
}

Status i2c_write_byte(uint8_t addr, uint8_t byte) {
    return i2c_write_multiple_bytes(addr, &byte, 1);
}

Status i2c_write_multiple_bytes(uint8_t addr, uint8_t * bytes, uint8_t length) {
    I2C_M_SETUP_Type transferCfg;
    transferCfg.sl_addr7bit = addr;

    transferCfg.tx_data = bytes;
    transferCfg.tx_length = length;

    transferCfg.rx_data = NULL; // disable recieving
    transferCfg.rx_length = 0;

    transferCfg.retransmissions_max = 3;

    return I2C_MasterTransferData(I2CDEV, &transferCfg, I2C_TRANSFER_POLLING);
}