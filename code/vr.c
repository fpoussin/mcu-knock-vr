#include "vr.h"
#include "hal.h"
#include "ipc.h"
#include "settings.h"
#include "median.h"
#include "timers.h"

#define VALID_MSK 0x03

#define TR1_EN palSetLineMode(LINE_TR1_OUT, 8)
#define TR2_EN palSetLineMode(LINE_TR2_OUT, 8)
#define TR3_EN palSetLineMode(LINE_TR3_OUT, 8)

#define TR1_DIS palSetLineMode(LINE_TR1_OUT, 0)
#define TR2_DIS palSetLineMode(LINE_TR2_OUT, 0)
#define TR3_DIS palSetLineMode(LINE_TR3_OUT, 0)

#define TR1_UP palSetLine(LINE_TR1_OUT)
#define TR2_UP palSetLine(LINE_TR2_OUT)
#define TR3_UP palSetLine(LINE_TR3_OUT)

#define TR1_DN palClearLine(LINE_TR1_OUT)
#define TR2_DN palClearLine(LINE_TR2_OUT)
#define TR3_DN palClearLine(LINE_TR3_OUT)


typedef struct
{
  uint16_t high;
  uint16_t low;
} high_low_t;

typedef struct
{
  uint8_t pad:6;
  bool time:1;
  bool peak:1;
} valid_t;

typedef struct
{
  high_low_t threshold;
  high_low_t peak;
  uint16_t min_time;
  union {
    valid_t valid;
    uint8_t valid_msk;
  };
  uint8_t pad;
} vr_t;

static vr_t vr1, vr2, vr3;

/*
 * Peripherals
 * VR1: ADC1 + COMP 1
 * VR2: ADC2 + COMP 2
 * VR3: ADC3 + COMP 6
 * DAC1 (bias)
 */

/*
 * Support functions
 */

#define pwmCounter(pwm) pwm.tim->CNT

inline static void pwmDisable(PWMDriver *pwm)
{
  pwm->tim->CR1 &= ~TIM_CR1_CEN;
}

inline static void pwmEnable(PWMDriver *pwm)
{
  pwm->tim->CNT = 0;
  pwm->tim->CR1 |= TIM_CR1_CEN;
}

inline static void pwmRestart(PWMDriver *pwm)
{
  pwmDisable(pwm);
  pwmEnable(pwm);
}

inline static void pwmSetFrequency(PWMDriver *pwm, uint32_t freq)
{
  pwmDisable(pwm);
  pwm->tim->PSC  = (pwm->clock / freq) - 1; // CLK is 36 (TIM3-4) or 72Mhz (TIM1/8)
}

inline static void pwmSetReload(PWMDriver *pwm, uint32_t value)
{
  pwmDisable(pwm);
  pwm->tim->ARR = value;
  pwmEnable(pwm);
}

/*
 * CALLBACKS
 */

void vr1IrqHandler(void)
{

}

void vr2IrqHandler(void)
{

}

void vr3IrqHandler(void)
{

}

/*
 * Watchdog timeout callback
 * This happens when no output was generated before the timer completes
 * We reset the thresholds to default values.
 * Timer should have stopped since we are in one pulse mode.
 */
static void pwm_ovfl_cb(PWMDriver *pwmp)
{
  if (pwmp == &PWMD3)
  {
    vr1.threshold.low = VR_DEFAULT_NEG_THRESHOLD;
    vr1.threshold.high = VR_DEFAULT_POS_THRESHOLD;
    vr1.valid_msk = 0;
  }
  else if (pwmp == &PWMD4)
  {
    vr2.threshold.low = VR_DEFAULT_NEG_THRESHOLD;
    vr2.threshold.high = VR_DEFAULT_POS_THRESHOLD;
    vr2.valid_msk = 0;
  }
  else if (pwmp == &PWMD8)
  {
    vr3.threshold.low = VR_DEFAULT_NEG_THRESHOLD;
    vr3.threshold.high = VR_DEFAULT_POS_THRESHOLD;
    vr3.valid_msk = 0;
  }
}

/*
 * VR validator timer
 * We are now far enough in the cycle to enable the output
 */
static void pwm_oc1_cb(PWMDriver *pwmp)
{
  if (pwmp == &PWMD3)
  {
    vr1.valid.time = true;
  }
  else if (pwmp == &PWMD4)
  {
    vr2.valid.time = true;
  }
  else if (pwmp == &PWMD8)
  {
    vr3.valid.time = true;
  }
}

