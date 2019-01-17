#include "vr.h"
#include "hal.h"
#include "ipc.h"
#include "settings.h"
#include "median.h"

typedef struct
{
  uint16_t low;
  uint16_t high;
} vr_peak_t;

static struct vr_peaks
{
  vr_peak_t vr1;
  vr_peak_t vr2;
  vr_peak_t vr3;
} vr_peaks = {{0,0},{0,0},{0,0}};

static struct vr_threshold
{
  vr_peak_t vr1;
  vr_peak_t vr2;
  vr_peak_t vr3;
} vr_threshold = {{0,4095},{0,4095},{0,4095}};

static struct vr_threshold_valid
{
  bool vr1:1;
  bool vr2:1;
  bool vr3:1;
  char pad:5;
} vr_threshold_valid = {0,0,0,0};

/*
 * Peripherals
 * VR1: ADC1 + COMP 1
 * VR2: ADC2 + COMP 2
 * VR3: ADC3 + COMP 6
 * DAC1 (bias)
 */


/*
 * CALLBACKS
 */

CCM_FUNC static void comp_cb(COMPDriver *comp) {

  /* Check if output is high (rising) */
  if (comp->reg->CSR & COMP_CSR_COMPxOUT) {

    /* Set new thresholds to 80% of previous peaks */
    if (comp == &COMPD1)
    {
      vr_threshold.vr1.low = (uint16_t)((float)vr_peaks.vr1.low * 1.2f);
      vr_threshold.vr1.high = (uint16_t)((float)vr_peaks.vr1.high * 0.8f);
      vr_threshold_valid.vr1 = false;
    }
    else if (comp == &COMPD2)
    {
      vr_threshold.vr2.low = (uint16_t)((float)vr_peaks.vr2.low * 1.2f);
      vr_threshold.vr2.high = (uint16_t)((float)vr_peaks.vr2.high * 0.8f);
      vr_threshold_valid.vr2 = false;
    }
    else if (comp == &COMPD6)
    {
      vr_threshold.vr3.low = (uint16_t)((float)vr_peaks.vr3.low * 1.2f);
      vr_threshold.vr3.high = (uint16_t)((float)vr_peaks.vr3.high * 0.8f);
      vr_threshold_valid.vr3 = false;
    }
  }
}

/* Every 1024 samples at 972.972Khz each, triggers at around 1KHz */
CCM_FUNC static void adcCallback(ADCDriver *adcp, adcsample_t *buffer, size_t n)
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
  COMP_IRQ_RISING,
  comp_cb,
  STM32_COMP_InvertingInput_DAC1OUT1 |
  STM32_COMP_NonInvertingInput_IO1 | // PA1
  STM32_COMP_Hysteresis_Low |
  STM32_COMP_OutputLevel_High |
  STM32_COMP_Mode_HighSpeed // CSR
};

// VR2
static const COMPConfig comp2_conf = {
  COMP_OUTPUT_NORMAL,
  COMP_IRQ_RISING,
  comp_cb,
  STM32_COMP_InvertingInput_DAC1OUT1 |
  STM32_COMP_NonInvertingInput_IO1 | // PA7
  STM32_COMP_Hysteresis_Low |
  STM32_COMP_OutputLevel_High |
  STM32_COMP_Mode_HighSpeed // CSR
};

// VR3
static const COMPConfig comp6_conf = {
  COMP_OUTPUT_NORMAL,
  COMP_IRQ_RISING,
  comp_cb,
  STM32_COMP_InvertingInput_DAC1OUT1 |
  STM32_COMP_NonInvertingInput_IO2 | // PB11
  STM32_COMP_Hysteresis_Low |
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
    if (min < vr_peaks.vr1.low)
        vr_peaks.vr1.low = min;
    if (max > vr_peaks.vr1.high)
        vr_peaks.vr1.high = max;
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
    if (min < vr_peaks.vr2.low)
        vr_peaks.vr2.low = min;
    if (max > vr_peaks.vr2.high)
        vr_peaks.vr2.high = max;
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
    if (min < vr_peaks.vr3.low)
        vr_peaks.vr3.low = min;
    if (max > vr_peaks.vr3.high)
        vr_peaks.vr3.high = max;
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

  chThdCreateStatic(waThreadVR1, sizeof(waThreadVR1), NORMALPRIO, ThreadVR1, NULL);
  chThdCreateStatic(waThreadVR2, sizeof(waThreadVR2), NORMALPRIO, ThreadVR2, NULL);
  chThdCreateStatic(waThreadVR3, sizeof(waThreadVR3), NORMALPRIO, ThreadVR3, NULL);
}
