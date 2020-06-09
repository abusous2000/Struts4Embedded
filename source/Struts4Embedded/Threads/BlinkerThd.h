/*
 * BlinkerThd.h
 *
 *  Created on: Sep 8, 2019
 *      Author: abusous2000
 */

#ifndef BLINKERTHD_H_
#define BLINKERTHD_H_
#include "CPal.h"
#ifndef S4E_USE_BLINKER_THD
#define S4E_USE_BLINKER_THD FALSE
#endif
#if S4E_USE_BLINKER_THD == TRUE

#ifndef BLINKER_THD_FREQ
#define BLINKER_THD_FREQ	500
#endif
#ifndef BLINKER_THD_STACK_SIZE
#define BLINKER_THD_STACK_SIZE	512
#endif
#ifdef __cplusplus
 extern "C" {
#endif
 void updateLCDScreen(uint32_t i);
 void initBlinkerThd(CPalTypedef  *pPal);
 void periodicSysTrigger(uint32_t i);
 extern thread_t *screenUpdaterThdHandle;
#ifdef __cplusplus
}
#endif
#endif
#endif /* BLINKERTHD_H_ */