static void comp_cb(COMPDriver *comp)
{
  /* Check if output is high (rising) */
  if (comp->reg->CSR & COMP_CSR_COMPxOUT)
  {
    /* Set new thresholds to 80% of previous peaks, reset validation */
    if (comp == &COMPD1)
    {
      if (vr1.valid_msk & VALID_MSK)
      {
        // Get last interval, set timeout;
        uint32_t cnt = pwmCounter(PWMD3);
        pwmSetReload(&PWMD3, cnt * VR_DEFAULT_MULT_THRESHOLD);

        vr1.threshold.low = (uint16_t)((float)vr1.peak.low * 1.2f);
        vr1.threshold.high = (uint16_t)((float)vr1.peak.high * 0.8f);
        vr1.peak.low = VR_ZERO;
        vr1.peak.high = VR_ZERO;
        vr1.valid_msk = 0;
      }
    }
    else if (comp == &COMPD2)
    {
      if (vr2.valid_msk & VALID_MSK)
      {
        // Get last interval, set timeout;
        uint32_t cnt = pwmCounter(PWMD4);
        pwmSetReload(&PWMD4, cnt * VR_DEFAULT_MULT_THRESHOLD);

        vr2.threshold.low = (uint16_t)((float)vr2.peak.low * 1.2f);
        vr2.threshold.high = (uint16_t)((float)vr2.peak.high * 0.8f);
        vr2.valid_msk = 0;
      }
    }
    else if (comp == &COMPD6)
    {
      if (vr3.valid_msk & VALID_MSK)
      {
        // Get last interval, set timeout;
        uint32_t cnt = pwmCounter(PWMD8);
        pwmSetReload(&PWMD8, cnt * VR_DEFAULT_MULT_THRESHOLD);

        vr3.threshold.low = (uint16_t)((float)vr3.peak.low * 1.2f);
        vr3.threshold.high = (uint16_t)((float)vr3.peak.high * 0.8f);
        vr3.valid_msk = 0;
      }
    }

  }
  else // LOW
  {

  }
}

/* Every 1024 samples at 972.972Khz each, triggers at around 1KHz */
static void adcCallback(ADCDriver *adcp, adcsample_t *buffer, size_t n)
{
  // Check for min/max in the ADC thread
  chSysLockFromISR();
  if (adcp == &ADCD1)
  {
    allocSendSamplesI(&vr1_mb, (void*)buffer, n);
  }
  else if (adcp == &ADCD2)
  {
    allocSendSamplesI(&vr2_mb, (void*)buffer, n);
  }
  else if (adcp == &ADCD3)
  {
    allocSendSamplesI(&vr2_mb, (void*)buffer, n);
  }
  chSysUnlockFromISR();
}

/*
 * Peripheral configs
 */

/*
 * PWM3/4/8 configuration.
 * Watchdog (overflow) and validator (channel 1) for VR peaks
 */
static const PWMConfig pwm_cfg = {
  30000,                                    /* 30kHz PWM clock frequency.   */
  0xFFFF,                                   /* Initial PWM period maxed out.       */
  pwm_ovfl_cb,
  {
   {PWM_OUTPUT_DISABLED, pwm_oc1_cb},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL}
  },
  0,
  0
};

static const DACConfig dac_conf = {
  .init         = 2047U,
  .datamode     = DAC_DHRM_12BIT_RIGHT,
  .cr           = 0
};

// VR1
static const COMPConfig comp1_conf = {
  COMP_OUTPUT_NORMAL,
  COMP_IRQ_BOTH,
  comp_cb,
  STM32_COMP_InvertingInput_DAC1OUT1 |
  STM32_COMP_BlankingSrce_None |
  STM32_COMP_NonInvertingInput_IO1 | // PA1
  STM32_COMP_Hysteresis_Medium | // 15mV
  STM32_COMP_OutputLevel_High |
  STM32_COMP_Mode_HighSpeed // CSR
};

// VR2
static const COMPConfig comp2_conf = {
  COMP_OUTPUT_NORMAL,
  COMP_IRQ_BOTH,
  comp_cb,
  STM32_COMP_InvertingInput_DAC1OUT1 |
  STM32_COMP_BlankingSrce_None |
  STM32_COMP_NonInvertingInput_IO1 | // PA7
  STM32_COMP_Hysteresis_Medium | // 15mV
  STM32_COMP_OutputLevel_High |
  STM32_COMP_Mode_HighSpeed // CSR
};

// VR3
static const COMPConfig comp6_conf = {
  COMP_OUTPUT_NORMAL,
  COMP_IRQ_BOTH,
  comp_cb,
  STM32_COMP_InvertingInput_DAC1OUT1 |
  STM32_COMP_BlankingSrce_None |
  STM32_COMP_NonInvertingInput_IO2 | // PB11
  STM32_COMP_Hysteresis_Medium | // 15mV
  STM32_COMP_OutputLevel_High |
  STM32_COMP_Mode_HighSpeed // CSR
};

