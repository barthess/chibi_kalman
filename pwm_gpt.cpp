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

#include "ch.hpp"
#include "hal.h"

/*
 ******************************************************************************
 * DEFINES
 ******************************************************************************
 */

#define PWM_CLK           4000000
#define PWM_MAX_PERIOD    4000
#define PWM_MIN_PERIOD    2

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

/*
 ******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************
 */

static uint16_t pwm_period = PWM_MIN_PERIOD;

/**
 *
 */
static PWMConfig pwm_cfg = {
  4000000,
  PWM_MAX_PERIOD,
  NULL,
  {
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL}
  },
  0,
  0
};

/*
 ******************************************************************************
 ******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************
 ******************************************************************************
 */

/*
 ******************************************************************************
 * EXPORTED FUNCTIONS
 ******************************************************************************
 */


/**
 *
 */
void toggle_pwm_press(void) {

  if (pwm_cfg.period != pwm_period) {
    pwmStop(&PWMD8);
    pwm_cfg.period = pwm_period;
    pwmStart(&PWMD8, &pwm_cfg);
    pwmEnableChannel(&PWMD8, 3, pwm_cfg.period / 2);
  }
}

/**
 *
 */
void toggle_pwm_release(void) {

  if (pwm_cfg.period == pwm_period) {
    pwm_period *= 2;
    if (pwm_period >= PWM_MAX_PERIOD)
      pwm_period = PWM_MIN_PERIOD;
  }
}
