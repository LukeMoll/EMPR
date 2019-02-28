#include <libempr/audioplayback.h>
#include <libempr/status.h>
#include <libempr/digit.h>
#include <stdlib.h>
#include <errno.h>

void fillbuf(uint8_t *buf, size_t len) {
    size_t i;
    for(i=0; i < len; i++) {
        // buf[i] = i & 1 ? 0xFF : 0x00; // square wave at 2xFsmp
        buf[i] = i << 4;
    }
}

int main() {
    i2c_setup_polling();
    digit_clear();
    digit_update();

    const size_t len = 0x2000;
    // uint8_t *buf = malloc(len); // 1024 samples, will take ~1s to play
    uint8_t buf[len];
    if(NULL != buf) {
        // malloc succeded
        status_code(1);
        fillbuf(buf, len);
        status_code(2);
        playback_init(buf, len);
        status_code(3);
        playback_play();
        status_code(4);
        while(1) {};
        status_code(15); // should be unreachable
        return 0;
    }
    else {
        status_code(10);
        int en = errno;
        i2c_setup_polling();
        status_code(11);
        digit_clear();
        digit_setPower(1);
        digit_writeDecValue(en, false);
        digit_update();
        status_code(12);
    }
}