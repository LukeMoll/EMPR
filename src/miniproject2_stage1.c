#include <lpc17xx_i2c.h>
#include <lpc17xx_pinsel.h>

#include "i2c.h"
#include "status.h"
#include "serial.h"
#include <stdio.h>

#define I2C_CLOCKRATE_HZ 1000
#define I2CDEV

int main(void);

int main(void) {
    i2c_setup_polling();
    serial_init();

    int i, devices = 0;
    for(i=0x00; i < 0x80; i++) {
        if(i2c_write_byte(i,0) != ERROR) {
            char buf[22];
            sprintf(buf, "Device found at 0x%02x\n\r", i);
            write_usb_serial_blocking(buf, 22);
            devices++;
        }
    }

    char buf[35];
    sprintf(buf, "%03d devices connected to i2c bus\n\r", devices);
    write_usb_serial_blocking(buf, 35);
    return 0;
}