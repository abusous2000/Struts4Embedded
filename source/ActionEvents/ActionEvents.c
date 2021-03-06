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
#ifdef LINE_LED_RED
	palToggleLine(LINE_LED_RED);
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
  volume = pActionEvent->dataType==CHAR_DTYPE? atoi(pActionEvent->u.pData): (int8_t)pActionEvent->u.data;
#if EBYTE_LORA_SERVER != 0
  eByteLoraSendFrame(NONE_BUTTON);
#endif

  updateDutyCycleBasedOnVolume();

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
	   char temp[DATA_FIELD_WAS_LT_MAX];
	   strlcpy(temp,pActionEvent->u.pData,DATA_FIELD_WAS_LT_MAX);
       char *strRed 	= strtok(temp,"|");
       char *strGreen 	= strtok(NULL,"|");
       char *strBlue 	= strtok(NULL,"|");

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
	   char temp[DATA_FIELD_WAS_LT_MAX];
	   strlcpy(temp,pActionEvent->u.pData,DATA_FIELD_WAS_LT_MAX);
	   char *frequency			 	= strtok((char *)temp,"|");
	   char *period 				= strtok(NULL,"|");
	   char *firstChannelDutyCycle 	= strtok(NULL,"|");
	   char *secondChannelDutyCycle = strtok(NULL,"|");

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
void onChannelPPMValueChange (uint8_t ch, uint8_t currentValue, uint8_t newValue){
	ButtonStats_Typedef buttonStatus;

	dbgprintf("OnChangeChannelValue: %d\t%d\t%d\r\n", ch, currentValue, newValue);
	switch(ch){
		case RC_CH3:{
			uint32_t  currentValue =  100 *(newValue-RC_MIN_VALUE)/(RC_MAX_VALUE - RC_MIN_VALUE);
			int8_t   delta         = currentValue- lastCh3Value;

			if ( delta > 2 )
				triggerActionEvent(SET_VOLUME_AE_NAME,NULL,currentValue,"RC");
		}
		break;

		case RC_SWB:
			buttonStatus = getRCButtonStatus(newValue);
			if ( buttonStatus != BUTTON_STATE_UNKNOWN)
				triggerActionEvent(TOGGLE_PAUSE_AE_NAME,NULL,buttonStatus,"RC");
			break;
		case RC_SWA:
			buttonStatus = getRCButtonStatus(newValue);
			if ( buttonStatus != BUTTON_STATE_UNKNOWN)
				triggerActionEvent(TOGGLE_MUTE_AE_NAME,NULL,buttonStatus,"RC");
			break;
	}

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
														&actionEventGoToSleep};

