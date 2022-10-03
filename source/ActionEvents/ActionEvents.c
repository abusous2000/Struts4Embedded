#include "Strust4EmbeddedConf.h"
#include "ch.h"
#include "hal.h"
#include "ActionEvents.h"
#include "Strust4Embedded.h"
#include "P9813_RGB_Driver.h"
#include "PWM.h"
#include "PPMFrameDecoder.h"
#include "EByteLora.h"
#include "RTCHelper.h"
#include "ssd1306.h"
#include "ButtonLEDs.h"
#include "CanBus.h"
#include "SDCard.h"
#if INCLUDE_SEGGER_JLINK != 0
#include "SEGGER_SYSVIEW_ChibiOS.h"
#include "SEGGER_RTT_streams.h"
#endif

static int8_t 		mute 	   			= 0;
static int8_t       pause               = 0;
static int8_t 		volume     			= 80;

#if S4E_USE_EBYTE_LORA != 0

void eByteOnMsgReply(EByteLoRaFrame_TypeDef   *pEByteLoRaFrame){
	EByteLoRaMsgReceived_TypeDef  myLoRaMsgReceived={0};

	memcpy(&myLoRaMsgReceived, pEByteLoRaFrame->payload, pEByteLoRaFrame->payloadSize);
	dbgprintf("Host ID:%d\tAddH:%d\tAddL:%d\tChannel:%d\tMsgTypeID:%d\tConfirming FrameID:%d\r\n",
			   pEByteLoRaFrame->hostID, 	   pEByteLoRaFrame->fromAddressHigh, pEByteLoRaFrame->fromAddressLow,
			   pEByteLoRaFrame->fromChannel,   pEByteLoRaFrame->msgTypeID,       myLoRaMsgReceived.frameID );

	#if S4E_USE_SSD1306_LCD != 0
	   char  buff[20]={0};
	   chsnprintf(buff, sizeof(buff),"Frame:%d Confirmed",myLoRaMsgReceived.frameID );
	   LCD_Display_String(buff,52, false);

	   LCD_Display_Update();
	#endif

}
void eByteProcessReceivedMsg(EByteLoRaFrame_TypeDef	*pEByteLoRaFrame, MyMessage_TypeDef *pMyPayload){
  	dbgprintf("+++FrameID:%d\tHostID:%d\tAddH:%d\tAddL:%d\tChannel:%d\tMsgTypeId:%d\tVolume:%d\tButtons:%d\r\n",
  			  pEByteLoRaFrame->frameID,  	pEByteLoRaFrame->hostID,     pEByteLoRaFrame->fromAddressHigh, 	pEByteLoRaFrame->fromAddressLow,
			  pEByteLoRaFrame->fromChannel, pEByteLoRaFrame->msgTypeID,  pMyPayload->volume,          		pMyPayload->buttons);
  	if ( pMyPayload->volume )
			triggerActionEvent(SET_VOLUME_AE_NAME,NULL,pMyPayload->volume,SOURCE_EVENT_LORA);
  	if ( pMyPayload->buttons ){
  		switch(pMyPayload->buttons ){
			case TOGGLE_MUTE_BUTTON:
				triggerActionEvent(TOGGLE_MUTE_AE_NAME,NULL,pMyPayload->buttons,SOURCE_EVENT_LORA);
				break;
			case TOGGLE_PAUSE_BUTTON:
				triggerActionEvent(TOGGLE_PAUSE_AE_NAME,NULL,pMyPayload->buttons,SOURCE_EVENT_LORA);
				break;
			case NEXT_TRACK_BUTTON:
				triggerActionEvent(NEXT_TRACK_AE_NAME,NULL,pMyPayload->buttons,SOURCE_EVENT_LORA);
				break;
			case PREV_TRACK_BUTTON:
				triggerActionEvent(NEXT_TRACK_AE_NAME,NULL,pMyPayload->buttons,SOURCE_EVENT_LORA);
				break;
			case VOLUME_UP_BUTTON:
				triggerActionEvent(VOLUME_UP_AE_NAME,NULL,pMyPayload->buttons,SOURCE_EVENT_LORA);
				break;
			case VOLUME_DOWN_BUTTON:
				triggerActionEvent(VOLUME_DOWN_AE_NAME,NULL,pMyPayload->buttons,SOURCE_EVENT_LORA);
				break;
  		}
   	}

  	eByteSendAckMsg(pEByteLoRaFrame);
#ifdef LINE_LED_BLUE
  	pBlueLedPAL->toggle(pBlueLedPAL);
#endif

	return;
}
#endif
int8_t isPaused(void){
	return pause;
}
int8_t getCurrentVolume(void){
	return volume;
}
int8_t getCurrentMute(void){
	return mute;
}
static int32_t loadSysProperties(ActionEvent_Typedef 	*pActionEvent){(void)pActionEvent;
#if S4E_USE_WIFI_MODULE_THD > 0
	sendGetPropertiesMsgToWifiModule(PROPERTIES_HOST,PROPERTIES_URL,PROPERTIES_FP);
#endif
   return MSG_OK;
}
static int32_t updateWifiHtml(ActionEvent_Typedef 	*pActionEvent){(void)pActionEvent;
#if S4E_USE_WIFI_MODULE_THD > 0
   initWifiModuleServer();
#endif
   return MSG_OK;
}
#if EBYTE_LORA_SERVER != 0
void eByteLoraSendFrame(int8_t buttonPressed){
	EByteLoRaFrame_TypeDef   myLoraFrame={0};
	MyMessage_TypeDef		 myPayload = {0};

	myPayload.volume  = volume;
	myPayload.buttons = buttonPressed?1 << buttonPressed:0;
	eBytePopulateFrame(&myLoraFrame,&myPayload,sizeof(myPayload),POT_JOYSTICK_MSG_TYPE,NULL);
	eByteSendMsg(&myLoraFrame);
	dbgprintf("FrameID:%d\tVolume:%d\tButtons:%0xX\r\n", myLoraFrame.frameID, myPayload.volume, myPayload.buttons);

}
#endif
static int32_t toggleModule(ActionEvent_Typedef 	*pActionEvent){(void)pActionEvent;

  switch(pActionEvent->u.data){
	  case 1:
          #if S4E_USE_PPM_FRAME_DECODER != 0
		  dbgprintf("toggleEnableDisablePPMDecoder\r\n");
		  toggleDebugPPMDecoder();
          #endif
		  break;
	  case 2:
          #if S4E_USE_PPM_FRAME_DECODER != 0
		  dbgprintf("toggleDebugPPMDecoder\r\n");
		  toggleDebugPPMDecoder();
          #endif
		  break;
     case 3:
          #if S4E_USE_EBYTE_LORA != 0
		  dbgprintf("toggleEnableDisableeByteLora\r\n" );
		  toggleEnableDisableEByteLora();
          #endif
		  break;
	  default:
		dbgprintf("toggleModule module ignored AE:%d\r\n", pActionEvent->u.data);
		break;
  }

  return MSG_OK;
}

