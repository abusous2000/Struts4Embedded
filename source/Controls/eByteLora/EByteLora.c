/*
 * eByteLora.c
 *
 * Inspired by Kris Kasprzak's Arduino library located at
 * https://github.com/KrisKasprzak/EBYTE
 *
 */

#include "Strust4EmbeddedConf.h"
#if S4E_USE_EBYTE_LORA != 0

#include "ch.h"
#include "hal.h"
#include "string.h"
#include "dbgtrace.h"
#include "EByteLora.h"
#include "ccportab.h"


static EByteParams_TypeDef 	    eByteParams = {0};
static const SerialConfig eBytecfg = {
  9600,
  0,
  USART_CR2_STOP1_BITS,
  0
};

static bool 				enableEbyte 			   = true;

static EByteParams_TypeDef  eByteDefaultParams = {._AddressHigh	=DEFAULT_ADDRESS_HIGH,
										   ._AddressLow		=DEFAULT_ADDRESS_LOW,
										   ._Channel		=DEFAULT_CHANNEL,
										   ._OptionTrans	=DEFAULT_OPTION_FIXED_TRANS,
										   ._AirDataRate	=DEFAULT_AIR_DATA_RATE,
										   ._UARTDataRate	=DEFAULT_UART_DATA_RATE,
										   ._ParityBit		=DEFAULT_PARITY_BIT,
                                           ._OptionPullup	=DEFAULT_OPTION_PULLUP,
                                           ._OptionWakeup	=DEFAULT_OPTION_WAKEUP,
										   ._OptionFEC		=DEFAULT_OPTION_FEC,
										   ._OptionPower	=DEFAULT_OPTION_POWER,
										   ._Address = 0, ._Version = 0, ._Features = 0, ._buf = 0, ._Speed =0, ._Options = 0, ._Save = 0};
static void eByteWriteTrippleCMD(uint8_t cmd){
	uint8_t CMD[3] = {cmd, cmd, cmd};
    sdWriteTimeout(&PORTAB_EBYTE_LORA_SD, (uint8_t*)CMD, sizeof(CMD), TIME_MS2I(DEFAULT_UART_TIMEOUT));
}

static uint16_t eByteGetAddress(void) {
	return (eByteParams._AddressHigh << 8) | (eByteParams._AddressLow );
}

static void eByteBuildSpeedByte(EByteParams_TypeDef *pLoraParams) {
	pLoraParams->_Speed = 0;
	pLoraParams->_Speed =   ((pLoraParams->_ParityBit & 0xFF) << 6) | ((pLoraParams->_UARTDataRate & 0xFF) << 3) |
			                (pLoraParams->_AirDataRate & 0xFF);
}
static void eByteBuildOptionByte(EByteParams_TypeDef *pLoraParams) {
	pLoraParams->_Options = 0;
	pLoraParams->_Options = ((pLoraParams->_OptionTrans & 0xFF) << 7)  | ((pLoraParams->_OptionPullup & 0xFF) << 6) |
			                ((pLoraParams->_OptionWakeup & 0xFF) << 3) | ((pLoraParams->_OptionFEC    & 0xFF) << 2) |
							 (pLoraParams->_OptionPower&0b11);
}


static void eByteClearBuffer(void){
	while (true){
		msg_t msg = sdGetTimeout(&PORTAB_EBYTE_LORA_SD, TIME_MS2I(4));

		if ( msg == MSG_TIMEOUT || msg == MSG_RESET)
			break;
	}
}

