#ifndef VR_H_
#define VR_H_

#include "ch.h"
#include "arm_math.h"

#define VR_SAMPLES 512
#define VR_SAMPLE_SPEED
#define VR_ZERO 1650
#define VR_MIN 0
#define VR_MAX 3300

#define VR_DEFAULT_POS_THRESHOLD 1750
#define VR_DEFAULT_NEG_THRESHOLD 1550
#define VR_DEFAULT_THRESHOLD_WATCHDOG 300
#define VR_DEFAULT_THRESHOLD_MULT 4

extern uint16_t vr1_min;
extern uint16_t vr1_max;
extern uint16_t vr2_min;
extern uint16_t vr2_max;
extern uint16_t vr3_min;
extern uint16_t vr3_max;

#endif
