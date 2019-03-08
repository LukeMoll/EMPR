#include <stdbool.h>
#include <lpc17xx_rtc.h>
#include <libempr/serial.h>

void main(void) {
    uint8_t buf[5] = {0, 0, 0, 0, 0};

	serial_send(0x02, NULL, 0);
	serial_recv(0x02, buf, 5);

    RTC_Init(LPC_RTC);
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_YEAR, 2019);
    RTC_SetTime(LPC_RTC, RTC_TIMETYPE_MONTH, buf[0]);
    RTC_SetTime(LPC_RTC, RTC_TIMETYPE_DAYOFMONTH, buf[1]);
    RTC_SetTime(LPC_RTC, RTC_TIMETYPE_HOUR, buf[2]);
    RTC_SetTime(LPC_RTC, RTC_TIMETYPE_MINUTE, buf[3]);
    RTC_SetTime(LPC_RTC, RTC_TIMETYPE_SECOND, buf[4]);
    RTC_Cmd(LPC_RTC, ENABLE);

	serial_puts("hello, world!\n");

	while (1) {}
}
