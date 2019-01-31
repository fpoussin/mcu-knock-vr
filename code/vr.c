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
 * VR2: ADC3 + COMP 2 + TIM16
 * VR3: ADC4 + COMP 6 + TIM17
 * All: DAC1 (bias)
 */

/*
 * Support functions
 */

#define timCounter(tim) tim->CNT

CCM_FUNC inline static void timDisable(TIM_TypeDef *tim)
{
  tim->CR1 &= ~TIM_CR1_CEN;
}

CCM_FUNC inline static void timEnable(TIM_TypeDef *tim)
{
  tim->CNT = 0;
  tim->CR1 |= TIM_CR1_CEN;
}

CCM_FUNC inline static void timRestart(TIM_TypeDef *tim)
{
  timDisable(tim);
  timEnable(tim);
}

CCM_FUNC inline static void timSetFrequency(TIM_TypeDef *tim, uint32_t freq)
{
  timDisable(tim);
  tim->PSC  = (STM32_TIMCLK2 / freq) - 1; // CLK is 36 (TIM3-4) or 72Mhz (TIM1/8)
}

CCM_FUNC inline static void timSetReload(TIM_TypeDef *tim, uint32_t value)
{
  tim->ARR = value;
}

/*
 * CALLBACKS and their support functions
 */

inline static void OverflowReset(vr_t *vr)
{
  vr->threshold.low = VR_DEFAULT_NEG_THRESHOLD;
  vr->threshold.high = VR_DEFAULT_POS_THRESHOLD;
  vr->valid_msk = 0;
}

/*
 * Watchdog timeout callbacks
 * This happens when no output was generated before the timer completes
 * We reset the thresholds to default values.
 * Timer should have stopped since we are in one pulse mode.
 */
CCM_FUNC void VR1_OVERFLOW_HANDLER(void)
{
  OverflowReset(&vr1);
}

CCM_FUNC void VR2_OVERFLOW_HANDLER(void)
{
  OverflowReset(&vr2);
}

CCM_FUNC void VR3_OVERFLOW_HANDLER(void)
{
  OverflowReset(&vr3);
}


/*
 * VR validator callbacks
 * We are now far enough in the cycle to enable the output
 */
CCM_FUNC void VR1_COMPARE_HANDLER(void)
{
  vr1.valid.time = true;
}

CCM_FUNC void VR2_COMPARE_HANDLER(void)
{
  vr2.valid.time = true;
}

CCM_FUNC void VR3_COMPARE_HANDLER(void)
{
  vr3.valid.time = true;
}


/* Set new thresholds to 80% of previous peaks, reset validation */
CCM_FUNC static void ComparatorThresholdHandler(vr_t *vr, TIM_TypeDef *tim)
{
  if (vr->valid_msk & VALID_MSK)
  {
    // Get last interval, set timeout;
    uint32_t cnt = timCounter(tim);
    uint32_t reload = cnt * VR_DEFAULT_MULT_THRESHOLD;
    if (reload > 0xFFFF)
        reload = 0xFFFF;
    timSetReload(tim, reload);

    vr->threshold.low = (uint16_t)((float)vr->peak.low * 1.2f);
    vr->threshold.high = (uint16_t)((float)vr->peak.high * 0.8f);
    vr->peak.low = VR_ZERO;
    vr->peak.high = VR_ZERO;
    vr->valid_msk = 0;
  }
}


/*
 * Comparator callback
 * Happends when the COMP zero crosses
 */
