#include "knock.h"
#include "hal.h"
#include "arm_math.h"
#include "ipc.h"
#include "settings.h"

/*
 * Knock peripherals:
 * OPAMP4 (buffer)
 * ADC4 Ch3
 * DAC2 (result output)
 */

static bool sampling_enabled = false;
static adcsample_t knock_samples[FFT_SAMPLES];
static uint16_t knock_value;
static EVENTSOURCE_DECL(evt_knock_result_rdy);

/* Every 1024 samples at 117.263KHz each, triggers at around 114Hz */
CCM_FUNC static void adcCallback(ADCDriver *adcp, adcsample_t *buffer, size_t n)
{
  (void)adcp;

  // Do FFT + Mag in a dedicated thread
  chSysLockFromISR();
  allocSendSamplesI(&knock_mb, (void*)buffer, n); // Send msg with buffer address and size
  chSysUnlockFromISR();
}

static const DACConfig dac_conf = {
  .init         = 2047U,
  .datamode     = DAC_DHRM_12BIT_RIGHT,
  .cr           = 0
};

static const OPAMPConfig opamp_conf = {
  STM32_OPAMP_NonInvertingInput_IO4 | // INP connectd to PB13
  STM32_OPAMP_InvertingInput_Vout // INM connected to vout (follower)
};

/* ADC4 Clk is 72Mhz/1 72Mhz  */
static const ADCConversionGroup adcgrpcfg_knock = {
  TRUE,
  1,
  adcCallback,
  NULL,
  ADC_CFGR_CONT | ADC_CFGR_ALIGN,    /* CFGR - Align result to left (convert 12 to 16 bits) */
  ADC_TR(0, 4095),                  /* TR1     */
  {                                 /* SMPR[2] */
    ADC_SMPR1_SMP_AN1(ADC_SMPR_SMP_601P5),  /* Sampling rate = 72000000/(601.5+12.5) = 117.263Khz  */
    0,
  },
  {                                 /* SQR[4]  */
    ADC_SQR1_SQ1_N(ADC_CHANNEL_IN3), /* Channel 3 */
    0,
    0,
    0
  }
};

CCM_FUNC static uint16_t calculateKnockIntensity(uint16_t tgtFreq, uint16_t ratio, uint16_t smplFreq, const uint16_t* buffer, uint16_t size)
{
  uint16_t i;
  uint16_t res;
  float32_t multiplier;

  const float32_t hzPerBin = (float32_t)smplFreq / (float32_t)size; // 458Hz
  const float32_t flRatio = ((float32_t)ratio / 100000.0f);
  const uint16_t range = 5;
  uint16_t index = (tgtFreq / (uint16_t)hzPerBin);

  if (index < range)
    index = range;
  else if (index > size - range)
    index = size - range;

  res = buffer[index];
  for (i = 1; i < range; i++)
  {
    multiplier = 1.0f / ((float32_t)i / flRatio);
    res += multiplier * (float32_t)buffer[index + i];
    res += multiplier * (float32_t)buffer[index - i];
  }

  return res;
}

/*
 * Knock processing thread.
 */
