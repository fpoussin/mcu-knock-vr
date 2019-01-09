#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "ch.h"

typedef struct {
    uint16_t knock_freq;
    uint16_t knock_ratio;
    uint16_t vr_modes;

} settings_t;

extern settings_t settings;

#endif
