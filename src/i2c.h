#ifndef EMPR_I2C
#define EMPR_I2C
    void i2c_setup_polling();
    Status i2c_write_byte(uint8_t addr, uint8_t byte);
#endif