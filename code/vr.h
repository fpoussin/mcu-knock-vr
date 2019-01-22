#ifndef VR_H_
#define VR_H_

#include "ch.h"
#include "arm_math.h"

#define VR_SAMPLES 512
#define VR_SAMPLE_SPEED
#define VR_ZERO 2047 // ADC raw value
#define VR_MIN 0
#define VR_MAX 4095

#define VR_DEFAULT_POS_THRESHOLD 2147 // ADC raw value
#define VR_DEFAULT_NEG_THRESHOLD 1947 // ADC raw value
#define VR_DEFAULT_WDG_THRESHOLD 300 // millisecond
#define VR_DEFAULT_MULT_THRESHOLD 4 // last interval multiplier

extern uint16_t vr1_min;
extern uint16_t vr1_max;
extern uint16_t vr2_min;
extern uint16_t vr2_max;
extern uint16_t vr3_min;
extern uint16_t vr3_max;

#endif
