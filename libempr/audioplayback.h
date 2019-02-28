#ifndef LIBEMPR_AUDIOPLAYBACK_H
#define LIBEMPR_AUDIOPLAYBACK_H

#include <stdint.h>
#include <stddef.h>
void playback_init(uint8_t *buf, size_t len);
void playback_play();
void playback_pause();
void playback_deinit();

#endif /* LIBEMPR_AUDIOPLAYBACK_H */