CCM_FUNC static void comp_cb(COMPDriver *comp)
{
  /* Check if output is high (rising) */
  if (comp->reg->CSR & COMP_CSR_COMPxOUT)
  {
    if (comp == &VR1_COMPD)
    {
      ComparatorThresholdHandler(&vr1, VR1_TIM);
    }
    else if (comp == &VR2_COMPD)
    {
      ComparatorThresholdHandler(&vr2, VR2_TIM);
    }
    else if (comp == &VR3_COMPD)
    {
      ComparatorThresholdHandler(&vr3, VR3_TIM);
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
  if (adcp == &VR1_ADCD)
  {
    allocSendSamplesI(&vr1_mb, (void*)buffer, n);
  }
  else if (adcp == &VR2_ADCD)
  {
    allocSendSamplesI(&vr2_mb, (void*)buffer, n);
  }
  else if (adcp == &VR3_ADCD)
  {
    allocSendSamplesI(&vr3_mb, (void*)buffer, n);
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
  OPAMP1_CSR_VPSEL_PA01 | // INP connectd to PA1
  OPAMP1_CSR_VMSEL_FOLWR // INM connected to vout (follower)
};

// VR2
static const OPAMPConfig opamp3_conf = {
  OPAMP3_CSR_VPSEL_PB00 | // INP connectd to PB0
  OPAMP3_CSR_VMSEL_FOLWR // INM connected to vout (follower)
};

// VR3
static const OPAMPConfig opamp4_conf = {
  OPAMP4_CSR_VPSEL_PB13 | // INP connectd to PB13
  OPAMP4_CSR_VMSEL_FOLWR // INM connected to vout (follower)
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

/* ADC3 Clk is 72Mhz/1 72Mhz  */
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


/* ADC4 Clk is 72Mhz/1 72Mhz  */
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

CCM_FUNC static bool checkPeak(vr_t* vr, median_t* median, const adcsample_t* samples, size_t size)
{
  /* Filtering and finding min/max */
  uint16_t val, min, max = 0;
  for (uint16_t i = 0; i < size; i++)
  {
    val = median_filter(median, samples[i]);
    if (val > max) max = val;
    if (val < min) min = val;
  }
  if (min < vr->peak.low)
      vr->peak.low = min;
  if (max > vr->peak.high)
      vr->peak.high = max;

  return vr->peak.low <= vr->threshold.low && vr->peak.high >= vr->threshold.high;
}

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
  VR1_ADC->OFR1 = ADC_OFR1_OFFSET1_EN | ADC_OFR1_OFFSET1_CH_0 | ADC_OFR1_OFFSET1_CH_1 | (2048 & 0xFFF);
  adcStartConversion(&VR1_ADCD, &vr1grpcfg, vr1_samples, VR_SAMPLES);

  while (TRUE)
  {
    recvFreeSamples(&vr1_mb, (void*)&adc_data_ptr, &adc_data_size, TIME_INFINITE);

    bool res = checkPeak(&vr1, &median, adc_data_ptr, adc_data_size);

    if (!vr1.valid.peak)
    {
      vr1.valid.peak = res;
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
  VR2_ADC->OFR1 = ADC_OFR1_OFFSET1_EN | ADC_OFR1_OFFSET1_CH_0 | (2048 & 0xFFF);
  adcStartConversion(&VR2_ADCD, &vr2grpcfg, vr2_samples, VR_SAMPLES);

  while (TRUE)
  {
    recvFreeSamples(&vr2_mb, (void*)&adc_data_ptr, &adc_data_size, TIME_INFINITE);

    bool res = checkPeak(&vr2, &median, adc_data_ptr, adc_data_size);

    if (!vr2.valid.peak)
    {
      vr2.valid.peak = res;
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
  VR3_ADC->OFR1 = ADC_OFR1_OFFSET1_EN | ADC_OFR1_OFFSET1_CH_0 | ADC_OFR1_OFFSET1_CH_1 | (2048 & 0xFFF);
  adcStartConversion(&VR3_ADCD, &vr3grpcfg, vr3_samples, VR_SAMPLES);

  while (TRUE)
  {
    recvFreeSamples(&vr3_mb, (void*)&adc_data_ptr, &adc_data_size, TIME_INFINITE);

    bool res = checkPeak(&vr3, &median, adc_data_ptr, adc_data_size);

    if (!vr3.valid.peak)
    {
      vr3.valid.peak = res;
    }
  }
}

void createVrThreads(void)
{
  opampStart(&VR1_OPAMPD, &opamp1_conf);
  opampStart(&VR2_OPAMPD, &opamp3_conf);
  opampStart(&VR3_OPAMPD, &opamp4_conf);
  adcStart(&VR1_ADCD, NULL);
  adcStart(&VR2_ADCD, NULL);
  adcStart(&VR3_ADCD, NULL);
  dacStart(&VR_DACD, &dac_conf);
  compStart(&VR1_COMPD, &comp1_conf);
  compStart(&VR2_COMPD, &comp2_conf);
  compStart(&VR3_COMPD, &comp6_conf);

  dacPutChannelX(&VR_DACD, 0, 2048); // This sets the biasing for our sensors and comparators.
  compEnable(&VR1_COMPD);
  compEnable(&VR2_COMPD);
  compEnable(&VR3_COMPD);
  opampEnable(&VR1_OPAMPD);
  opampEnable(&VR2_OPAMPD);
  opampEnable(&VR3_OPAMPD);

  setupTimers();

  chThdCreateStatic(waThreadVR1, sizeof(waThreadVR1), NORMALPRIO, ThreadVR1, NULL);
  chThdCreateStatic(waThreadVR2, sizeof(waThreadVR2), NORMALPRIO, ThreadVR2, NULL);
  chThdCreateStatic(waThreadVR3, sizeof(waThreadVR3), NORMALPRIO, ThreadVR3, NULL);
}
