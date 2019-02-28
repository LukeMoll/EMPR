#include <lpc17xx_rtc.h>
#include "serial.h"
#include "status.h"
#include "i2c.h"
#include "lcd_buf.h"

int main(void) {

    status_code(1);
    serial_init();
    i2c_setup_polling();
    serial_init();
    lcd_init();
    status_code(3);
    uint8_t buf[5] = {0, 0, 0, 0, 0};
    uint8_t size = 0;

    size = read_usb_serial_blocking(buf, 5);
    status_code(2);
    RTC_Init(LPC_RTC);
    status_code(6);
    RTC_SetTime(LPC_RTC, RTC_TIMETYPE_MONTH, buf[0]);
    RTC_SetTime(LPC_RTC, RTC_TIMETYPE_DAYOFMONTH, buf[1]);
    RTC_SetTime(LPC_RTC, RTC_TIMETYPE_HOUR, buf[2]);
    RTC_SetTime(LPC_RTC, RTC_TIMETYPE_MINUTE, buf[3]);
    RTC_SetTime(LPC_RTC, RTC_TIMETYPE_SECOND, buf[4]);
    status_code(3);
    RTC_Cmd(LPC_RTC, ENABLE);
    status_code(4);

    lcd_buf_clear_screen();
    lcd_buf_write_string("test", 4, 0);
    lcd_buf_update();
    status_code(5);
    uint32_t month = RTC_GetTime(LPC_RTC, RTC_TIMETYPE_MONTH);
    status_code(6);
    uint32_t day = RTC_GetTime(LPC_RTC, RTC_TIMETYPE_DAYOFMONTH);
    status_code(7);
    uint32_t hour = RTC_GetTime(LPC_RTC, RTC_TIMETYPE_HOUR);
    status_code(8);
    uint32_t minute = RTC_GetTime(LPC_RTC, RTC_TIMETYPE_MINUTE);
    status_code(9);
    uint32_t seconds = RTC_GetTime(LPC_RTC, RTC_TIMETYPE_SECOND);
    status_code(10);
    status_code(minute); //it is definitely sending the right thing
    char buff[17];
    snprintf(buff, 16, "%02u/%02u - %02u:%02u:%02u", month, day, hour, minute, seconds); //Can change this later, for now just wanted to get RTC working
    lcd_buf_write_string(buff, 16, 16);
    lcd_buf_update();
    return(1);

}