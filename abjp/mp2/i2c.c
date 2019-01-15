#include <lpc17xx_pinsel.h>

#include "serial.h"
#include "i2c.h"

void i2c_init(void)
{
	PINSEL_CFG_Type pin_cfg;
	pin_cfg.Pinmode = PINSEL_PINMODE_PULLUP;
	pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	pin_cfg.Funcnum = 3;
	pin_cfg.Portnum = 0;

	pin_cfg.Pinnum = 0;
	PINSEL_ConfigPin(&pin_cfg);

	pin_cfg.Pinnum = 1;
	PINSEL_ConfigPin(&pin_cfg);

	I2C_Init(LPC_I2C1, 100000);
	I2C_Cmd(LPC_I2C1, ENABLE);
}

void i2c_init_transfer_cfg(I2C_M_SETUP_Type *cfg, uint32_t addr)
{
	cfg->sl_addr7bit = addr;
	cfg->tx_data = NULL;
	cfg->tx_length = 0;
	cfg->tx_count = 0;
	cfg->rx_data = NULL;
	cfg->rx_length = 0;
	cfg->rx_count = 0;
	cfg->retransmissions_max = 0;
	cfg->retransmissions_count = 0;
	cfg->status = 0;
	cfg->callback = NULL;
}

Status i2c_send(uint32_t addr, uint8_t *buf, uint32_t len)
{
	I2C_M_SETUP_Type cfg;
	i2c_init_transfer_cfg(&cfg, addr);
	cfg.tx_data = buf;
	cfg.tx_length = len;

	return I2C_MasterTransferData(LPC_I2C1, &cfg, I2C_TRANSFER_POLLING);
}

Status i2c_recv(uint32_t addr, uint8_t *buf, uint32_t len)
{
	I2C_M_SETUP_Type cfg;
	i2c_init_transfer_cfg(&cfg, addr);
	cfg.rx_data = buf;
	cfg.rx_length = len;

	return I2C_MasterTransferData(LPC_I2C1, &cfg, I2C_TRANSFER_POLLING);
}
