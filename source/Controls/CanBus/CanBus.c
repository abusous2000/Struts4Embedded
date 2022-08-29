/*
 * CanBus.c
 *
 *  Created on: Nov 14, 2021
 *      Author: abusous2000
 */
/*
 * Use project the modified version of USBtinViewer to view the Can Msgs on Desktop streamed via serial
 */

#include "Strust4EmbeddedConf.h"
#if S4E_USE_CAN_BUS != 0

#include "ch.h"
#include "hal.h"
#include "string.h"
#include "stdlib.h"
#include "dbgtrace.h"
#include "ActionEvents.h"
#include "CanBus.h"
#include "ccportab.h"

static const SerialConfig canBusSerialCfg = {
  115200,
  0,
  USART_CR2_STOP1_BITS,
  0
};


struct can_instance {
  CANDriver     *canp;
  uint32_t      led;
};

static const struct can_instance can1 = {&CAND1, CAN_BUS_LED};
static const struct can_instance can2 = {&CAND2, CAN_BUS_LED};

static uint32_t data32[20] = {0x50F050F1, 0x50F050F2, 0x50F050F3, 0x50F050F4, 0x50F050F5, 0x50F050F6, 0x50F050F7, 0x50F050F8, 0x50F050F9, 0x50F05010, 0x50F05011, 0x50F05012};
static uint32_t iRandDataNdx = 0;

static uint32_t getRandomData32(void){
	if ( iRandDataNdx>= 8 )
		iRandDataNdx = 0;
	return data32[iRandDataNdx++];
}

static const CANConfig cancfg = {
  .mcr=CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
  .btr=CAN_BTR_SJW(CAN_BUS_BTR_SJW) | CAN_BTR_TS2(CAN_BUS_BTR_TS2) | CAN_BTR_TS1(CAN_BUS_BTR_TS1) | CAN_BTR_BRP(CAN_BUS_BTR_BRP)
};

uint8_t halfbyte_to_hexascii(uint8_t _halfbyte){
  _halfbyte &= 0x0F;
  if (_halfbyte >= 10)
    return ('A' + _halfbyte - 10);
  else
    return ('0' + _halfbyte);
}
uint8_t packCanFrameForUartTransmission(uint8_t *buf, CANRxFrame *pCanFrame){

	uint8_t bytes = 0;
	buf[bytes++] = 't';
	buf[bytes++] = halfbyte_to_hexascii((pCanFrame->EID) >> 8);
	buf[bytes++] = halfbyte_to_hexascii((pCanFrame->EID) >> 4);
	buf[bytes++] = halfbyte_to_hexascii((pCanFrame->EID));
	buf[bytes++] = halfbyte_to_hexascii((pCanFrame->DLC));
	for( int i = 0; i < pCanFrame->DLC; ++i ){
		  buf[bytes++] = halfbyte_to_hexascii((pCanFrame->data8[i]) >> 4);
		  buf[bytes++] = halfbyte_to_hexascii((pCanFrame->data8[i]));
	}
	buf[bytes++] = '\r';

	return bytes;
}
static uint8_t logText = CAN_BUS_LOG_TEXT;
static uint8_t streamDataToSerial  = 1;
static uint8_t playDummyCanBusData = 0;
static uint32_t cnt = 0;
static uint8_t checkCanQueue = 1;
static CanBusMonitorChange_Typedef  headLightMonitor={.msgId=0x622,.monitorByte={0,0,0,1,0,0,0,0},.actionEventToExecute=TOGGLE_BUZZER_AE_NAME};
static CanBusMonitorChange_Typedef  *arCanMsgsToMonitor[MAX_CAN_MSGS_TO_MONITOR]={&headLightMonitor};

CC_WEAK uint8_t handleCanMsg(CANRxFrame  *pRXFrame) {
	uint8_t rc = 0;
	for(uint8_t i = 0; i < MAX_CAN_MSGS_TO_MONITOR && !rc ; ++i){
		CanBusMonitorChange_Typedef  *pMonitor = arCanMsgsToMonitor[i];

		if ( pMonitor->msgId > 0 && pMonitor->msgId == pRXFrame->SID){
			if ( pMonitor->lastRXFrame.data64[0] != 0 ){
				for(uint8_t j = 0; j < pRXFrame->DLC && !rc; ++j){
					if ( pMonitor->monitorByte[j] != 0 && pMonitor->lastRXFrame.data8[j] != pRXFrame->data8[j] ){
						triggerActionEvent(pMonitor->actionEventToExecute,(char*)pRXFrame->data8,0,SOURCE_EVENT_CAN_BUS);
						rc = 1;
					}
				}
			}
			pMonitor->lastRXFrame = *pRXFrame;
		}
	}

	return rc;
}

