#ifndef LIBEMPR_AUDIOPLAYBACK_H
#define LIBEMPR_AUDIOPLAYBACK_H

#include <stdint.h>
#include <stddef.h>
void recording_init(uint16_t *buf, size_t len, uint32_t sample_period);
void recording_start();
void recording_deinit();
bool isrecording();

#endif