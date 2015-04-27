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

#include "ch.hpp"
#include "hal.h"

#include "chprintf.h"
#include "usbcfg.h"
#include "pads.h"
#include "debouncer.hpp"
#include "pwm_gpt.hpp"

/* Virtual serial port over USB.*/
SerialUSBDriver SDU1;

/*
 ******************************************************************************
 * DEFINES
 ******************************************************************************
 */

#define   UI_REFRESH_PERIOD       MS2ST(5)

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

static bool usb_ready = false;
static Debouncer btn_debouncer(4, 4, read_button);
static Debouncer usb_debouncer(4, 0, read_usb_plug);

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
void ui_print (const char *str) {
  if (usb_ready) {
    chprintf((BaseSequentialStream *)&SDU1, str);
  }
}

/**
 * Button handler thread
 */
static THD_WORKING_AREA(RedBlinkThreadWA, 192);
static THD_FUNCTION(RedBlinkThread, arg) {
  chRegSetThreadName("WarningBlink");
  (void)arg;

  while(!chThdShouldTerminateX()){
    osalThreadSleep(UI_REFRESH_PERIOD);

    if (1 == btn_debouncer.update()) {
      red_led_off();
      toggle_pwm_release();
    }
    else {
      red_led_on();
      toggle_pwm_press();
    }

    if (1 == usb_debouncer.update())
      usb_ready = true;
    else
      usb_ready = false;
  }

  chThdExit(MSG_OK);
}

/*
 * Application entry point.
 */
void uiInit(void) {

  chThdCreateStatic(RedBlinkThreadWA,
          sizeof(RedBlinkThreadWA),
          NORMALPRIO,
          RedBlinkThread,
          NULL);

  /*
   * Initializes a serial-over-USB CDC driver.
   */
  sduObjectInit(&SDU1);
  sduStart(&SDU1, &serusbcfg);

  /*
   * Stopping and restarting the USB in order to test the stop procedure. The
   * following lines are not usually required.
   */
  usbDisconnectBus(serusbcfg.usbp);
  usbStop(serusbcfg.usbp);
  chThdSleepMilliseconds(1000);
  usbStart(serusbcfg.usbp, &usbcfg);
  usbConnectBus(serusbcfg.usbp);

  toggle_pwm_press();
}