void monitorClearCanMsg(uint8_t val) {
	int8_t cleared = 0;
	for (uint8_t i; i < MAX_CAN_MSGS_TO_MONITOR; ++i) {
		if (arCanMsgsToMonitor[i]->msgId > 0
				&& arCanMsgsToMonitor[i]->msgId == val) {
			arCanMsgsToMonitor[i]->msgId = 0;
			memset(&arCanMsgsToMonitor[i]->lastRXFrame, 0, sizeof(CANRxFrame));
			dbgprintf("Cleared monitoring can bus msg:%d#\r\n",
					arCanMsgsToMonitor[i]->msgId);
			cleared = 1;
			break;
		}
	}
	if (!cleared) {
		for (uint8_t i; i < MAX_CAN_MSGS_TO_MONITOR; ++i) {
			if (arCanMsgsToMonitor[i]->msgId == 0) {
				arCanMsgsToMonitor[i]->msgId = val;
				dbgprintf("Will monitor can bus msg:%d#\r\n",
						arCanMsgsToMonitor[i]->msgId);
				break;
			}
		}
	}
}

CC_WEAK void canBusControl(uint32_t   val, char *pData) {
	switch(val){
	case TOGGLE_CAN_BUS:
		checkCanQueue = !checkCanQueue;
		break;
	case TOGGLE_CAN_BUS_DUMMY_DATA:
		playDummyCanBusData = !playDummyCanBusData;
		break;
	case TOGGLE_CAN_BUS_STREAM_DATA:
		streamDataToSerial = !streamDataToSerial;
		break;
	case CAN_BUS_SET_FILTER:
		{
			char *a = pData;
			char *b = strstr(pData, ";");
			int position = b - a;
			if ( pData != NULL && position > 0 && position < 30 ){
				char temp[ACTION_EVENT_DATA_MAX_SIZE];
				char *context    = NULL;

				strlcpy(temp,pData,DATA_FIELD_WAS_LT_MAX);
				char     *mask1Str 	= strtok_r(temp,";",&context);
				char     *mask2Str 	= strtok_r(NULL,";",&context);

				changeFilter((uint32_t)strtol(mask1Str, NULL, 16),(uint32_t)strtol(mask2Str, NULL, 16));
			}
		}
		break;
	default:
		 if (val >= 0x100 ){
			monitorClearCanMsg(val);
		 }
		 break;
	}
}


#if CAN_BUS_START_CAN1_THD != 0
static THD_WORKING_AREA(can_rx1_wa, 1024);
#endif
#if CAN_BUS_START_CAN2_THD != 0
static THD_WORKING_AREA(can_rx2_wa, 1024);
#endif
#if CAN_BUS_START_DUMMY_MSG_SENDER != 0
static THD_WORKING_AREA(dummyDataSenderThd_wa, 1024);
#endif

THD_FUNCTION(dummyDataSenderThd, p) {(void)p;
  CANRxFrame    testMsg     =  {0};
  SerialDriver *sCandp      =  &CAN_BUS_SD;
  SerialDriver *sGeneralp   =  &PORTAB_SD;
  uint8_t      serialListen =  sCandp != sGeneralp;

  testMsg.IDE = CAN_IDE_STD;
  testMsg.EID = 0x01234567;
  testMsg.RTR = CAN_RTR_DATA;
  testMsg.DLC = 8;
  testMsg.data32[0] = 0x55AA55AA;
  testMsg.data32[1] = 0x00FF00FF;
  chRegSetThreadName("dummyDataSenderThd");

  while (true) {
	chThdSleepMilliseconds(CAN_BUD_DUMMY_THRD_SLEEP);
	if ( playDummyCanBusData ){
		uint8_t buf[40]  = {0};

		testMsg.EID = 0x01234567;
		testMsg.data32[1] = getRandomData32();
		uint8_t num_bytes = packCanFrameForUartTransmission(buf,&testMsg);
		sdWriteTimeout(&SD1, buf, num_bytes, TIME_MS2I(3000));
		if ( logText )
			dbgprintf("%d#\r\n",testMsg.SID);
		chThdSleepMilliseconds(500);
		testMsg.EID = 0x01234568;
		testMsg.data32[1] = getRandomData32();
		uint8_t num_bytes2 = packCanFrameForUartTransmission(buf,&testMsg);
		sdWriteTimeout(&SD1, buf, num_bytes2, TIME_MS2I(3000));
		if ( logText )
			dbgprintf("%d#\r\n",testMsg.SID);
	}
  }

}
static void printCanMsg(uint32_t msgId, uint8_t len, uint8_t *data) {
	dbgprintf("%x#", msgId);
	for (uint8_t i = 0; i < len; ++i){
		char c = *(data+i);
		dbgprintf("%c", c);
	}
	dbgprintf("\r");
}