static int32_t toggleRelay(ActionEvent_Typedef 	*pActionEvent){(void)pActionEvent;
#ifdef LINE_RELAY
      pRelayPAL->toggle(pRelayPAL);
#endif
	dbgprintf("toggleRelay\r\n", pActionEvent->u.data);

	return MSG_OK;
}

static int32_t toggleMute(ActionEvent_Typedef 	*pActionEvent){(void)pActionEvent;
  mute= !mute;

#if EBYTE_LORA_SERVER != 0
  eByteLoraSendFrame(TOGGLE_MUTE_BUTTON);
#endif
  return MSG_OK;
}

static int32_t togglePausePlay(ActionEvent_Typedef 	*pActionEvent){(void)pActionEvent;
#if EBYTE_LORA_SERVER != 0
  eByteLoraSendFrame(TOGGLE_PAUSE_BUTTON);
#endif
  getHeapUsageInfo();
  if ( pause == 0 ){
 	 dbgprintf("***Pausing....\r\n");
	 pause = 1;
  }
  else{
	 dbgprintf("***Resuming....\r\n");
	 pause = 0;
  }
  return MSG_OK;
}
void updateDutyCycleBasedOnVolume(void){
#if S4E_USE_PWM > 0
	double dc= ((double)volume/100.0f);
	setPWMDutyCycle(TIM_FREQ*dc,TIM_FREQ*dc);
	dbgprintf("New Duty Cycle:%f\r\n",dc);
#endif
}

