#include "vrtimers.h"

static TIM_TypeDef * const timersp[3] = {VR1_TIM, VR2_TIM, VR3_TIM};

extern void VR1_OVERFLOW_HANDLER(void);
extern void VR1_COMPARE_HANDLER(void);

extern void VR2_OVERFLOW_HANDLER(void);
extern void VR2_COMPARE_HANDLER(void);

extern void VR3_OVERFLOW_HANDLER(void);
extern void VR3_COMPARE_HANDLER(void);

#define VR1_TIM_HANDLER STM32_TIM15_HANDLER
#define VR2_TIM_HANDLER STM32_TIM16_HANDLER
#define VR3_TIM_HANDLER STM32_TIM17_HANDLER

OSAL_IRQ_HANDLER(VR1_TIM_HANDLER)
{
  OSAL_IRQ_PROLOGUE();

  uint32_t sr = VR1_TIM->SR;
  sr &= VR1_TIM->DIER & STM32_TIM_DIER_IRQ_MASK;
  VR1_TIM->SR = ~sr;
  if ((sr & STM32_TIM_SR_CC1IF) != 0)
    VR1_OVERFLOW_HANDLER();
  if ((sr & STM32_TIM_SR_UIF) != 0)
    VR1_COMPARE_HANDLER();

  OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(VR2_TIM_HANDLER)
{
  OSAL_IRQ_PROLOGUE();

  uint32_t sr = VR2_TIM->SR;
  sr &= VR2_TIM->DIER & STM32_TIM_DIER_IRQ_MASK;
  VR2_TIM->SR = ~sr;
  if ((sr & STM32_TIM_SR_CC1IF) != 0)
    VR2_OVERFLOW_HANDLER();
  if ((sr & STM32_TIM_SR_UIF) != 0)
    VR2_COMPARE_HANDLER();

  OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(VR3_TIM_HANDLER)
{
  OSAL_IRQ_PROLOGUE();

  uint32_t sr = VR3_TIM->SR;
  sr &= VR3_TIM->DIER & STM32_TIM_DIER_IRQ_MASK;
  VR3_TIM->SR = ~sr;
  if ((sr & STM32_TIM_SR_CC1IF) != 0)
    VR3_OVERFLOW_HANDLER();
  if ((sr & STM32_TIM_SR_UIF) != 0)
    VR3_COMPARE_HANDLER();

  OSAL_IRQ_EPILOGUE();
}

void setupTimers()
{
  rccEnableTIM15();
  rccEnableTIM16();
  rccEnableTIM17();

  rccResetTIM15();
  rccResetTIM16();
  rccResetTIM17();

  for (int i = 0; i < 3; i++)
  {
    TIM_TypeDef * tp = timersp[i];
    tp->CCMR1 = STM32_TIM_CCMR1_OC1M(1) | STM32_TIM_CCMR1_OC1PE; // Set channel 1 to active level on match. Enable OC preload (CC refreshed after update event)
    tp->CCMR2 = 0; // Ch 3-4 unused
    tp->CCER = 0; // All outputs disabled
    tp->CCR1 = 0; // Channels disabled
    tp->CCR2 = 0; // Channels disabled
    tp->CNT = 0; // Reset counter
    tp->PSC = STM32_TIMCLK2 / 30000;  /* 30kHz PWM clock frequency.   */
    tp->ARR = 0xFFFF; /* Initial period maxed out.    */
    tp->EGR = STM32_TIM_EGR_UG | STM32_TIM_EGR_CC1G; // Enable events for CC1 and Update.
    tp->SR = 0; // Clear status reg
    tp->SMCR = 0; // No slaves
    tp->DCR = 0; // No DMA
    tp->DMAR = 0; // No DMA
    tp->DIER = STM32_TIM_DIER_CC1IE | STM32_TIM_DIER_UIE; // Enable interrupts for CC1 and Update (overflow)
    tp->CR1 = STM32_TIM_CR1_ARPE | STM32_TIM_CR1_URS | STM32_TIM_CR1_CEN | STM32_TIM_CR1_OPM; // Enable, auto reload preload, one pulse
  }

  nvicEnableVector(STM32_TIM15_NUMBER, 7);
  nvicEnableVector(STM32_TIM16_NUMBER, 7);
  nvicEnableVector(STM32_TIM17_NUMBER, 7);
}