THD_FUNCTION(can_rx, p) {  (void)p;
  struct can_instance *cip = p;
  event_listener_t el;
  CANRxFrame rxmsg;
  uint8_t buf[40] = {0};


#if CAN_BUS_START_CAN1_THD != 0
  if (cip->canp == (&CAND1) )
     chRegSetThreadName("receiver_can1");
  else
#endif
     chRegSetThreadName("receiver_can2");
  chEvtRegister(&cip->canp->rxfull_event, &el, 0);
  while (true) {
    if (chEvtWaitAnyTimeout(ALL_EVENTS, TIME_MS2I(100)) == 0)
      continue;
    while ( checkCanQueue && canReceive(cip->canp, CAN_ANY_MAILBOX,&rxmsg, TIME_IMMEDIATE) == MSG_OK) {
    	if ( handleCanMsg(&rxmsg) ){
            palToggleLine(cip->led);
            if ( logText )
    		  dbgprintf("Handled frame\r\n");
    	}
        uint8_t num_bytes = packCanFrameForUartTransmission(buf,&rxmsg);
        if ( streamDataToSerial )
            sdWriteTimeout(&CAN_BUS_SD, buf, num_bytes, TIME_MS2I(3000));
        if ( logText )
			printCanMsg(rxmsg.SID,rxmsg.DLC,rxmsg.data8);

    }
  }
  chEvtUnregister(&cip->canp->rxfull_event, &el);
}
static CANFilter can_filter[1] = {\
                            {.filter=14,  .mode=1, .scale=1, .assignment=0, .register1=0xC5<<21, .register2=0xC7<<21}
                          };

//when idMask1 = idMask2 = 0; the filter is cleared
CC_WEAK void changeFilter(uint32_t idMask1, uint32_t idMask2) {
  dbgprintf("Changing filter: %x-%x\r\n",idMask1, idMask2);
  can_filter[2].register1 = idMask1<<21;
  can_filter[2].register2 = idMask2<<21;
  checkCanQueue = 0;
  chThdSleepMilliseconds(250);
  checkCanQueue = 1;
  canStop(&CAND2);
  canStop(&CAND1);
  chThdSleepMilliseconds(250);
  uint8_t n =  (idMask1 == 0 && idMask2 == 0) ? 0 : 1 ;
  canSTM32SetFilters(&CAND1, 14, n, &can_filter[0]);
  canStart(&CAND1, &cancfg);
  canStart(&CAND2, &cancfg);

}
static   CANDriver     *sendCanDriver = &CAND1;
CC_WEAK void sendCanMsg(uint32_t msgId, uint8_t len, uint8_t *pData8) {
  CANTxFrame txmsg;
  txmsg.IDE = CAN_IDE_STD;
  txmsg.SID = msgId;
  txmsg.RTR = CAN_RTR_DATA;
  txmsg.DLC = len;
  for(uint8_t i = 0; i < len; ++i)
	  txmsg.data8[i] = pData8[i];
  dbgprintf("Sending msg\r\n");
  printCanMsg(txmsg.SID,txmsg.DLC,txmsg.data8);
  canTransmit(sendCanDriver, CAN_ANY_MAILBOX, &txmsg, TIME_MS2I(100));
  chThdSleepMilliseconds(10);
}

void initCanBusThreads(void){
	SerialDriver *sCandp    =   &CAN_BUS_SD;
	SerialDriver *sGeneralp =   &PORTAB_SD;

	if ( sCandp != sGeneralp)
	  sdStart(&CAN_BUS_SD, &canBusSerialCfg);

	canStart(&CAND1, &cancfg);
	canStart(&CAND2, &cancfg);
    chThdSleepMilliseconds(10);
#if CAN_BUS_START_DUMMY_MSG_SENDER != 0
    chThdCreateStatic(dummyDataSenderThd_wa,  sizeof(dummyDataSenderThd_wa),  NORMALPRIO+2,     dummyDataSenderThd, NULL);
#endif
#if CAN_BUS_START_CAN1_THD != 0
	palSetLineMode(CAN1_RX_LINE, CAN_PORT_MODE);
	palSetLineMode(CAN1_TX_LINE, CAN_PORT_MODE);
    chThdCreateStatic(can_rx1_wa, sizeof(can_rx1_wa), NORMALPRIO + 7, can_rx, (void *)&can1);
#endif
#if CAN_BUS_START_CAN2_THD != 0
	palSetLineMode(CAN2_RX_LINE, CAN_PORT_MODE);
	palSetLineMode(CAN2_TX_LINE, CAN_PORT_MODE);
    chThdCreateStatic(can_rx2_wa, sizeof(can_rx2_wa), NORMALPRIO + 7, can_rx, (void *)&can2);
#endif
}
#endif

