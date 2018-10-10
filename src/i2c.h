#ifndef EMPR_I2C
#define EMPR_I2C
    #include <lpc17xx_pinsel.h>
    void i2c_setup_polling();
    Status i2c_write_byte(uint8_t addr, uint8_t byte);
    Status i2c_write_multiple_bytes(uint8_t addr, uint8_t * bytes, uint8_t length);
#endif