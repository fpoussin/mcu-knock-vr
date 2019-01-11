#include "vr.h"
#include "hal.h"
#include "ipc.h"
#include "settings.h"
#include "median.h"

/*
 * VR peripherals
 * VR1: ADC1 + COMP 1
 * VR2: ADC2 + COMP 2
 * VR3: ADC3 + COMP 6
 */

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
    while (!recvFreeSamples(&vr1_mb, (void*)&adc_data_ptr, &adc_data_size))
      chThdSleepMilliseconds(5);

    /* Filtering and finding min/max */
    uint16_t val, min, max = 0;
    for (uint16_t i = 0; i < VR_SAMPLES; i++)
    {
      val = median_filter(&median, adc_data_ptr[i]);
      if (val > max) max = val;
      if (val < min) min = val;
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
    while (!recvFreeSamples(&vr2_mb, (void*)&adc_data_ptr, &adc_data_size))
      chThdSleepMilliseconds(5);

    /* Filtering and finding min/max */
    uint16_t val, min, max = 0;
    for (uint16_t i = 0; i < VR_SAMPLES; i++)
    {
      val = median_filter(&median, adc_data_ptr[i]);
      if (val > max) max = val;
      if (val < min) min = val;
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
    while (!recvFreeSamples(&vr3_mb, (void*)&adc_data_ptr, &adc_data_size))
      chThdSleepMilliseconds(5);

    /* Filtering and finding min/max */
    uint16_t val, min, max = 0;
    for (uint16_t i = 0; i < VR_SAMPLES; i++)
    {
      val = median_filter(&median, adc_data_ptr[i]);
      if (val > max) max = val;
      if (val < min) min = val;
    }
  }

}

void createVrThreads(void)
{
    chThdCreateStatic(waThreadVR1, sizeof(waThreadVR1), NORMALPRIO, ThreadVR1, NULL);
    chThdCreateStatic(waThreadVR2, sizeof(waThreadVR2), NORMALPRIO, ThreadVR2, NULL);
    chThdCreateStatic(waThreadVR3, sizeof(waThreadVR3), NORMALPRIO, ThreadVR3, NULL);
}
