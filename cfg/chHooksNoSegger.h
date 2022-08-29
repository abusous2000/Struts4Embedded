/*
 * chHooks.h
 *
 *  Created on: Oct 3, 2020
 *      Author: abusous2000
 */

#ifndef CFG_CHHOOKS_H_
#define CFG_CHHOOKS_H_

#if INCLUDE_SEGGER_JLINK == 0
/*===========================================================================*/
/**
 * @name Kernel hooks
 * @{
 */
/*===========================================================================*/

/**
 * @brief   System structure extension.
 * @details User fields added to the end of the @p ch_system_t structure.
 */
#define CH_CFG_SYSTEM_EXTRA_FIELDS                                          \
  /* Add system custom fields here.*/

/**
 * @brief   System initialization hook.
 * @details User initialization code added to the @p chSysInit() function
 *          just before interrupts are enabled globally.
 */
#define CH_CFG_SYSTEM_INIT_HOOK() {                                         \
  /* Add system initialization code here.*/                                 \
}

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

/**
 * @brief   Threads descriptor structure extension.
 * @details User fields added to the end of the @p thread_t structure.
 */
#define CH_CFG_THREAD_EXTRA_FIELDS                                          \
  /* Add threads custom fields here.*/

/**
 * @brief   Threads initialization hook.
 * @details User initialization code added to the @p _thread_init() function.
 *
 * @note    It is invoked from within @p _thread_init() and implicitly from all
 *          the threads creation APIs.
 *
 * @param[in] tp        pointer to the @p thread_t structure
 */
#define CH_CFG_THREAD_INIT_HOOK(tp) {                                       \
  /* Add threads initialization code here.*/                                \
}

/**
 * @brief   Threads finalization hook.
 * @details User finalization code added to the @p chThdExit() API.
 *
 * @param[in] tp        pointer to the @p thread_t structure
 */
#define CH_CFG_THREAD_EXIT_HOOK(tp) {                                       \
  /* Add threads finalization code here.*/                                  \
}

/**
 * @brief   Context switch hook.
 * @details This hook is invoked just before switching between threads.
 *
 * @param[in] ntp       thread being switched in
 * @param[in] otp       thread being switched out
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
 * @brief   Idle thread enter hook.
 * @note    This hook is invoked within a critical zone, no OS functions
 *          should be invoked from here.
 * @note    This macro can be used to activate a power saving mode.
 */
#define CH_CFG_IDLE_ENTER_HOOK() {                                          \
  /* Idle-enter code here.*/                                                \
}

/**
 * @brief   Idle thread leave hook.
 * @note    This hook is invoked within a critical zone, no OS functions
 *          should be invoked from here.
 * @note    This macro can be used to deactivate a power saving mode.
 */
#define CH_CFG_IDLE_LEAVE_HOOK() {                                          \
  /* Idle-leave code here.*/                                                \
}

/**
 * @brief   Idle Loop hook.
 * @details This hook is continuously invoked by the idle thread loop.
 */
#define CH_CFG_IDLE_LOOP_HOOK() {                                           \
  /* Idle loop code here.*/                                                 \
}

/**
 * @brief   System tick event hook.
 * @details This hook is invoked in the system tick handler immediately
 *          after processing the virtual timers queue.
 */
#define CH_CFG_SYSTEM_TICK_HOOK() {                                         \
  /* System tick event code here.*/                                         \
}

/**
 * @brief   System halt hook.
 * @details This hook is invoked in case to a system halting error before
 *          the system is halted.
 */
#define CH_CFG_SYSTEM_HALT_HOOK(reason) {                                   \
  /* System halt code here.*/                                               \
}

/**
 * @brief   Trace hook.
 * @details This hook is invoked each time a new record is written in the
 *          trace buffer.
 */
#define CH_CFG_TRACE_HOOK(tep) {                                            \
  /* Trace code here.*/                                                     \
}

/**
 * @brief   Runtime Faults Collection Unit hook.
 * @details This hook is invoked each time new faults are collected and stored.
 */
#define CH_CFG_RUNTIME_FAULTS_HOOK(mask) {                                  \
  /* Faults handling code here.*/                                           \
}

/** @} */

#else
#error Segger hooks needs to be migrated for ChibiOS 21.6.0+
#endif


#if !defined(_FROM_ASM_)
#if INCLUDE_SEGGER_JLINK != 0
#include "SEGGER_SYSVIEW_ChibiOS.h"
#endif
#endif

#endif /* CFG_CHHOOKS_H_ */
