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
static uint8_t test_buf[sram_size];

/* this config successfully works with . */
//static const SRAMConfig sram_cfg = {
//    (2 << 8)
//};

static const SRAMConfig sram_cfg = {
    (4 << 8)
};

static time_measurement_t tmu_memtest_16;
static time_measurement_t tmu_memtest_8;

static memtest_t memtest_struct = {
    (void *)FSMC_Bank1_1_MAP,
    sram_size,
    MEMTEST_WIDTH_16,
    mem_error_cb,
    42
};

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
  osalSysHalt("");
}

/**
 *
 */
static void memtest(void) {
  chTMObjectInit(&tmu_memtest_16);
  chTMObjectInit(&tmu_memtest_8);

  green_led_on();
  while (true) {
    memtest_struct.width = MEMTEST_WIDTH_16;
    chTMStartMeasurementX(&tmu_memtest_16);
    memtest_run(&memtest_struct, MEMTEST_WALKING_ONE);
    chTMStopMeasurementX(&tmu_memtest_16);

    memtest_struct.width = MEMTEST_WIDTH_8;
    chTMStartMeasurementX(&tmu_memtest_8);
    memtest_run(&memtest_struct, MEMTEST_WALKING_ONE);
    chTMStopMeasurementX(&tmu_memtest_8);
  }
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

  //uiInit();

  fsmcSramInit();
  fsmcSramStart(&SRAMD1, &sram_cfg);

  memtest();

  /*
   * Normal main() thread activity, in this demo it does nothing.
   */
  while (TRUE) {
    chThdSleepMilliseconds(100);
    green_led_toggle();
  }
}