// VR1
static const OPAMPConfig opamp1_conf = {
  STM32_OPAMP_NonInvertingInput_IO3 | // INP connectd to PA3
  STM32_OPAMP_InvertingInput_Vout // INM connected to vout (follower)
};

// VR2
static const OPAMPConfig opamp2_conf = {
  STM32_OPAMP_NonInvertingInput_IO2 | // INP connectd to PB14
  STM32_OPAMP_InvertingInput_Vout // INM connected to vout (follower)
};

// VR3
static const OPAMPConfig opamp3_conf = {
  STM32_OPAMP_NonInvertingInput_IO3 | // INP connectd to PB0
  STM32_OPAMP_InvertingInput_Vout // INM connected to vout (follower)
};

/* ADC1 Clk is 72Mhz/1 72Mhz  */
static const ADCConversionGroup vr1grpcfg = {
  TRUE,
  1,
  adcCallback,
  NULL,
  ADC_CFGR_CONT,    /* CFGR - Continous */
  ADC_TR(0, 4095),                  /* TR1 - Watchdog  */
  {                                 /* SMPR[2] */
    ADC_SMPR1_SMP_AN1(ADC_SMPR_SMP_61P5),  /* Sampling rate = 72000000/(61.5+12.5) = 972.972Khz  */
    0,
  },
  {                                 /* SQR[4]  */
    ADC_SQR1_SQ1_N(ADC_CHANNEL_IN3),
    0,
    0,
    0
  }
};

/* ADC2 Clk is 72Mhz/1 72Mhz  */
static const ADCConversionGroup vr2grpcfg = {
  TRUE,
  1,
  adcCallback,
  NULL,
  ADC_CFGR_CONT,    /* CFGR - Continous */
  ADC_TR(0, 4095),                  /* TR1 - Watchdog  */
  {                                 /* SMPR[2] */
    ADC_SMPR1_SMP_AN1(ADC_SMPR_SMP_61P5),  /* Sampling rate = 72000000/(61.5+12.5) = 972.972Khz  */
    0,
  },
  {                                 /* SQR[4]  */
    ADC_SQR1_SQ1_N(ADC_CHANNEL_IN1),
    0,
    0,
    0
  }
};


/* ADC3 Clk is 72Mhz/1 72Mhz  */
static const ADCConversionGroup vr3grpcfg = {
  TRUE,
  1,
  adcCallback,
  NULL,
  ADC_CFGR_CONT,    /* CFGR - Continous */
  ADC_TR(0, 4095),                  /* TR1 - Watchdog  */
  {                                 /* SMPR[2] */
    ADC_SMPR1_SMP_AN1(ADC_SMPR_SMP_61P5),  /* Sampling rate = 72000000/(61.5+12.5) = 972.972Khz  */
    0,
  },
  {                                 /* SQR[4]  */
    ADC_SQR1_SQ1_N(ADC_CHANNEL_IN3),
    0,
    0,
    0
  }
};

static pair_t vr1_pair[VR_SAMPLES];
static adcsample_t vr1_samples[VR_SAMPLES];
static THD_WORKING_AREA(waThreadVR1, 128);
CCM_FUNC static THD_FUNCTION(ThreadVR1, arg)
{
  (void)arg;
  chRegSetThreadName("VR1");

  adcsample_t * adc_data_ptr;
  size_t adc_data_size;
  median_t median;

  PWMD3.tim->CR1 |= TIM_CR1_OPM; // One pulse

  median_init(&median, 0, vr1_pair, VR_SAMPLES);

  /* ADC 1 Ch3 Offset. -2048 */
  ADC1->OFR1 = ADC_OFR1_OFFSET1_EN | ADC_OFR1_OFFSET1_CH_0 | ADC_OFR1_OFFSET1_CH_1 | (2048 & 0xFFF);
  adcStartConversion(&ADCD1, &vr1grpcfg, vr1_samples, VR_SAMPLES);

  while (TRUE)
  {
    recvFreeSamples(&vr1_mb, (void*)&adc_data_ptr, &adc_data_size, TIME_INFINITE);

    /* Filtering and finding min/max */
    uint16_t val, min, max = 0;
    for (uint16_t i = 0; i < VR_SAMPLES; i++)
    {
      val = median_filter(&median, adc_data_ptr[i]);
      if (val > max) max = val;
      if (val < min) min = val;
    }
    if (min < vr1.peak.low)
        vr1.peak.low = min;
    if (max > vr1.peak.high)
        vr1.peak.high = max;

    if (!vr1.valid.peak)
    {
      vr1.valid.peak = \
             vr1.peak.low <= vr1.threshold.low
          && vr1.peak.high >= vr1.threshold.high;
    }
  }
}

