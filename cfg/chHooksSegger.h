
/*
 * chHooks.h
 *
 *  Created on: Oct 3, 2020
 *      Author: abusous2000
 */

#ifndef CFG_CHHOOKS_H_
#define CFG_CHHOOKS_H_
/*===========================================================================*/
/**
 * @name Kernel hooks
 * @{
 */
/*===========================================================================*/

/**
 * @brief   OS instance structure extension.
 * @details User fields added to the end of the @p os_instance_t structure.
 */
#define CH_CFG_OS_INSTANCE_EXTRA_FIELDS                                     \
  /* Add OS instance custom fields here.*/

/**
 * @brief   OS instance initialization hook.
 *
 * @param[in] oip       pointer to the @p os_instance_t structure
 */
#define CH_CFG_OS_INSTANCE_INIT_HOOK(oip) {                                 \
  /* Add OS instance initialization code here.*/                            \
}

#define CH_CFG_RUNTIME_FAULTS_HOOK(mask) {                                  \
  /* Faults handling code here.*/                                           \
}

#if INCLUDE_SEGGER_JLINK == 0

#define CH_CFG_THREAD_INIT_HOOK(tp) {                                       \
  /* Add threads initialization code here.*/                                \
}

#define CH_CFG_THREAD_INIT_HOOK(tp) {                                       \
  /* Add threads initialization code here.*/                                \
}

/**
 * @brief   Threads finalization hook.
 * @details User finalization code added to the @p chThdExit() API.
 */
#define CH_CFG_THREAD_EXIT_HOOK(tp) {                                       \
  /* Add threads finalization code here.*/                                  \
}

/**
 * @brief   Context switch hook.
 * @details This hook is invoked just before switching between threads.
 */
#define CH_CFG_CONTEXT_SWITCH_HOOK(ntp, otp) {                              \
  /* Context switch code here.*/                                            \
}

/**
 * @brief   ISR enter hook.
 */
#define CH_CFG_IRQ_PROLOGUE_HOOK() {                                        \
  /* IRQ prologue code here.*/                                              \
}

/**
 * @brief   ISR exit hook.
 */
#define CH_CFG_IRQ_EPILOGUE_HOOK() {                                        \
  /* IRQ epilogue code here.*/                                              \
}
/**
 * @brief   System halt hook.
 * @details This hook is invoked in case to a system halting error before
 *          the system is halted.
 */

#define CH_CFG_SYSTEM_HALT_HOOK(reason) {                                   \
  /* System halt code here.*/                                               \
}

#else

#define _CH_CFG_THREAD_INIT_HOOK(tp) {                                      \
  /* Add threads initialization code here.*/                                \
}

#define _CH_CFG_THREAD_INIT_HOOK(tp) {                                      \
  /* Add threads initialization code here.*/                                \
}
/**
 * @brief   Threads finalization hook.
 * @details User finalization code added to the @p chThdExit() API.
 */
#define _CH_CFG_THREAD_EXIT_HOOK(tp) {                                      \
  /* Add threads finalization code here.*/                                  \
}

/**
 * @brief   Context switch hook.
 * @details This hook is invoked just before switching between threads.
 */
#define _CH_CFG_CONTEXT_SWITCH_HOOK(ntp, otp) {                             \
  /* Context switch code here.*/                                            \
}

/**
 * @brief   ISR enter hook.
 */
#define _CH_CFG_IRQ_PROLOGUE_HOOK() {                                       \
  /* IRQ prologue code here.*/                                              \
}

/**
 * @brief   ISR exit hook.
 */
#define _CH_CFG_IRQ_EPILOGUE_HOOK() {                                       \
  /* IRQ epilogue code here.*/                                              \
}
/**
 * @brief   System halt hook.
 * @details This hook is invoked in case to a system halting error before
 *          the system is halted.
 */
#define _CH_CFG_SYSTEM_HALT_HOOK(reason) {                                 \
 /* System halt code here.*/                                               \
}

#endif




#if !defined(_FROM_ASM_)
	#if INCLUDE_SEGGER_JLINK != 0
	#include "SEGGER_SYSVIEW_ChibiOS.h"
	#endif
#endif

#endif /* CFG_CHHOOKS_H_ */