static int32_t setVolume(ActionEvent_Typedef   *pActionEvent){(void)pActionEvent;
//  SEGGER_SYSVIEW_RecordString(23, "setVolumeStart");
  volume = pActionEvent->dataType==CHAR_DTYPE? atoi(pActionEvent->u.pData): (int8_t)pActionEvent->u.data;
#if EBYTE_LORA_SERVER != 0
  eByteLoraSendFrame(NONE_BUTTON);
#endif

  updateDutyCycleBasedOnVolume();
//  SEGGER_SYSVIEW_RecordString(24, "setVolumeStop");
  #if INCLUDE_SEGGER_JLINK != 0
  SEGGER_SYSVIEW_PrintfHost("setVolumeStart: %d\r\n", volume);
  #endif

  return MSG_OK;
}

static int32_t volumeDown(ActionEvent_Typedef 	*pActionEvent){(void)pActionEvent;
   volume -= 5;
   volume = volume<0?0:volume;
   updateDutyCycleBasedOnVolume();
#if EBYTE_LORA_SERVER != 0
  eByteLoraSendFrame(VOLUME_DOWN_BUTTON);
#endif
  return MSG_OK;
}
static int32_t volumeUp(ActionEvent_Typedef 	*pActionEvent){(void)pActionEvent;
   volume += 5;
   volume = volume>100?100:volume;

   updateDutyCycleBasedOnVolume();
#if EBYTE_LORA_SERVER != 0
  eByteLoraSendFrame(VOLUME_UP_BUTTON);
#endif
  return MSG_OK;
}

static int32_t nextTrack(ActionEvent_Typedef 	*pActionEvent){(void)pActionEvent;
   dbgprintf("Next track\r\n");
#if EBYTE_LORA_SERVER != 0
  eByteLoraSendFrame(NEXT_TRACK_BUTTON);
#endif
  return MSG_OK;
}

int32_t setRGBLED(ActionEvent_Typedef 	*pActionEvent){(void)pActionEvent;
#if S4E_USE_RGB == 1
   if ( pActionEvent->u.pData != NULL){
	   char     temp[ACTION_EVENT_DATA_MAX_SIZE];
	   char     *context    = NULL;

	   strlcpy(temp,pActionEvent->u.pData,ACTION_EVENT_DATA_MAX_SIZE);

       char *strRed 	= strtok_r(temp,"|",&context);
       char *strGreen 	= strtok_r(NULL,"|",&context);
       char *strBlue 	= strtok_r(NULL,"|",&context);

	   uint8_t  red 	= strRed!=NULL?  atoi(strRed):  0;
	   uint8_t  green	= strGreen!=NULL?atoi(strGreen):0;
	   uint8_t  blue 	= strBlue!=NULL? atoi(strBlue): 0;
	   setColorRGB(0, red, green, blue);
   }
   else{
	   dbgprintf("No data to set RGB LED\r\n");
	   return MSG_RESET;
   }
#else
   dbgprintf("RGB LED not supported in this build\r\n");
#endif
   return MSG_OK;
}

static int32_t setPWMParams(ActionEvent_Typedef 	*pActionEvent){(void)pActionEvent;
#if S4E_USE_PWM == 1
	if ( pActionEvent->u.pData != NULL){
	   char     temp[ACTION_EVENT_DATA_MAX_SIZE];
	   char     *context    = NULL;

	   strlcpy(temp,pActionEvent->u.pData,DATA_FIELD_WAS_LT_MAX);
	   char *frequency			 	= strtok_r((char *)temp,"|",&context);
	   char *period 				= strtok_r(NULL,"|",&context);
	   char *firstChannelDutyCycle 	= strtok_r(NULL,"|",&context);
	   char *secondChannelDutyCycle = strtok_r(NULL,"|",&context);

	   uint32_t iFrequency 	= atoi(frequency);
	   uint32_t iPeriod     = atoi(period);
       double  dc1 	        = firstChannelDutyCycle!=NULL?atof(firstChannelDutyCycle):0;
       double  dc2 	        = secondChannelDutyCycle!=NULL?atof(secondChannelDutyCycle):0;

       if ( dc1 > 0.0f && dc1 < 1.0f && dc2 > 0.0f && dc2 < 1.0f)
    	   reInitPWM(iFrequency,iPeriod,dc1,dc2);
       else
    	   dbgprintf("Duty Cycles out of range:%d-%d\r\n",dc1,dc2);
   }
   else{
	   dbgprintf("No data to set PWM Duty Cycle\r\n");
	   return MSG_RESET;
   }
#else
   dbgprintf("PWM LED not supported in this build\r\n");
#endif

   return MSG_OK;
}
#if S4E_USE_PPM_FRAME_DECODER != 0
static uint8_t  				lastCh3Value = 0;
static ButtonStats_Typedef  	rcSWA = BUTTON_STATE_UNKNOWN;
static ButtonStats_Typedef  	rcSWB = BUTTON_STATE_UNKNOWN;

