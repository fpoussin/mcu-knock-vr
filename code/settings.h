#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "ch.h"

#define SETTING_KNOCK_ON  (1 << 0)
#define SETTING_KNOCK_INV (1 << 1)

#define SETTING_VR1_ON  (1 << 0)
#define SETTING_VR2_ON  (1 << 1)
#define SETTING_VR3_ON  (1 << 2)
#define SETTING_VR1_INV (1 << 3)
#define SETTING_VR2_INV (1 << 4)
#define SETTING_VR3_INV (1 << 5)

#define SETTING_VR_ON_MSK  0x06

typedef struct {
    uint16_t knock_modes;
    uint16_t knock_freq;
    uint16_t knock_ratio;
    uint16_t vr_modes;
    uint16_t vr_watchdog;
    uint16_t vr_threshold;
} settings_t;

extern settings_t settings;

#endif
