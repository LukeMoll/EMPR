#include "i2c.h"
#include <lpc_types.h>
#include "lcd.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "serial.h"

int main(void);

int main(void) {

    i2c_setup_polling();
    lcd_init();

    uint8_t length = 32;
    char * message_dyn = malloc(length); // error check this later
    strncpy(message_dyn, "D0n't [\a]  open\n*dead*  <inside>", length);

    uint8_t * bytes = lcd_a2b_in_place(message_dyn, length);

    lcd_write_bytes(message_dyn, length, 0x80);
    free(message_dyn);
    free(bytes);

    return 0;
}