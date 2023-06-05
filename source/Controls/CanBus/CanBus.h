/*
 * CanBus.h
 *
 *  Created on: Nov 14, 2021
 *      Author: abusous2000
 */
#include "Strust4EmbeddedConf.h"
#if S4E_USE_CAN_BUS != 0
#include "ch.h"
#include "hal.h"

#ifndef SOURCE_CONTROLS_CANBUS_CANBUS_H_
#define SOURCE_CONTROLS_CANBUS_CANBUS_H_



#ifndef CAN2_RX_LINE
	#ifdef STM32F4_INDUSTRIAL1_NEW
	#define CAN1_RX_LINE       PAL_LINE(GPIOA, 11)
	#define CAN1_TX_LINE       PAL_LINE(GPIOA, 12)
	#define CAN2_RX_LINE       PAL_LINE(GPIOB, 12)
	#define CAN2_TX_LINE       PAL_LINE(GPIOB, 13)
	#elif defined(STM32F4_INDUSTRIAL1_OLD)
	#define CAN1_RX_LINE       PAL_LINE(GPIOD, 0)
	#define CAN1_TX_LINE       PAL_LINE(GPIOD, 1)
	#define CAN2_RX_LINE       PAL_LINE(GPIOB, 5)
	#define CAN2_TX_LINE       PAL_LINE(GPIOB, 6)
	#elif defined(BOARD_BLACKBOARD_INDUSTRIAL2)
	#define CAN1_RX_LINE       PAL_LINE(GPIOA, 11)
	#define CAN1_TX_LINE       PAL_LINE(GPIOA, 12)
	#define CAN2_RX_LINE       PAL_LINE(GPIOB, 12)
	#define CAN2_TX_LINE       PAL_LINE(GPIOB, 13)
	#else
	#error Can Bus: plz define which board you are using
	#endif
#endif
#ifndef CAN_BUD_DUMMY_THRD_SLEEP
#define CAN_BUD_DUMMY_THRD_SLEEP     500
#endif
#ifndef CAN_PORT_MODE
#define CAN_PORT_MODE       	                PAL_MODE_ALTERNATE(CAN_BUS_AF)    |    PAL_STM32_OSPEED_HIGHEST | PAL_MODE_INPUT_PULLUP
#endif

#ifndef CAN_BUS_LOG_TEXT
#define CAN_BUS_LOG_TEXT 0
#endif

#ifndef CAN_BUS_START_DUMMY_MSG_SENDER
#define CAN_BUS_START_DUMMY_MSG_SENDER 1
#endif

#ifndef CAN_BUS_START_CAN1_THD
#define CAN_BUS_START_CAN1_THD 0
#endif

#ifndef CAN_BUS_START_CAN2_THD
#define CAN_BUS_START_CAN2_THD 1
#endif
/*
 * The default is 500kb; which is common
 * Internal loopback mode, 500KBaud, automatic wakeup, automatic recover
 * See section 22.7.7 on the STM32F4 reference manual. This is how the boad rate is calculated
 * STM32_PCLK1/((BRP+1)*(SWJ+(TS1+1)+(TS2+1)))
 * 42M/((1+6+7)*6)=500Kbps
 * 42M/((1+9+2)*7)=
 * Receiver thread.
 */
#ifndef CAN_BUS_BTR_SJW
#define CAN_BUS_BTR_SJW  0
#endif
#ifndef CAN_BUS_BTR_TS1
#define CAN_BUS_BTR_TS1  8
#endif
#ifndef CAN_BUS_BTR_TS2
#define CAN_BUS_BTR_TS2  1
#endif
#ifndef CAN_BUS_BTR_BRP
#define CAN_BUS_BTR_BRP  6
#endif

#define MAX_CAN_MSGS_TO_MONITOR   10

typedef struct CanBusMonitorChange_Typedef {
	uint32_t    msgId;
	uint8_t     monitorByte[8];
	char        *actionEventToExecute;
	CANRxFrame  lastRXFrame;
}CanBusMonitorChange_Typedef;

enum CanBusTogglerEnum {
  TOGGLE_CAN_BUS               = 1,
  TOGGLE_CAN_BUS_DUMMY_DATA    = 2,
  TOGGLE_CAN_BUS_STREAM_DATA   = 3,
  CAN_BUS_SET_FILTER           = 4
};

#endif//S4E_USE_CAN_BUS
#ifdef __cplusplus
 extern "C" {
#endif
uint8_t handleCanMsg(CANRxFrame  *pRXFrame);
void canBusControl(uint32_t   canBusToggler, char *pData);
void changeFilter(uint32_t idMask1, uint32_t idMask2);
void initCanBusThreads(void);
void sendCanMsg(uint32_t msgId, uint8_t len, uint8_t *pData8);
#ifdef __cplusplus
 }
#endif
#endif /* SOURCE_CONTROLS_CANBUS_CANBUS_H_ */
