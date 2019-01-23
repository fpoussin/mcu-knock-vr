#include "vr.h"
#include "hal.h"
#include "ipc.h"
#include "settings.h"
#include "median.h"
#include "vrtimers.h"

#define VALID_MSK 0x03

#define tr1Enable() palSetLineMode(LINE_TR1_OUT, 8)
#define tr2Enable() palSetLineMode(LINE_TR2_OUT, 8)
#define tr3Enable() palSetLineMode(LINE_TR3_OUT, 8)

#define tr1Disable() palSetLineMode(LINE_TR1_OUT, 0)
#define tr2Disable() palSetLineMode(LINE_TR2_OUT, 0)
#define tr3Disable() palSetLineMode(LINE_TR3_OUT, 0)

#define tr1LineUp() palSetLine(LINE_TR1_OUT)
#define tr2LineUp() palSetLine(LINE_TR2_OUT)
#define tr3LineUp() palSetLine(LINE_TR3_OUT)

#define tr1LineDown() palClearLine(LINE_TR1_OUT)
#define tr2LineDown() palClearLine(LINE_TR2_OUT)
#define tr3LineDown() palClearLine(LINE_TR3_OUT)


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
 * VR1: ADC1 + COMP 1 + TIM15
 * VR2: ADC2 + COMP 2 + TIM16
 * VR3: ADC3 + COMP 6 + TIM17
 * All: DAC1 (bias)
 */

/*
 * Support functions
 */

#define timCounter(tim) tim->CNT

inline static void timDisable(TIM_TypeDef *tim)
{
  tim->CR1 &= ~TIM_CR1_CEN;
}

inline static void timEnable(TIM_TypeDef *tim)
{
  tim->CNT = 0;
  tim->CR1 |= TIM_CR1_CEN;
}

inline static void timRestart(TIM_TypeDef *tim)
{
  timDisable(tim);
  timEnable(tim);
}

inline static void timSetFrequency(TIM_TypeDef *tim, uint32_t freq)
{
  timDisable(tim);
  tim->PSC  = (STM32_TIMCLK2 / freq) - 1; // CLK is 36 (TIM3-4) or 72Mhz (TIM1/8)
}

inline static void timSetReload(TIM_TypeDef *tim, uint32_t value)
{
  tim->ARR = value;
}

/*
 * CALLBACKS
 */


/*
 * Watchdog timeout callbacks
 * This happens when no output was generated before the timer completes
 * We reset the thresholds to default values.
 * Timer should have stopped since we are in one pulse mode.
 */
void VR1_OVERFLOW_HANDLER(void)
{
  vr1.threshold.low = VR_DEFAULT_NEG_THRESHOLD;
  vr1.threshold.high = VR_DEFAULT_POS_THRESHOLD;
  vr1.valid_msk = 0;
}

void VR2_OVERFLOW_HANDLER(void)
{
  vr2.threshold.low = VR_DEFAULT_NEG_THRESHOLD;
  vr2.threshold.high = VR_DEFAULT_POS_THRESHOLD;
  vr2.valid_msk = 0;
}

void VR3_OVERFLOW_HANDLER(void)
{
  vr3.threshold.low = VR_DEFAULT_NEG_THRESHOLD;
  vr3.threshold.high = VR_DEFAULT_POS_THRESHOLD;
  vr3.valid_msk = 0;
}


/*
 * VR validator callbacks
 * We are now far enough in the cycle to enable the output
 */
void VR1_COMPARE_HANDLER(void)
{
  vr1.valid.time = true;
}

void VR2_COMPARE_HANDLER(void)
{
  vr2.valid.time = true;
}

void VR3_COMPARE_HANDLER(void)
{
  vr3.valid.time = true;
}


/*
 * Comparator callback
 * Happends when the COMP zero crosses
 */
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
        uint32_t cnt = timCounter(VR1_TIM);
        uint32_t reload = cnt * VR_DEFAULT_MULT_THRESHOLD;
        if (reload > 0xFFFF)
            reload = 0xFFFF;
        timSetReload(VR1_TIM, reload);

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
        uint32_t cnt = timCounter(VR2_TIM);
        uint32_t reload = cnt * VR_DEFAULT_MULT_THRESHOLD;
        if (reload > 0xFFFF)
            reload = 0xFFFF;
        timSetReload(VR2_TIM, reload);


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
        uint32_t cnt = timCounter(VR3_TIM);
        uint32_t reload = cnt * VR_DEFAULT_MULT_THRESHOLD;
        if (reload > 0xFFFF)
            reload = 0xFFFF;
        timSetReload(VR3_TIM, reload);


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

  dacPutChannelX(&DACD1, 0, 2048); // This sets the biasing for our sensors and comparators.
  compEnable(&COMPD1);
  compEnable(&COMPD2);
  compEnable(&COMPD6);
  opampEnable(&OPAMPD1);
  opampEnable(&OPAMPD2);
  opampEnable(&OPAMPD3);

  setupTimers();

  chThdCreateStatic(waThreadVR1, sizeof(waThreadVR1), NORMALPRIO, ThreadVR1, NULL);
  chThdCreateStatic(waThreadVR2, sizeof(waThreadVR2), NORMALPRIO, ThreadVR2, NULL);
  chThdCreateStatic(waThreadVR3, sizeof(waThreadVR3), NORMALPRIO, ThreadVR3, NULL);
}