static float32_t input[FFT_SAMPLES];
static float32_t output[FFT_SAMPLES];
static float32_t mag_knock[SPECTRUM_SIZE];
static uint16_t output_knock[SPECTRUM_SIZE];
static THD_WORKING_AREA(waThreadKnock, 600);
CCM_FUNC THD_FUNCTION(ThreadKnock, arg)
{
  (void)arg;
  (void)waThreadKnock;
  chRegSetThreadName("Knock");

  q15_t* knock_data_ptr;
  size_t knock_data_sz;
  uint16_t i;

  /* ADC 4 Ch3 Offset. -2048 */
  KNOCK_ADC->OFR1 = ADC_OFR1_OFFSET1_EN | (ADC_OFR1_OFFSET1_CH_0 | ADC_OFR1_OFFSET1_CH_1) | (2048 & 0xFFF);

  adcStartConversion(&ADCD3, &adcgrpcfg_knock, knock_samples, FFT_SAMPLES);

  /* Initialize the CFFT/CIFFT module */
  arm_rfft_fast_instance_f32 S1;
  arm_rfft_fast_init_f32(&S1, FFT_SIZE);

  while (TRUE)
  {
    recvFreeSamples(&knock_mb, (void*)&knock_data_ptr, &knock_data_sz, TIME_INFINITE);

    /* Copy and convert ADC samples */
    for (i = 0; i < FFT_SAMPLES; i += 4)
    {
      /* Hann Window */
      const float32_t multiplier = (1.0f - arm_cos_f32((2.0f * PI * (float32_t)i) / (((float32_t)FFT_SIZE * 2.0f) - 1.0f)));
      input[i] = multiplier * (float32_t)knock_data_ptr[i];
      input[i+1] = multiplier * (float32_t)knock_data_ptr[i+1];
      input[i+2] = multiplier * (float32_t)knock_data_ptr[i+2];
      input[i+3] = multiplier * (float32_t)knock_data_ptr[i+3];
    }

    /* Process the data through the RFFT module */
    arm_rfft_fast_f32(&S1, input, output, 0);

    /* Process the data through the Complex Magnitude Module for
    calculating the magnitude at each bin */
    arm_cmplx_mag_f32(output, mag_knock, SPECTRUM_SIZE); // Calculate magnitude, outputs q2.14

    // Convert 2.14 to 16 Bits unsigned
    for (i=0; i < SPECTRUM_SIZE; i++)
    {
      uint32_t tmp = (uint32_t)(mag_knock[i] / 16384.0f);
      if (tmp > 0xFFFF) // Cap to 16b max
        tmp = 0xFFFF;
      output_knock[i] = (uint16_t)tmp; // 16 bits minus the 2 fractional bits
    }

    knock_value = calculateKnockIntensity(
                                          settings.knock_freq,
                                          settings.knock_ratio,
                                          FFT_FREQ,
                                          output_knock,
                                          sizeof(output_knock));
    chEvtBroadcast(&evt_knock_result_rdy);

  }
  return;
}

/*
 * We just keep the peak value and output it
 */
static THD_WORKING_AREA(waThreadKnockOuput, 128);
CCM_FUNC THD_FUNCTION(ThreadKnockOuput, arg)
{
  (void)arg;
  (void)waThreadKnockOuput;
  chRegSetThreadName("Knock Integrator");

  /* Events registration.*/
  event_listener_t el0;
  chEvtRegister(&evt_knock_result_rdy, &el0, 0);

  while (TRUE)
  {
    uint16_t knock_out = 0;
    while (chEvtWaitOne(EVENT_MASK(0)) == 1 && sampling_enabled)
    {
      if (knock_value > knock_out)
      {
        knock_out = knock_value;
      }
      dacPutChannelX(&KNOCK_DACD, 0, knock_out >> 4); // This sets the knock output DAC to our value.
    }
  }
}

static void sample_cb(void *arg)
{
  (void)arg;

  chSysLockFromISR();
  if (palReadLine(LINE_SAMPLE) == PAL_HIGH) {
    sampling_enabled = true;
  }
  else {
    sampling_enabled = false;
  }
  chSysUnlockFromISR();
}

void createKnockThread(void)
{
  opampStart(&KNOCK_OPAMPD, &opamp_conf);
  adcStart(&KNOCK_ADCD, NULL);

  dacStart(&KNOCK_DACD, &dac_conf);
  dacPutChannelX(&KNOCK_DACD, 0, 0); // This sets knock output to 0;

  /* Events initialization. */
  chEvtObjectInit(&evt_knock_result_rdy);

  chThdCreateStatic(waThreadKnock, sizeof(waThreadKnock), NORMALPRIO, ThreadKnock, NULL);
  chThdCreateStatic(waThreadKnockOuput, sizeof(waThreadKnockOuput), NORMALPRIO, ThreadKnockOuput, NULL);

  palEnableLineEvent(LINE_SAMPLE, PAL_EVENT_MODE_BOTH_EDGES);
  palSetLineCallback(LINE_SAMPLE, sample_cb, NULL);
}
