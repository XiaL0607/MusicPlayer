#ifndef _ALSA_H
#define _ALSA_H

#include <stdint.h>

//#define PLAY_DEVICE  "hw:0,0"
#define PLAY_DEVICE  "default"

int init_alsa_playback();
int32_t play_callback(const float *samples, int32_t n);

#endif
