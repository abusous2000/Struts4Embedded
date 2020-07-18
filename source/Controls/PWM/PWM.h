/*
 * PWM.h
 *
 *  Created on: Jan 1, 2020
 *      Author: abusous2000
 */
#include "Strust4EmbeddedConf.h"
#include "ch.h"
#include "hal.h"
#ifndef S4E_USE_PWM
#define S4E_USE_PWM 	0
#endif
#if S4E_USE_PWM != 0
#ifndef SOURCE_STRUTS4EMBEDDED_PWM_PWM_H_
#define SOURCE_STRUTS4EMBEDDED_PWM_PWM_H_



#define TIM_FREQ 		10000
//the defaults are tailored for STM32F7xxx
#ifndef PWM_LINE1
#define PWM_LINE1 		LINE_ARD_D0
#define PWM_LINE2 		LINE_ARD_D1
#define PWM_LINE1_CH    0
#define PWM_LINE2_CH    1
#define PWM_LINE_AF   	3
#endif
#ifdef __cplusplus
 extern "C" {
#endif
void initPWM(void);
void reInitPWM(uint32_t  frequency, pwmcnt_t    period , double dc1, double dc2);
void setPWMDutyCycle(uint32_t dc1, uint32_t dc2);
#ifdef __cplusplus
 }
#endif
#endif /* SOURCE_STRUTS4EMBEDDED_PWM_PWM_H_ */
#endif
