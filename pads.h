#ifndef PADS_H_
#define PADS_H_

#include "board.h"

static inline unsigned int read_button(void)  {return palReadPad(GPIOE, GPIOE_BUTTON);}
static inline unsigned int read_usb_plug(void){return palReadPad(GPIOA, GPIOA_USB_PRESENT);}

static inline void red_led_on(void)           {palSetPad(GPIOE,     GPIOE_LED_R);}
static inline void red_led_off(void)          {palClearPad(GPIOE,   GPIOE_LED_R);}
static inline void red_led_toggle(void)       {palTogglePad(GPIOE,  GPIOE_LED_R);}
static inline void green_led_on(void)         {palSetPad(GPIOE,     GPIOE_LED_G);}
static inline void green_led_off(void)        {palClearPad(GPIOE,   GPIOE_LED_G);}
static inline void green_led_toggle(void)     {palTogglePad(GPIOE,  GPIOE_LED_G);}

#endif /* PADS_H_ */
