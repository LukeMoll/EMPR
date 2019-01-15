#include <stdint.h>
#include <lpc_types.h>
#include <lpc17xx_i2c.h>

#ifndef _I2C_H
#define _I2C_H

void i2c_init(void);

Status i2c_send(uint32_t addr, uint8_t *buf, uint32_t len);
Status i2c_recv(uint32_t addr, uint8_t *buf, uint32_t len);

#endif /* _I2C_H */