static void eByteSetMode(uint8_t mode) {
	// data sheet claims module needs some extra time after mode setting (2ms)
	// most of my projects uses 10 ms, but 40ms is safer

	chThdSleepMilliseconds(PIN_RECOVER);
	switch(mode){
		case MODE_NORMAL:
			palClearLine(EBYTE_LORA_M0);
			palClearLine(EBYTE_LORA_M1);
			break;
		case MODE_WAKEUP:
			palSetLine(EBYTE_LORA_M0);
			palClearLine(EBYTE_LORA_M1);
			break;
		case MODE_POWERDOWN:
			palClearLine(EBYTE_LORA_M0);
			palSetLine(EBYTE_LORA_M1);
			break;
		case MODE_PROGRAM:
			palSetLine(EBYTE_LORA_M0);
			palSetLine(EBYTE_LORA_M1);
			break;
	}
	// data sheet says 2ms later control is returned, let's give just a bit more time
	// these modules can take time to activate pins
	chThdSleepMilliseconds(PIN_RECOVER);
}
static void eByteCompleteTask(unsigned long timeout) {
	systime_t startTime = chVTGetSystemTime();
#ifdef EBYTE_LORA_AUX
	while (palReadLine(EBYTE_LORA_AUX) == PAL_LOW) {
		if (TIME_I2MS(chVTGetSystemTimeX() - startTime)  > timeout)
			break;
	}
#endif
   chThdSleepMilliseconds(20);
}
void eByteReset(void) {
	eByteSetMode(MODE_PROGRAM);

	chThdSleepMilliseconds(50);
	eByteWriteTrippleCMD(WRITE_RESET_MODULE);
	eByteCompleteTask(100);

	eByteSetMode(MODE_NORMAL);
}
static bool eByteReadModelData(void) {

	memset(eByteParams._Params,0,sizeof(eByteParams._Params));
	eByteSetMode(MODE_PROGRAM);

	eByteClearBuffer();
	eByteWriteTrippleCMD(READ_MODULE_VERSION);

	chThdSleepMilliseconds(5);
	sdReadTimeout(&PORTAB_EBYTE_LORA_SD, eByteParams._Params,sizeof(eByteParams._Params), TIME_MS2I(DEFAULT_UART_TIMEOUT));
	chThdSleepMilliseconds(5);

	eByteParams._Save     = eByteParams._Params[0];
	eByteParams._Model    = eByteParams._Params[1];
	eByteParams._Version  = eByteParams._Params[2];
	eByteParams._Features = eByteParams._Params[3];

	eByteSetMode(MODE_NORMAL);
	return READ_MODULE_VERSION == eByteParams._Params[0];
}
static bool eByteReadParameters(void){
	memset(eByteParams._Params,0,sizeof(eByteParams._Params));
	eByteSetMode(MODE_PROGRAM);

	eByteClearBuffer();
	chThdSleepMilliseconds(5);
	eByteWriteTrippleCMD(READ_CONFIGURATION);
	chThdSleepMilliseconds(5);
	sdReadTimeout(&PORTAB_EBYTE_LORA_SD, eByteParams._Params,sizeof(eByteParams._Params), TIME_MS2I(DEFAULT_UART_TIMEOUT));
	chThdSleepMilliseconds(5);
	eByteSetMode(MODE_NORMAL);

	eByteParams._Save 		 = eByteParams._Params[0];
	eByteParams._AddressHigh = eByteParams._Params[1];
	eByteParams._AddressLow  = eByteParams._Params[2];
	eByteParams._Speed 		 = eByteParams._Params[3];
	eByteParams._Channel 	 = eByteParams._Params[4];
	eByteParams._Options 	 = eByteParams._Params[5];

	eByteParams._Address     =  (eByteParams._AddressHigh << 8) | (eByteParams._AddressLow);
	eByteParams._ParityBit    = (eByteParams._Speed & 0XC0) >> 6;
	eByteParams._UARTDataRate = (eByteParams._Speed & 0X38) >> 3;
	eByteParams._AirDataRate  = (eByteParams._Speed & 0X07);

	eByteParams._OptionTrans  = (eByteParams._Options & 0X80) >> 7;
	eByteParams._OptionPullup = (eByteParams._Options & 0X40) >> 6;
	eByteParams._OptionWakeup = (eByteParams._Options & 0X38) >> 3;
	eByteParams._OptionFEC    = (eByteParams._Options & 0X07) >> 2;
	eByteParams._OptionPower  = (eByteParams._Options & 0X03);


	return WRITE_CFG_PWR_DWN_SAVE == eByteParams._Params[0];
}
static void eByteSaveParameters(uint8_t val, EByteParams_TypeDef *pEByteDefaultParams) {

	eByteBuildSpeedByte(pEByteDefaultParams);
	eByteBuildOptionByte(pEByteDefaultParams);
	eByteSetMode(MODE_PROGRAM);

	eByteClearBuffer();
	chThdSleepMilliseconds(5);
	uint8_t data[6] = {val, pEByteDefaultParams->_AddressHigh, pEByteDefaultParams->_AddressLow,pEByteDefaultParams->_Speed,pEByteDefaultParams->_Channel,pEByteDefaultParams->_Options};
    sdWriteTimeout(&PORTAB_EBYTE_LORA_SD, data, sizeof(data), TIME_MS2I(DEFAULT_UART_TIMEOUT*3));

	chThdSleepMilliseconds(50);
	eByteCompleteTask(400);

	eByteSetMode(MODE_NORMAL);

	return;
}
void eBytePrintParameters(void) {
	eByteReadParameters();
	dbgprintf("----------------------------------------\r\n");
	dbgprintf("Model no.: 0x%X \r\n", eByteParams._Model);
	dbgprintf("Version  : 0x%X \r\n",eByteParams._Version);
	dbgprintf("Features : 0x%X \r\n",eByteParams._Features);
	dbgprintf("\r\n");
	dbgprintf("Mode (HEX/DEC): 0x%X/%d\r\n", eByteParams._Save, eByteParams._Save);
	dbgprintf("AddH (HEX/DEC): 0x%X/%d\r\n", eByteParams._AddressHigh, eByteParams._AddressHigh);
	dbgprintf("AddL (HEX/DEC): 0x%X/%d\r\n", eByteParams._AddressLow, eByteParams._AddressLow);
	dbgprintf("Sped (HEX/DEC): 0x%X/%d\r\n", eByteParams._Speed, eByteParams._Speed);
	dbgprintf("Chan (HEX/DEC): 0x%X/%d\r\n", eByteParams._Channel, eByteParams._Channel);
	dbgprintf("Optn (HEX/DEC): 0x%X/%d\r\n", eByteParams._Options, eByteParams._Options);
	dbgprintf("Addr (HEX/DEC): 0x%X/%d\r\n", eByteGetAddress(), eByteGetAddress());

	dbgprintf("\r\n");
	dbgprintf("SpeedParityBit (HEX/DEC)  : 0x%X/%d\r\n",eByteParams._ParityBit,eByteParams._ParityBit);
	dbgprintf("SpeedAirDataRate (HEX/DEC): 0x%X/%d\r\n",eByteParams._AirDataRate,eByteParams._AirDataRate);
	dbgprintf("OptionTrans (HEX/DEC)     : 0x%X/%d\r\n",eByteParams._OptionTrans,eByteParams._OptionTrans);
	dbgprintf("OptionPullup (HEX/DEC)    : 0x%X/%d\r\n",eByteParams._OptionPullup,eByteParams._OptionPullup);
	dbgprintf("OptionWakeup (HEX/DEC)    : 0x%X/%d\r\n",eByteParams._OptionWakeup,eByteParams._OptionWakeup);
	dbgprintf("OptionFEC (HEX/DEC)       : 0x%X/%d\r\n",eByteParams._OptionFEC,eByteParams._OptionFEC);
	dbgprintf("OptionPower (HEX/DEC)     : 0x%X/%d\r\n",eByteParams._OptionPower,eByteParams._OptionPower);
	dbgprintf("----------------------------------------\r\n");

	return;
}

