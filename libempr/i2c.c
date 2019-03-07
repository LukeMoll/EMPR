#include <lpc17xx_i2c.h>

#include "serial.h"
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

Status i2c_read_byte(uint8_t addr, uint8_t * dest) {
    // TODO: this hangs - why?
    I2C_M_SETUP_Type transferCfg;
    transferCfg.sl_addr7bit = addr;

    transferCfg.tx_data = NULL;
    transferCfg.tx_length = 0;

    transferCfg.rx_data = dest; // rx_data is pointer to destination, as is dest
    transferCfg.rx_length = 1; // recieve 1 byte

    transferCfg.retransmissions_max = 3;

    return I2C_MasterTransferData(I2CDEV, &transferCfg, I2C_TRANSFER_POLLING);
}

void i2c_detect() {
    int hi, lo, devices = 0;
    serial_puts("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n\r");
    for(hi=0; hi<0x8; hi++) { // hi is the high nibble of the address, range 0x0-8 (7-bit addresses). see L25
        serial_printf("%02x:", hi<<4);
        for(lo=0; lo<0x10; lo++) { // likewise, lo is the low nibble of the address, range 0x0-f
            uint8_t addr = (hi<<4) | lo;
            if(i2c_write_byte(addr,0) != ERROR) {
                serial_printf(" %02x", addr);
                devices++;
            }
            else {
                serial_puts(" --");
            }
        }
        serial_puts("\n\r");
    }

    serial_printf("\n\r%03d devices connected to i2c bus\n\r", devices);
}
