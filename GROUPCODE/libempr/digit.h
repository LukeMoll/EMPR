#ifndef EMPR_DIGIT
#define EMPR_DIGIT
    #include <stdint.h>
    #include "i2c.h"
    #include <stdbool.h>
    void digit_setPower(uint8_t pwr);
    void digit_enableSegmentTest(bool enabled);
    void digit_setBits(uint8_t digit, uint8_t mask);
    void digit_clearBits(uint8_t digit, uint8_t mask);
    void digit_setByte(uint8_t digit, uint8_t byte);
    void digit_writeHexByte(uint8_t val, bool high);
    void digit_writeTwoHexBytes(uint16_t val);
    void digit_writeDecValue(int16_t val, bool pad);
    void digit_clear();
    uint8_t digit_digitToAddr(uint8_t digitNo);
    Status digit_update();
#endif