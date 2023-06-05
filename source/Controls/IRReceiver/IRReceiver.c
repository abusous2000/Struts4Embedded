/*
  PLAY Embedded demos - Copyright (C) 2014...2019 Rocco Marco Guglielmi

  This file is part of PLAY Embedded demos.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/
#include "Strust4EmbeddedConf.h"
#if S4E_USE_IR_RECEIVER != 0
#include "ch.h"
#include "hal.h"
#include "ActionEvents.h"
#include "IRReceiver.h"
#include "dbgtrace.h"
#include "ccportab.h"


static event_listener_t el;
static event_source_t IR_receiver;

static int32_t index = -1;
static bool START_OCCURED = FALSE, REPEAT_FLAG = FALSE;
static IR_CMD_t tmp, command;
icucnt_t cntW = 0;
#ifdef RAW_IR
static void icuwidthcbRAW(ICUDriver *icup) {
	  cntW = icuGetWidthX(icup);
	  chSysLockFromISR();
	  chEvtBroadcastFlagsI(&IR_receiver, 0);
	  chSysUnlockFromISR();
}
#else
static void icuwidthcb(ICUDriver *icup) {

  icucnt_t cnt = icuGetWidthX(icup);
  if((cnt > (START_PULSE - DELTA)) && (cnt < (START_PULSE + DELTA))){
    index = 0;
    START_OCCURED = TRUE;
  }
  else if((cnt > (ONE_PULSE - DELTA)) && (cnt < (ONE_PULSE + DELTA))){
    tmp |= 1 << (31 - index);
    index++;
  }
  else if((cnt > (ZERO_PULSE - DELTA)) && (cnt < (ZERO_PULSE + DELTA))){
    tmp &= ~(1 << (31 - index));
    index++;
  }
  else if((cnt > (END_PULSE - DELTA)) && (cnt < (END_PULSE + DELTA))){
    if((START_OCCURED) && (index == 32))
      command = tmp;
    else{
      command = 0;
    }
    REPEAT_FLAG = FALSE;
    START_OCCURED = FALSE;
    index = -1;
  }
  else if((cnt > (RPT_CMD_PULSE - DELTA)) && (cnt < (RPT_CMD_PULSE + DELTA))){
    REPEAT_FLAG = TRUE;
  }
  else if((cnt > (COMMA_PULSE - DELTA)) && (cnt < (COMMA_PULSE + DELTA))){
	  chSysLockFromISR();
	  chEvtBroadcastFlagsI(&IR_receiver, 0);
	  chSysUnlockFromISR();
 }
  else{
    /* Long dead pulse nothing to do */
  }
}
#endif

static ICUConfig icucfg = {
  ICU_INPUT_ACTIVE_HIGH,
  IR_FREQUENCY_USED,            /* 10kHz ICU clock frequency. */
#ifdef RAW_IR
  icuwidthcbRAW,
#else
  icuwidthcb,
#endif
  NULL,
  NULL,
  PORTABLE_ICU_CHANNEL,
  0,
  0xFFFFFFFFU
};

