/*
 * P9813_RGB_Driver.h
 *
 *  Created on: Dec 31, 2019
 *      Author: abusous2000
 */
#include "Strust4EmbeddedConf.h"
#include "ch.h"
#include "hal.h"
#ifndef S4E_USE_RGB
#define S4E_USE_RGB  0
#endif

#ifndef SOURCE_ACTIONEVENTS_P9813_RGB_DRIVER_H_
#define SOURCE_ACTIONEVENTS_P9813_RGB_DRIVER_H_
#if S4E_USE_RGB == 1
#include "CPal.h"
#define LINE_IO_DATA		LINE_ARD_D5
#define LINE_IO_CLOCK		LINE_ARD_D6
#define RGB_LED_GPIO_MODE   PAL_STM32_MODE_OUTPUT | PAL_STM32_OTYPE_PUSHPULL
#define TOTAL_NUM_OF_LEDS   1
#ifdef __cplusplus
 extern "C" {
#endif
void initP9813RGBDriver(uint8_t num_leds);
void setColorRGB(uint8_t led, uint8_t red, uint8_t green, uint8_t blue);

#ifdef __cplusplus
 }
#endif
#endif
#endif/* SOURCE_ACTIONEVENTS_P9813_RGB_DRIVER_H_ */