void onChannelPPMValueChange(uint8_t ch, PPM_FRAME_TYPDEF   *pCurrentPPMFrame, PPM_FRAME_TYPDEF   *pLastPPMFrame ){
	ButtonStats_Typedef buttonStatus;
    uint8_t    toggleLED = 0;
    uint8_t    printDebugInfo = 0;
    uint8_t    currentValue = pCurrentPPMFrame->valueInCycles[ch];
    uint8_t    newValue = pLastPPMFrame->valueInCycles[ch];
	switch(ch){
		case RC_CH3:{
			uint32_t  currentValue =  100 *(newValue-RC_BUTTON_MIN_VALUE)/(RC_BUTTON_MAX_VALUE - RC_BUTTON_MIN_VALUE);
			int8_t   delta         = currentValue- lastCh3Value;

			if ( delta > 2 ){
				triggerActionEvent(SET_VOLUME_AE_NAME,NULL,currentValue,"RC");
				toggleLED = 1;
			}
			printDebugInfo =1;
		}
		break;
		case RC_SWA:
			buttonStatus = getRCButtonStatus(newValue);
			if ( buttonStatus != BUTTON_STATE_UNKNOWN && rcSWA != buttonStatus ){
				toggleLED = 1;
				triggerActionEvent(TOGGLE_MUTE_AE_NAME,NULL,buttonStatus,"RC");
				rcSWA = buttonStatus;
			}
			printDebugInfo =1;
			break;
		case RC_SWB:
			buttonStatus = getRCButtonStatus(newValue);
			if ( buttonStatus != BUTTON_STATE_UNKNOWN && rcSWB != buttonStatus){
				toggleLED = 1;
				//triggerActionEvent(TOGGLE_PAUSE_AE_NAME,NULL,buttonStatus,"RC");
				triggerActionEvent(TOGGLE_BUZZER_AE_NAME,NULL,buttonStatus,"RC");
				rcSWB = buttonStatus;
			}
			printDebugInfo =1;
			break;
	}
	if ( printDebugInfo )
		printPPMValueChange(ch,pCurrentPPMFrame,pLastPPMFrame);

#ifdef LINE_LED_RED
	if ( toggleLED )
  	    pRedLedPAL->toggle(pRedLedPAL);
#endif

}
#endif
static int32_t newHTMLLoaded(ActionEvent_Typedef 	*pActionEvent){(void)pActionEvent;
    dbgprintf("newHTMLLoaded\r\n");
#if S4E_USE_RGB > 0
    setColorRGB(0, 0, 255, 0);
    //A good example by trigger another AE from inside another, but it will be executed asynchronously
    triggerActionEvent(SET_RGB_LED_AE_NAME,"0|0|0",0,NEW_HTML_LOADED_AE_NAME);
#else
   dbgprintf("RGB LED not supported in this build\r\n");
#endif
   return MSG_OK;
}

static int32_t performanceInfo(ActionEvent_Typedef 	*pActionEvent){(void)pActionEvent;
   getHeapUsageInfo();

   return MSG_OK;
}
static int32_t setUnixtime(ActionEvent_Typedef 	*pActionEvent){
	#if HAL_USE_RTC != 0
	if ( pActionEvent->u.pData != NULL){
		rtcSetTimeUnixSecFromString(pActionEvent->u.pData);
		dbgprintf("New RTC Date:%s", rtcGetTimeAsString());
	}
#else
	(void)pActionEvent;
	#endif

   return MSG_OK;
}
static int32_t goToSleep(ActionEvent_Typedef 	*pActionEvent){(void)pActionEvent;
	#if HAL_USE_RTC != 0
	rtcGoToSleep();
	#endif

   return MSG_OK;
}

