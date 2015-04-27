/*
    ChibiOS/RT - Copyright (C) 2013-2014 Uladzimir Pylinsky aka barthess

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

#include <string.h>
#include <stdlib.h>

#include "ch.hpp"
#include "hal.h"

#include "fsmc_sram.h"
#include "pads.h"
#include "ui.hpp"
#include "memtest.hpp"

/*
 ******************************************************************************
 * DEFINES
 ******************************************************************************
 */
#define USE_INFINITE_MEMTEST    TRUE

/*
 ******************************************************************************
 * EXTERNS
 ******************************************************************************
 */

/*
 ******************************************************************************
 * PROTOTYPES
 ******************************************************************************
 */
static void mem_error_cb(memtest_t *memp, testtype_t e, size_t address);

/*
 ******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************
 */

static const size_t sram_size = 64 * 1024;
static double test_buf_mtrx[33*33];

static const SRAMConfig sram_cfg = {
    (3 << 8) | (1 << 0)
};

static memtest_t memtest_struct = {
    (void *)FSMC_Bank1_1_MAP,
    sram_size,
    MEMTEST_WIDTH_16,
    mem_error_cb,
    42
};

time_measurement_t mem_tmu;

/*
 ******************************************************************************
 ******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************
 ******************************************************************************
 */

static void mem_error_cb(memtest_t *memp, testtype_t e, size_t address) {
  (void)memp;
  (void)e;
  (void)address;

  green_led_off();
  red_led_on();
  osalSysHalt("Memory broken");
}

/*
 *
 */
static void memtest(void) {

  red_led_off();

  while (true) {
    memtest_struct.rand_seed = chSysGetRealtimeCounterX();
    memtest_run(&memtest_struct, MEMTEST_RUN_ALL);
    //memtest_run(&memtest_struct, MEMTEST_MOVING_INVERSION_RAND);
    green_led_toggle();
  }

  green_led_on();
  green_led_off();
}

/*
 *
 */
static void membench(void) {

  chTMObjectInit(&mem_tmu);

  double pattern = 1.1;
  for (size_t i=0; i<33*33; i++) {
    test_buf_mtrx[i] = pattern;
    pattern += pattern;
  }

  green_led_on();
  chTMStartMeasurementX(&mem_tmu);
  for (size_t i=0; i<1000; i++)
    memcpy(memtest_struct.start, test_buf_mtrx, sizeof(test_buf_mtrx));
  chTMStopMeasurementX(&mem_tmu);
  green_led_off();
}

/*
 ******************************************************************************
 * EXPORTED FUNCTIONS
 ******************************************************************************
 */

/*
 * Application entry point.
 */
int main(void) {

  /*
  * System initializations.
  * - HAL initialization, this also initializes the configured device drivers
  *   and performs the board-specific initializations.
  * - Kernel initialization, the main() function becomes a thread and the
  *   RTOS is active.
  */
  halInit();
  chSysInit();

  osalThreadSleepMilliseconds(500);

  uiInit();

  fsmcSramInit();
  fsmcSramStart(&SRAMD1, &sram_cfg);

  membench();
  memtest();

  /*
   * Normal main() thread activity, in this demo it does nothing.
   */
  while (TRUE) {
    chThdSleepMilliseconds(100);
    green_led_toggle();
  }
}