static ActionEvent_Typedef 		*pSetVolumeUpAE 		= NULL;
static ActionEvent_Typedef 		*pSetVolumeDownAE 		= NULL;
static ActionEvent_Typedef 		*pTogglePauseAE			= NULL;
static ActionEvent_Typedef 		*pToggleMuteAE			= NULL;
static ActionEvent_Typedef 		*pToggleSleepAE			= NULL;
static ActionEvent_Typedef 		*pNextTrackAE			= NULL;
static ActionEvent_Typedef 		*pPrevTrackAE			= NULL;
//Plz don't change this method. Just override in your code; that is why it is a weak method
CC_WEAK void handlIREvent(IR_CMD_t command, bool repeatFlag){
	   switch(command){
	    case BTN_POWER:
	       dbgprintf("PWR");
	       if(repeatFlag)
	         dbgprintf(" RPT");
	       else
	    	   triggerActionEvent(pToggleSleepAE->name, NULL, 0, SOURCE_EVENT_IR);
	       break;
	    case BTN_MODE:
	      dbgprintf("MODE");
	      if(repeatFlag)
	        dbgprintf(" RPT");
	      break;
	    case BTN_MUTE:
	      dbgprintf("MUTE");
	      if(repeatFlag)
	        dbgprintf(" RPT");
	      else
	    	triggerActionEvent(pToggleMuteAE->name, NULL, 0, SOURCE_EVENT_IR);
	      break;
	    case BTN_PREV:
	      dbgprintf("PREV");
	      if(repeatFlag)
	        dbgprintf(" RPT");
	      break;
	    case BTN_NEXT:
	      dbgprintf("NEXT");
	      if(repeatFlag)
	        dbgprintf(" RPT");
	      break;
	    case BTN_PLAY_PAUSE:
	      dbgprintf("PLAY/PAUSE");
	      if(REPEAT_FLAG)
	        dbgprintf(" RPT");
	      else
			triggerActionEvent(pTogglePauseAE->name, NULL, 0, SOURCE_EVENT_IR);
	      break;
	    case BTN_VOL_DOWN:
	      dbgprintf("VOL-");
	      if(repeatFlag)
	        dbgprintf(" RPT");
	      else
	    	 triggerActionEvent(pSetVolumeDownAE->name, NULL, 0, SOURCE_EVENT_IR);
	      break;
	    case BTN_VOL_UP:
	      dbgprintf("VOL+");
	      if(repeatFlag)
	        dbgprintf(" RPT");
	      else
	    	 triggerActionEvent(pSetVolumeUpAE->name, NULL, 0, SOURCE_EVENT_IR);
	      break;
	    case BTN_EQ:
	      dbgprintf("EQ");
	      if(repeatFlag)
	        dbgprintf(" RPT");
	      break;
	    case BTN_0:
	      dbgprintf("0");
	      if(repeatFlag)
	        dbgprintf(" RPT");
	      break;
	    case BTN_LOOP:
	      dbgprintf("100+");
	      if(repeatFlag)
	        dbgprintf(" RPT");
	      break;
	    case BTN_USD:
	      dbgprintf("200+");
	      if(repeatFlag)
	        dbgprintf(" RPT");
	      break;
	    case BTN_1:
	      dbgprintf("1");
	      if(repeatFlag)
	        dbgprintf(" RPT");
	      break;
	    case BTN_2:
	      dbgprintf("2");
	      if(repeatFlag)
	        dbgprintf(" RPT");
	      break;
	    case BTN_3:
	      dbgprintf("3");
	      if(repeatFlag)
	        dbgprintf(" RPT");
	      break;
	    case BTN_4:
	      dbgprintf("4");
	      if(repeatFlag)
	        dbgprintf(" RPT");
	      break;
	    case BTN_5:
	      dbgprintf("5");
	      if(repeatFlag)
	        dbgprintf(" RPT");
	      break;
	    case BTN_6:
	      dbgprintf("6");
	      if(repeatFlag)
	        dbgprintf(" RPT");
	      break;
	    case BTN_7:
	      dbgprintf("7");
	      if(repeatFlag)
	        dbgprintf(" RPT");
	      break;
	    case BTN_8:
	      dbgprintf("8");
	      if(repeatFlag)
	        dbgprintf(" RPT");
	      break;
	    case BTN_9:
	      dbgprintf("9");
	      if(repeatFlag)
	        dbgprintf(" RPT");
	      break;
	    default:
	      dbgprintf("Unknown");
	      break;
	    }
	    dbgprintf("\n\r");
}

static THD_WORKING_AREA(wairReceiverThd, 1024);
THD_FUNCTION(irReceiverThd, arg) {
  (void)arg;
   chRegSetThreadName("irReceiverThd");
  pSetVolumeUpAE   	    = getActionEvent(VOLUME_UP_AE_NAME);
  pSetVolumeDownAE   	= getActionEvent(VOLUME_DOWN_AE_NAME);
  pTogglePauseAE 		= getActionEvent(TOGGLE_PAUSE_AE_NAME);
  pToggleMuteAE  		= getActionEvent(TOGGLE_MUTE_AE_NAME);
  pToggleSleepAE  	    = getActionEvent(GO_TO_SLEEP_AE_NAME);
  pNextTrackAE   		= getActionEvent(NEXT_TRACK_AE_NAME);

  chEvtObjectInit(&IR_receiver);
  chEvtRegister(&IR_receiver, &el, 0);

  icuStart(&PORTABLE_ICUD, &icucfg);
  palSetLineMode(PORTABLE_ICU_LINE, PAL_MODE_ALTERNATE(PORTABLE_ICU_AF) );
  icuStartCapture(&PORTABLE_ICUD);
  icuEnableNotifications(&PORTABLE_ICUD);
  chThdSleepMilliseconds(2000);

  systime_t lastPulseTime = chVTGetSystemTimeX();
  while (true) {
    chEvtWaitAny(ALL_EVENTS);
#ifdef RAW_IR
    dbgprintf("%d ",cntW);
#else
	systime_t currentPulseTime = chVTGetSystemTimeX();
	int       delta            = currentPulseTime - lastPulseTime;
    bool      ignore           = TIME_I2MS(delta) <= 150;//Handle multi transmission

    if ( !ignore){
        lastPulseTime = currentPulseTime;
        handlIREvent(command,REPEAT_FLAG);
    }
#endif
  }
  return ;
}

void initIRReeceiver(void){
    chThdCreateStatic(wairReceiverThd,  sizeof(wairReceiverThd),  NORMALPRIO+2,     irReceiverThd, NULL);
}

#endif
