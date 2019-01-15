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

    int hi, lo, devices = 0;
    write_usb_serial_blocking("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n\r", 54);
    for(hi=0; hi<0x8; hi++) { // hi is the high nibble of the address, range 0x0-8 (7-bit addresses). see L25
        char axx_buf[4];
        sprintf(axx_buf, "%02x:", hi<<4);
        write_usb_serial_blocking(axx_buf, 4);
        for(lo=0; lo<0x10; lo++) { // likewise, lo is the low nibble of the address, range 0x0-f
            uint8_t addr = (hi<<4) | lo;
            char buf[4];
            if(i2c_write_byte(addr,0) != ERROR) {
                sprintf(buf, " %02x", addr);
                devices++;
            }
            else {
                sprintf(buf, " --");
            }
            write_usb_serial_blocking(buf,4);
        }
        write_usb_serial_blocking("\n\r",3);
    }

    char buf[37];
    sprintf(buf, "\n\r%03d devices connected to i2c bus\n\r", devices);
    write_usb_serial_blocking(buf, 37);
    return 0;
}