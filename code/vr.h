#ifndef VR_H_
#define VR_H_

#include "ch.h"
#include "arm_math.h"

#define VR1_ADC ADC1
#define VR2_ADC ADC3
#define VR3_ADC ADC4
#define VR1_ADCD ADCD1
#define VR2_ADCD ADCD3
#define VR3_ADCD ADCD4

#define VR1_COMP COMP1
#define VR2_COMP COMP2
#define VR3_COMP COMP6
#define VR1_COMPD COMPD1
#define VR2_COMPD COMPD2
#define VR3_COMPD COMPD6

#define VR1_OPAMP OPAMP1
#define VR2_OPAMP OPAMP3
#define VR3_OPAMP OPAMP4
#define VR1_OPAMPD OPAMPD1
#define VR2_OPAMPD OPAMPD3
#define VR3_OPAMPD OPAMPD4

#define VR_DAC DAC1
#define VR_DACD DACD1

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