static pair_t vr2_pair[VR_SAMPLES];
static adcsample_t vr2_samples[VR_SAMPLES];
static THD_WORKING_AREA(waThreadVR2, 128);
CCM_FUNC static THD_FUNCTION(ThreadVR2, arg)
{
  (void)arg;
  chRegSetThreadName("VR2");

  adcsample_t * adc_data_ptr;
  size_t adc_data_size;
  median_t median;

  PWMD4.tim->CR1 |= TIM_CR1_OPM; // One pulse

  median_init(&median, 0, vr2_pair, VR_SAMPLES);

  /* ADC 2 Ch1 Offset. -2048 */
  ADC2->OFR1 = ADC_OFR1_OFFSET1_EN | ADC_OFR1_OFFSET1_CH_0 | (2048 & 0xFFF);
  adcStartConversion(&ADCD2, &vr2grpcfg, vr2_samples, VR_SAMPLES);

  while (TRUE)
  {
    recvFreeSamples(&vr2_mb, (void*)&adc_data_ptr, &adc_data_size, TIME_INFINITE);

    /* Filtering and finding min/max */
    uint16_t val, min, max = 0;
    for (uint16_t i = 0; i < VR_SAMPLES; i++)
    {
      val = median_filter(&median, adc_data_ptr[i]);
      if (val > max) max = val;
      if (val < min) min = val;
    }
    if (min < vr2.peak.low)
        vr2.peak.low = min;
    if (max > vr2.peak.high)
        vr2.peak.high = max;

    if (!vr2.valid.peak)
    {
      vr2.valid.peak = \
             vr2.peak.low <= vr2.threshold.low
          && vr2.peak.high >= vr2.threshold.high;
    }
  }

}

static pair_t vr3_pair[VR_SAMPLES];
static adcsample_t vr3_samples[VR_SAMPLES];
static THD_WORKING_AREA(waThreadVR3, 128);
CCM_FUNC static THD_FUNCTION(ThreadVR3, arg)
{
  (void)arg;
  chRegSetThreadName("VR3");

  adcsample_t * adc_data_ptr;
  size_t adc_data_size;
  median_t median;

  PWMD8.tim->CR1 |= TIM_CR1_OPM; // One pulse

  median_init(&median, 0, vr3_pair, VR_SAMPLES);

  /* ADC 3 Ch3 Offset. -2048 */
  ADC1->OFR1 = ADC_OFR1_OFFSET1_EN | ADC_OFR1_OFFSET1_CH_0 | ADC_OFR1_OFFSET1_CH_1 | (2048 & 0xFFF);
  adcStartConversion(&ADCD1, &vr3grpcfg, vr3_samples, VR_SAMPLES);

  while (TRUE)
  {
    recvFreeSamples(&vr3_mb, (void*)&adc_data_ptr, &adc_data_size, TIME_INFINITE);

    /* Filtering and finding min/max */
    uint16_t val, min, max = 0;
    for (uint16_t i = 0; i < VR_SAMPLES; i++)
    {
      val = median_filter(&median, adc_data_ptr[i]);
      if (val > max) max = val;
      if (val < min) min = val;
    }
    if (min < vr3.peak.low)
        vr3.peak.low = min;
    if (max > vr3.peak.high)
        vr3.peak.high = max;

    if (!vr3.valid.peak)
    {
      vr3.valid.peak = \
             vr3.peak.low <= vr3.threshold.low
          && vr3.peak.high >= vr3.threshold.high;
    }
  }

}

void createVrThreads(void)
{
  opampStart(&OPAMPD1, &opamp1_conf);
  opampStart(&OPAMPD2, &opamp2_conf);
  opampStart(&OPAMPD3, &opamp3_conf);
  adcStart(&ADCD1, NULL);
  adcStart(&ADCD2, NULL);
  adcStart(&ADCD3, NULL);
  dacStart(&DACD1, &dac_conf);
  compStart(&COMPD1, &comp1_conf);
  compStart(&COMPD2, &comp2_conf);
  compStart(&COMPD6, &comp6_conf);
  pwmStart(&PWMD3, &pwm_cfg);
  pwmStart(&PWMD4, &pwm_cfg);
  pwmStart(&PWMD8, &pwm_cfg);

  dacPutChannelX(&DACD1, 0, 2048); // This sets the biasing for our sensors and comparators.
  compEnable(&COMPD1);
  compEnable(&COMPD2);
  compEnable(&COMPD6);
  opampEnable(&OPAMPD1);
  opampEnable(&OPAMPD2);
  opampEnable(&OPAMPD3);

  chThdCreateStatic(waThreadVR1, sizeof(waThreadVR1), NORMALPRIO, ThreadVR1, NULL);
  chThdCreateStatic(waThreadVR2, sizeof(waThreadVR2), NORMALPRIO, ThreadVR2, NULL);
  chThdCreateStatic(waThreadVR3, sizeof(waThreadVR3), NORMALPRIO, ThreadVR3, NULL);
}
