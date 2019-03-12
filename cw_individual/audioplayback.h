#ifndef LIBEMPR_AUDIOPLAYBACK_H
#define LIBEMPR_AUDIOPLAYBACK_H

#include <stdint.h>
#include <stddef.h>

#define PLAYBACK_8KHZ 125
#define PLAYBACK_4KHZ 250
#define PLAYBACK_2KHZ 500
#define PLAYBACK_1KHZ 1000

#define CW_DEMO_DELTA 0x10 // value difference (2^16/2^4 = 2^12)
#define PLAYBACK_COMMAND 0x06 //serial command

void playback_init(uint16_t *buf, size_t len, uint32_t sample_period);
void playback_play();
void playback_pause();
void playback_deinit();

#endif /* LIBEMPR_AUDIOPLAYBACK_H */