bool eByteInit(void){
	bool rc = true;

	palSetLineMode(EBYTE_LORA_TX, PAL_MODE_ALTERNATE(EBYTE_LORA_AF) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_OTYPE_PUSHPULL);
	palSetLineMode(EBYTE_LORA_RX, PAL_MODE_ALTERNATE(EBYTE_LORA_AF) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_OTYPE_PUSHPULL);
	palSetLineMode(EBYTE_LORA_M0, EBYTE_LORA_MODE);
	palSetLineMode(EBYTE_LORA_M1, EBYTE_LORA_MODE);
#ifdef EBYTE_LORA_AUX
	palSetLineMode(EBYTE_LORA_AUX, PAL_MODE_INPUT);
#endif
	chThdSleepMilliseconds(5);
	sdStart(&PORTAB_EBYTE_LORA_SD, &eBytecfg);
	rc = eByteReadModelData();
//	if (rc)
		rc = eByteReadParameters();
	#if EBYTE_LORA_SAVE_PARAMS != 0
	eByteSaveParameters(PERMANENT,&eByteDefaultParams);
	eByteReset();
	chThdSleepMilliseconds(50);
	#endif
	rc = eByteReadModelData();
//	if ( rc )
		eBytePrintParameters();

	return rc;
}
static uint16_t frameID = 0;

