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
#include "usb_config.h"

/*
 * Watchdog deadline set to 250ms (LSI=40000 / (16 * 1000)).
 */
static const WDGConfig wdgcfg = {
  STM32_IWDG_PR_16,
  STM32_IWDG_RL(1000),
  STM32_IWDG_WIN_DISABLED
};

/*
 * CPU Load Monitoring thread.
 */
static THD_WORKING_AREA(waThreadMonitor, 64);
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
}

static THD_WORKING_AREA(waThreadWdg, 64);
static THD_FUNCTION(ThreadWdg, arg)
{
  (void)arg;
  chRegSetThreadName("Watchdog");

  while (true)
  {
    wdgResetI(&WDGD1);
    chThdSleepMilliseconds(200);
  }
}

/*
 * Application entry point.
 */
int main(void) {

  halInit();
  chSysInit();

  wdgStart(&WDGD1, &wdgcfg);
  createKnockThread();
  createVrThreads();

  chThdCreateStatic(waThreadMonitor, sizeof(waThreadMonitor), NORMALPRIO + 10, ThreadMonitor, NULL);
  chThdCreateStatic(waThreadWdg, sizeof(waThreadWdg), HIGHPRIO, ThreadWdg, NULL);

  /*
   * Normal main() thread activity.
   */
  while (true)
  {
    chThdSleepMilliseconds(100);
  }
}
