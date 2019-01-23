#ifndef _VRTIMERS_H_
#define _VRTIMERS_H_

#include "hal.h"

#define VR1_TIM TIM15
#define VR2_TIM TIM16
#define VR3_TIM TIM17

#define VR1_TIM_HANDLER STM32_TIM15_HANDLER
#define VR2_TIM_HANDLER STM32_TIM16_HANDLER
#define VR3_TIM_HANDLER STM32_TIM17_HANDLER

void setupTimers(void);


#endif
