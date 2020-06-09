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
#if S4E_USE_PWM == 1
#ifndef SOURCE_STRUTS4EMBEDDED_PWM_PWM_H_
#define SOURCE_STRUTS4EMBEDDED_PWM_PWM_H_

#define PWM_DRIVER 		PWMD8
#define TIM8_FREQ 		10000
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
