/*
 * CPal.h
 *
 *  Created on: Jul 12, 2019
 *      Author: abusous2000
 */

#ifndef SOURCE_CPAL_H_
#define SOURCE_CPAL_H_
#include "ch.h"
#include "hal.h"

#if HAL_USE_PAL
//  https://chunminchang.github.io/blog/post/how-to-simulate-a-c-class-in-c
//http://www.chibios.org/dokuwiki/doku.php?id=chibios:book:kernel_memory
#define CALL_METHOD(__obj_,___method_)          __obj_->___method_(__obj_
#define CALL_SINGLE_METHOD(__obj_,___method_)   __obj_->___method_(__obj_)
#define VA_ARGS(...) , ##__VA_ARGS__
#define CALL(obj, method, ...) ((obj).method((&obj) VA_ARGS(__VA_ARGS__)))

struct CPal;
typedef struct CPal CPalTypedef;
typedef CPalTypedef *(*initPal)(CPalTypedef *pSelf);
typedef CPalTypedef *(*setPal)(CPalTypedef *pSelf);
typedef CPalTypedef *(*clearPal)(CPalTypedef *pSelf);
typedef bool         (*readPal)(CPalTypedef *pSelf);
typedef CPalTypedef *(*togglePal)(CPalTypedef *pSelf);

struct CPal{
  CPalTypedef   *self;
  ioline_t      line;
  uint32_t      mode;
#if (PAL_USE_CALLBACKS == TRUE) || defined(__DOXYGEN__)
   palcallback_t cb;
  void           *arg;
  ioeventmode_t  risingfallingEdge;
#endif
  initPal       init;
  setPal        set;
  clearPal      clear;
  readPal       read;
  togglePal     toggle;
};
#ifdef __cplusplus
extern "C" {
#endif
CPalTypedef *initCPalInstance(CPalTypedef *pSelf);
CPalTypedef newCPal(void);
#ifdef __cplusplus
}
#endif
#endif

#endif /* SOURCE_CPAL_H_ */
