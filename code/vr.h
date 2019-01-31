#ifndef VR_H_
#define VR_H_

#include "ch.h"
#include "arm_math.h"

// NO EDITS
#define VR1_ADC VR1_ADCD.adcm
#define VR2_ADC VR2_ADCD.adcm
#define VR3_ADC VR3_ADCD.adcm
#define VR1_COMP VR1_COMPD.comp
#define VR2_COMP VR2_COMPD.comp
#define VR3_COMP VR3_COMPD.comp
#define VR1_OPAMP VR1_OPAMPD.opamp
#define VR2_OPAMP VR2_OPAMPD.opamp
#define VR3_OPAMP VR3_OPAMPD.opamp
#define VR_DAC VR_DACD.dacm
// NO EDITS

#define VR1_ADCD ADCD1
#define VR2_ADCD ADCD3
#define VR3_ADCD ADCD4

#define VR1_COMPD COMPD1
#define VR2_COMPD COMPD2
#define VR3_COMPD COMPD6

#define VR1_OPAMPD OPAMPD1
#define VR2_OPAMPD OPAMPD3
#define VR3_OPAMPD OPAMPD4

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
