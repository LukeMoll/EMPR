#ifndef LIBEMPR_AUDIOPLAYBACK_H
#define LIBEMPR_AUDIOPLAYBACK_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
void recording_init(uint16_t *buf, size_t len, uint32_t sample_period);
void recording_start();
bool isrecording();

#endif