CC_WEAK void eBytePopulateFrame(EByteLoRaFrame_TypeDef *pEByteLoRaFrame, void *pPayload, uint8_t payloadSize, uint8_t msgTypeID, EByteLoRaFrame_TypeDef	*pFromMsgPayload){
#if DEFAULT_OPTION_FIXED_TRANS != 0
	pEByteLoRaFrame->toAddressHigh   		= pFromMsgPayload !=NULL?pFromMsgPayload->fromAddressHigh :  DEFAULT_TO_ADDRESS_HIGH;
	pEByteLoRaFrame->toAddressLow    		= pFromMsgPayload !=NULL?pFromMsgPayload->fromAddressLow  :  DEFAULT_TO_ADDRESS_LOW;
	pEByteLoRaFrame->toChannel       		= pFromMsgPayload !=NULL?pFromMsgPayload->fromChannel     :  DEFAULT_TO_CHANNEL;
#endif
	pEByteLoRaFrame->fromAddressHigh 		= DEFAULT_ADDRESS_HIGH;
	pEByteLoRaFrame->fromAddressLow  		= DEFAULT_ADDRESS_LOW;
	pEByteLoRaFrame->fromChannel     		= DEFAULT_CHANNEL;
	pEByteLoRaFrame->hostID         		= EBYTE_LORA_HOST_ID;
	pEByteLoRaFrame->payloadSize     		= payloadSize;
	pEByteLoRaFrame->msgTypeID       		= msgTypeID;
	pEByteLoRaFrame->frameID          		= frameID++;

	memcpy(pEByteLoRaFrame->payload, pPayload, payloadSize);

	return;
}
void eByteSendMsg(EByteLoRaFrame_TypeDef *pEByteLoRaFrame){
    sdWriteTimeout(&PORTAB_EBYTE_LORA_SD, (uint8_t*)pEByteLoRaFrame,
    		       sizeof(EByteLoRaFrame_TypeDef)+pEByteLoRaFrame->payloadSize-EBYTE_LORA_MAX_PAYLOAD_SIZE,
				   TIME_MS2I(DEFAULT_UART_TIMEOUT));
    return;
}

CC_WEAK void eByteOnMsgReply(EByteLoRaFrame_TypeDef   *pEByteLoRaFrame){
	EByteLoRaMsgReceived_TypeDef  myLoRaMsgReceived={0};

	memcpy(&myLoRaMsgReceived, pEByteLoRaFrame->payload, pEByteLoRaFrame->payloadSize);
	dbgprintf("Host ID:%d\tAddH:%d\tAddL:%d\tChannel:%d\tMsgTypeID:%d\tConfirming FrameID:%d\r\n",
			   pEByteLoRaFrame->hostID, 	   pEByteLoRaFrame->fromAddressHigh, pEByteLoRaFrame->fromAddressLow,
			   pEByteLoRaFrame->fromChannel,   pEByteLoRaFrame->msgTypeID,       myLoRaMsgReceived.frameID );
}
void eByteProcessReplyMsgs(void){
    while ( true ){
          EByteLoRaFrame_TypeDef	myLoraFrame   = {0};
          uint8_t	*pMyLoraMsgPayload = (uint8_t*)&myLoraFrame;
          uint8_t                       *tmp               = (uint8_t*)(pMyLoraMsgPayload+FIXED_TRANS_PAD);
		  size_t size = sdReadTimeout(&PORTAB_EBYTE_LORA_SD, tmp, sizeof(myLoraFrame)-FIXED_TRANS_PAD, TIME_MS2I(EBYTE_LORA_DEFAULT_IO_TIMEOUT));
		  if ( size > 0 ){
			  if ( myLoraFrame.msgTypeID == MSG_RECEIVED_MSG_TYPE )
				  eByteOnMsgReply(&myLoraFrame);
		  }

		  if ( size <= 0 )
			  break;
     }
    return;
}
CC_WEAK size_t eByteReceiveMsg(EByteLoRaFrame_TypeDef *pEByteLoRaFrame, MyMessage_TypeDef		*pPayload){
	uint8_t* tmp = (uint8_t*)pEByteLoRaFrame;
    size_t size = sdReadTimeout(&PORTAB_EBYTE_LORA_SD, tmp + FIXED_TRANS_PAD, sizeof(EByteLoRaFrame_TypeDef)-FIXED_TRANS_PAD, TIME_MS2I(EBYTE_LORA_DEFAULT_REC_TIMEOUT));

    if ( size > 0 )
    	memcpy(pPayload, pEByteLoRaFrame->payload, pEByteLoRaFrame->payloadSize);

    return size;
}