static int32_t toggleBuzzer(ActionEvent_Typedef 	*pActionEvent){(void)pActionEvent;
	#if S4E_USE_BUZZER != 0
    pBuzzer->toggle(pBuzzer);
	#endif

   return MSG_OK;
}

static int32_t testSDCard(ActionEvent_Typedef 	*pActionEvent){(void)pActionEvent;
	#if S4E_USE_SDCARD != 0
	SDCardDriverITF_Typedef *pSDCardDriverITF 			= getSDCardDriver();

	pSDCardDriverITF->init(pSDCardDriverITF);
	if ( !pSDCardDriverITF->mount(pSDCardDriverITF))
		pSDCardDriverITF->processFiles(pSDCardDriverITF);
	else
		dbgprintf("Failed to mount SD card\r\n");
	#endif

   return MSG_OK;
}

static int32_t canBusControlAE(ActionEvent_Typedef 	*pActionEvent){(void)pActionEvent;
   char     *context    = NULL;
   char     temp[ACTION_EVENT_DATA_MAX_SIZE];

   #if S4E_USE_CAN_BUS != 0
   strlcpy(temp,pActionEvent->u.pData,ACTION_EVENT_DATA_MAX_SIZE);

   char     *actionId 	= strtok_r(temp,"#",&context);
   char     *pData 	    = strtok_r(NULL,"#",&context);

   canBusControl(atoi(actionId),pData);
   #endif
   return MSG_OK;
}


///MSGFormat:XXX#<LEN><Data>
static int32_t sendCanBusMsg(ActionEvent_Typedef 	*pActionEvent){(void)pActionEvent;
    #if S4E_USE_CAN_BUS != 0
	if ( pActionEvent->u.pData != NULL){
		char temp[ACTION_EVENT_DATA_MAX_SIZE];
		char *context    = NULL;

		strlcpy(temp,pActionEvent->u.pData,ACTION_EVENT_DATA_MAX_SIZE);
		char     *strMsgId 	= strtok_r(temp,"#",&context);
		char     *pData 	= strtok_r(NULL,"#",&context);
		uint8_t  len        = (uint8_t)(*pData)-'0';
		uint8_t  *pData8    = (uint8_t*)(pData+1);

		sendCanMsg((uint32_t)strtol(strMsgId, NULL, 16),len,pData8);
	}
	else{
		   dbgprintf("No data to send CanBus Msg\r\n");
		   return MSG_RESET;
	}
    #endif
   return MSG_OK;
}

