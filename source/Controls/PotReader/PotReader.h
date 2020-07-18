/*
 * PotReader.h
 *
 *  Created on: Aug 23, 2019
 *      Author: abusous2000
 */

#ifndef SOURCE_POTREADER_POTREADER_H_
#define SOURCE_POTREADER_POTREADER_H_
#include "Strust4EmbeddedConf.h"
#include "ch.h"
#include "hal.h"
#include "Strust4Embedded.h"
#ifndef S4E_USE_POT
#define S4E_USE_POT 0
#endif
#if S4E_USE_POT != 0
#ifndef ADC_POT_TIM
#define ADC_POT_TIM                 GPTD4
#endif

#define MAX_ADC_VALUE               4096
#ifndef POT_GPIO_PIN
#define POT_GPIO_PIN                LINE_ARD_A0
#endif
#define ADC_TIM_FREQ 				10000U
#define TIM_PERIOD_CYCLES   		2500         ///5.4KHz/2700=2Hz-->Equation as follows-->update event (in Hz)= Timer Clock /((prescaler+1)*(period+1))
#define POT_VALUE_BTWN_0_TO_100    ((100*getPotValue())/MAX_ADC_VALUE)

#ifdef __cplusplus
 extern "C" {
#endif
adcsample_t  getPotValue(void);
void initPotReader(void);
void checkOnPotVolumeChange(void);
#ifdef __cplusplus
 }
#endif

#endif
#endif /* SOURCE_POTREADER_POTREADER_H_ */
