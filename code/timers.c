#include "timers.h"

static TIM_TypeDef * const timersp[3] = {TIM15, TIM16, TIM17};

OSAL_IRQ_HANDLER(STM32_TIM15_HANDLER)
{
  OSAL_IRQ_PROLOGUE();
  vr1IrqHandler();
  OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(STM32_TIM16_HANDLER)
{
  OSAL_IRQ_PROLOGUE();
  vr2IrqHandler();
  OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(STM32_TIM17_HANDLER)
{
  OSAL_IRQ_PROLOGUE();
  vr3IrqHandler();
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
    timersp[i]->CCMR1 = STM32_TIM_CCMR1_OC1M(6) | STM32_TIM_CCMR1_OC1PE;
    timersp[i]->CCMR1 = 0;
    timersp[i]->CCR1 = 0;
    timersp[i]->CNT = 0;
    timersp[i]->PSC = 30000;
    timersp[i]->ARR = 0xFFFF;
    timersp[i]->EGR = STM32_TIM_EGR_UG;
    timersp[i]->SR = 0;
    timersp[i]->CR1 = STM32_TIM_CR1_ARPE | STM32_TIM_CR1_URS | STM32_TIM_CR1_CEN | STM32_TIM_CR1_OPM;
  }

  nvicEnableVector(STM32_TIM15_NUMBER, 7);
  nvicEnableVector(STM32_TIM16_NUMBER, 7);
  nvicEnableVector(STM32_TIM17_NUMBER, 7);

}