static ActionEvent_Typedef actionEventToggleMute 	 	= {.name=TOGGLE_MUTE_AE_NAME,  			.eventSource="Center",      	.action=toggleMute,			.view=toggleMuteView,		.dataType = INT_DTYPE};
static ActionEvent_Typedef actionEventNextTrack  	 	= {.name=NEXT_TRACK_AE_NAME,			.eventSource="Up",          	.action=nextTrack,          							.dataType = INT_DTYPE};
static ActionEvent_Typedef actionEventTogglePausePlay	= {.name=TOGGLE_PAUSE_AE_NAME,			.eventSource="Down",        	.action=togglePausePlay};
static ActionEvent_Typedef actionEventVolumeDown   	 	= {.name=VOLUME_DOWN_AE_NAME,			.eventSource="Left",        	.action=volumeDown,         .view=volumeView,			.dataType = INT_DTYPE};
static ActionEvent_Typedef actionEventVolumeUp       	= {.name=VOLUME_UP_AE_NAME,				.eventSource="Right",       	.action=volumeUp,			.view=volumeView,			.dataType = INT_DTYPE};
static ActionEvent_Typedef actionEventSetVolume      	= {.name=SET_VOLUME_AE_NAME,			.eventSource="setVolume",   	.action=setVolume, 			.view=volumeView,			.dataType = INT_DTYPE};
static ActionEvent_Typedef actionEventUpdateWifiHTML 	= {.name=UPDATE_WIFI_HTML_AE_NAME,		.eventSource="updateWifiHtml",  .action=updateWifiHtml,     .view=updateWifiHtmlView,	.dataType = INT_DTYPE};
static ActionEvent_Typedef actionEventLoadSysProperties = {.name=LOAD_SYS_PROPERTIES_AE_NAME,	.eventSource="wifi",            .action=loadSysProperties,  .view=loadSysPropertiesView,.dataType = INT_DTYPE};
static ActionEvent_Typedef actionEventSetRGBLED         = {.name=SET_RGB_LED_AE_NAME,			.eventSource="wifi",            .action=setRGBLED, 			.view=setRGBLEDView,		.dataType = INT_DTYPE};
static ActionEvent_Typedef actionEventSetMototsParams   = {.name=SET_MOTORS_PARAMS_AE_NAME,	    .eventSource="wifi",            .action=setPWMParams, 		.view=setPWMParamsView,		.dataType = INT_DTYPE};
static ActionEvent_Typedef actionEventNewHTMLLoaded	 	= {.name=NEW_HTML_LOADED_AE_NAME,  		.eventSource="wifi",      	    .action=newHTMLLoaded, 	    .view=NULL,				    .dataType = INT_DTYPE};
static ActionEvent_Typedef actionEventPerformanceInfo 	= {.name=PERFORMANCE_INFO_AE_NAME,  	.eventSource="wifi",      	    .action=performanceInfo, 	.view=NULL,				    .dataType = INT_DTYPE};
static ActionEvent_Typedef actionEventSetUnixtime      	= {.name=SET_UNIX_TIME_AE_NAME,			.eventSource="WiFi",   		    .action=setUnixtime, 		.view=NULL,			        .dataType = CHAR_DTYPE};
static ActionEvent_Typedef actionEventGoToSleep      	= {.name=GO_TO_SLEEP_AE_NAME,			.eventSource="WiFi",   		    .action=goToSleep, 		    .view=NULL,			        .dataType = INT_DTYPE};
static ActionEvent_Typedef actionEventToggleBuzzer     	= {.name=TOGGLE_BUZZER_AE_NAME,			.eventSource="WiFi",   		    .action=toggleBuzzer, 		.view=NULL,			        .dataType = INT_DTYPE};
static ActionEvent_Typedef actionEventTestSDCard     	= {.name=TEST_SDCARD,			        .eventSource="WiFi",   		    .action=testSDCard, 		.view=NULL,			        .dataType = INT_DTYPE};
static ActionEvent_Typedef actionEventCanBusControl    	= {.name=CAN_BUS_CONTROL_AE_NAME,		.eventSource="WiFi",   		    .action=canBusControlAE, 	.view=NULL,			        .dataType = CHAR_DTYPE};
static ActionEvent_Typedef actionEventCanBusSendMsg    	= {.name=CAN_BUS_SEND_MSG_AE_NAME,		.eventSource="WiFi",   		    .action=sendCanBusMsg, 		.view=NULL,			        .dataType = CHAR_DTYPE};
static ActionEvent_Typedef actionEventToggleModule      = {.name=TOGGLE_ENABLE_MODULE_AE_NAME,	.eventSource="WiFi",   		    .action=toggleModule, 		.view=NULL,			        .dataType = INT_DTYPE};
static ActionEvent_Typedef actionEventToggleRelay       = {.name=TOGGLE_RELAY_MODULE_AE_NAME,	.eventSource="WiFi",   		    .action=toggleRelay, 		.view=NULL,			        .dataType = INT_DTYPE};


ActionEvent_Typedef *gActionEvents[MAX_ACTION_EVENTS] ={&actionEventToggleMute,
		                                                &actionEventNextTrack,
														&actionEventTogglePausePlay,
														&actionEventVolumeDown,
														&actionEventVolumeUp,
                                                        &actionEventSetVolume,
                                                        &actionEventUpdateWifiHTML,
                                                        &actionEventLoadSysProperties,
                                                        &actionEventSetRGBLED,
														&actionEventSetMototsParams,
														&actionEventNewHTMLLoaded,
														&actionEventPerformanceInfo,
														&actionEventSetUnixtime,
														&actionEventGoToSleep,
                                                        &actionEventToggleBuzzer,
                                                        &actionEventTestSDCard,
														&actionEventCanBusControl,
														&actionEventCanBusSendMsg,
														&actionEventToggleModule,
                                                        &actionEventToggleRelay};

