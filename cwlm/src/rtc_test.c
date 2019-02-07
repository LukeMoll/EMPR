#include <lpc17xx_rtc.h>
#include "serial.h"
#include "status.h"
#include "i2c.h"
#include "lcd_buf.h"

int main(void) {

    i2c_setup_polling();
    lcd_init();
    lcd_buf_flush();
    serial_init();
    
    char buf[3];

    status_code(1);
    uint8_t month = 0;
    while(!month) {
        month = read_usb_serial_blocking(buf, 2);
    }
    //can send stuff via screen, but when I try to send directly (or echo), I get "Device or ressource busy"
    lcd_buf_write_string(buf, 2, 0);
    lcd_buf_update();
    status_code(3);
    return(1);

}