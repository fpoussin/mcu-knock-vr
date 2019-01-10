/*
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"
#include "threads.h"

static const DACConfig dac_conf = {
  .init         = 2047U,
  .datamode     = DAC_DHRM_12BIT_RIGHT,
  .cr           = 0
};

// VR1
static const COMPConfig comp1_conf = {
  COMP_OUTPUT_NORMAL,
  COMP_IRQ_RISING,
  NULL,
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
  NULL,
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
  NULL,
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

// VR1
static const OPAMPConfig opamp2_conf = {
  STM32_OPAMP_NonInvertingInput_IO2 | // INP connectd to PB14
  STM32_OPAMP_InvertingInput_Vout // INM connected to vout (follower)
};

// VR1
static const OPAMPConfig opamp3_conf = {
  STM32_OPAMP_NonInvertingInput_IO3 | // INP connectd to PB0
  STM32_OPAMP_InvertingInput_Vout // INM connected to vout (follower)
};

// VR1
static const OPAMPConfig opamp4_conf = {
  STM32_OPAMP_NonInvertingInput_IO4 | // INP connectd to PB13
  STM32_OPAMP_InvertingInput_Vout // INM connected to vout (follower)
};


/*
 * CPU Load Monitoring thread.
 */
static THD_WORKING_AREA(waThreadMonitor, 128);
static THD_FUNCTION(ThreadMonitor, arg)
{
  (void)arg;
  chRegSetThreadName("Monitor");

  DWT->CTRL |= DWT_CTRL_EXCEVTENA_Msk;

  // TODO
  while (TRUE)
  {

    chThdSleepMilliseconds(1000);
  }
  return;
}

/*
 * Application entry point.
 */
int main(void) {

  halInit();
  chSysInit();

  /*
   * Start peripherals
   */
  adcStart(&ADCD1, NULL);
  adcStart(&ADCD2, NULL);
  adcStart(&ADCD3, NULL);
  adcStart(&ADCD4, NULL);
  dacStart(&DACD1, &dac_conf);
  dacStart(&DACD2, &dac_conf);
  compStart(&COMPD1, &comp1_conf);
  compStart(&COMPD2, &comp2_conf);
  compStart(&COMPD6, &comp6_conf);
  opampStart(&OPAMPD1, &opamp1_conf);
  opampStart(&OPAMPD2, &opamp2_conf);
  opampStart(&OPAMPD3, &opamp3_conf);
  opampStart(&OPAMPD4, &opamp4_conf);

  /*
   * Starting a continuous conversion.
   */
  dacPutChannelX(&DACD1, 0, 2048); // This sets the biasing for our sensors and comparators.
  dacPutChannelX(&DACD2, 0, 0); // This sets the knock output DAC to 0.
  compEnable(&COMPD1);
  compEnable(&COMPD2);
  compEnable(&COMPD6);
  opampEnable(&OPAMPD1);
  opampEnable(&OPAMPD2);
  opampEnable(&OPAMPD3);
  opampEnable(&OPAMPD4);

  createKnockThread();
  createVrThreads();

  chThdCreateStatic(waThreadMonitor, sizeof(waThreadMonitor), NORMALPRIO+10, ThreadMonitor, NULL);

  /*
   * Normal main() thread activity.
   */
  while (true) {

    chThdSleepMilliseconds(250);
  }
  return 0;
}