CC_WEAK void eByteSendPeriodicMsg(void){
#if EBYTE_LORA_SEND_PERIODIC_MSG != 0
	EByteLoRaFrame_TypeDef   myLoraFrame={0};
	MyMessage_TypeDef		 myPayload = {0};

	myPayload.volume  = 45 + frameID % 10;
	myPayload.buttons = 1 << frameID % 8;
	eBytePopulateFrame(&myLoraFrame,&myPayload,sizeof(myPayload),POT_JOYSTICK_MSG_TYPE,NULL);
	eByteSendMsg(&myLoraFrame);
	#ifdef LINE_BUTTON
	if (!palReadLine(LINE_BUTTON)) {
		dbgprintf("FrameID:%d\tVolume:%d\tButtons:%d\r\n", frameID, myPayload.volume, myPayload.buttons);
		eBytePrintParameters();
	}
	#endif
#endif
	return;
}
void eByteSendAckMsg(EByteLoRaFrame_TypeDef	*pEByteLoRaFrame){
	EByteLoRaFrame_TypeDef   		myLoraFrame = {0};
	EByteLoRaMsgReceived_TypeDef	myPayload 	= {0};

	myPayload.frameID =  pEByteLoRaFrame->frameID;
	eBytePopulateFrame(&myLoraFrame,&myPayload,sizeof(myPayload),MSG_RECEIVED_MSG_TYPE,pEByteLoRaFrame);
	eByteSendMsg(&myLoraFrame);

	return;
}

CC_WEAK void eByteProcessReceivedMsg(EByteLoRaFrame_TypeDef	*pEByteLoRaFrame, MyMessage_TypeDef *pMyPayload){(void)pMyPayload;
	#ifdef LINE_LED_RED
  	palToggleLine(LINE_LED_RED);
	#endif

	#if EBYTE_LORA_SEND_PERIODIC_MSG != 0
  	dbgprintf("FrameID:%d\tHostID:%d\tAddH:%d\tAddL:%d\tChannel:%d\tMsgTypeId:%d\tVolume:%d\tButtons:%d\r\n",
  			  pEByteLoRaFrame->frameID,  	pEByteLoRaFrame->hostID,     pEByteLoRaFrame->fromAddressHigh, 	pEByteLoRaFrame->fromAddressLow,
			  pEByteLoRaFrame->fromChannel, pEByteLoRaFrame->msgTypeID,  pMyPayload->volume,          		pMyPayload->buttons);
	#else
  	dbgprintf("FrameID:%d\tHostID:%d\tAddH:%d\tAddL:%d\tChannel:%d\tMsgTypeId:%d\r\n",
  			  pEByteLoRaFrame->frameID,     	pEByteLoRaFrame->hostID,      pEByteLoRaFrame->fromAddressHigh,
			  pEByteLoRaFrame->fromAddressLow, 	pEByteLoRaFrame->fromChannel, pEByteLoRaFrame->msgTypeID);
    #endif
  	eByteSendAckMsg(pEByteLoRaFrame);

	return;
}

void toggleEnableDisableEByteLora(void){
	enableEbyteThd(enableEbyte?false:true);
}
void enableEbyteThd(bool enable){
	enableEbyte = enable;
	dbgprintf("Setting enableEbyte: %d\r\n", enable);
}
static THD_WORKING_AREA(waEByteLoraThread, EBYTE_LORA_THD_STACK_SIZE);
THD_FUNCTION(eByteLoraThread, arg) {(void)arg;
	chRegSetThreadName("eByteLoraThd");
    eByteInit();
	while (true) {

		if ( enableEbyte == false ){
			dbgprintf("eByte Lora Thd disabled\r\n");
			chThdSleepSeconds(120);
			continue;
		}
		#if EBYTE_LORA_SERVER != 0
			#if EBYTE_LORA_SEND_PERIODIC_MSG != 0
			eByteSendPeriodicMsg();
			#endif
			eByteProcessReplyMsgs();
			#if EBYTE_LORA_SEND_PERIODIC_MSG != 0
			chThdSleepMilliseconds(EBYTE_LORA_SLEEP_MS_BTWN_MSGS);
			#endif
		#else
			EByteLoRaFrame_TypeDef   myLoraMsgPayload = {0};
			MyMessage_TypeDef		      myPayload        = {0};

			size_t size = eByteReceiveMsg(&myLoraMsgPayload, &myPayload);
			if ( size > 0 )
				eByteProcessReceivedMsg(&myLoraMsgPayload,&myPayload);
			#ifdef LINE_BUTTON
			if (palReadLine(LINE_BUTTON))
			  eBytePrintParameters();
			#endif
		#endif
	}

	return;
}

void initEByteLoraThread(void){
    chThdCreateStatic(waEByteLoraThread,  sizeof(waEByteLoraThread),  NORMALPRIO+2,     eByteLoraThread, NULL);
}

#endif

