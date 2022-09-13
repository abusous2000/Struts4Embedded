/*
 * PPMFrameDecoder.c

 *
 *  Created on: Aug 27, 2020
 *      Author: abusous2000
 */

#include "Strust4EmbeddedConf.h"
#if S4E_USE_PPM_FRAME_DECODER != 0
#include "ch.h"
#include "hal.h"
#include <string.h>
#include <stdlib.h>
#include "dbgtrace.h"
#include "PPMFrameDecoder.h"
#include "ccportab.h"

ButtonStats_Typedef getRCButtonStatus(uint8_t value){
	if ( PPM_WITHIN_MARGIN(value,RC_BUTTON_MIN_VALUE) )
		return BUTTON_STATE_LOW;
	else
	if ( PPM_WITHIN_MARGIN(value,RC_BUTTON_MID_VALUE) )
		return BUTTON_STATE_MID;
	else
	if ( PPM_WITHIN_MARGIN(value,RC_BUTTON_MAX_VALUE) )
		return BUTTON_STATE_HIGH;
	else
		return BUTTON_STATE_UNKNOWN;
}

static icucnt_t last_width=0, last_period=0;
static uint8_t currentChannel = 0;
static uint8_t currentFrame = 0;
static uint32_t frame = 0;
static uint32_t incompleteFrame = 0;


static PPM_FRAME_TYPDEF   	PPM_Frame[MAX_FRAMES_TO_COLLECT*2] = {0};
static PPM_FRAME_TYPDEF   	*pCurrentPPM_Frame                 =  &PPM_Frame[0];
static PPM_FRAME_TYPDEF 	mostRecentFrame 				   = {0};
static PPM_FRAME_TYPDEF 	mostRecentFrame2 				   = {0};
static uint8_t              currentBatch 					   = 0;
static bool 				currentPPMDecoderState 			   = true;

thread_reference_t 	ppmDecoderThdRef 	= NULL;

#if S4E_USE_IBUS == 0

#if 0
static void icuwidthcb(ICUDriver *icup) {
  last_width = icuGetWidthX(icup);

}

static void icuperiodcb(ICUDriver *icup) {
  last_period = icuGetPeriodX(icup);
  if ( last_period >= PPM_MIN_GAP_BTWN_FRAMES ){
	  bool invalidFrame = currentChannel < (MAX_CHANNELS -2);
	  pCurrentPPM_Frame->end = chVTGetSystemTimeX();
	  if ( pCurrentPPM_Frame->start > 0 ){
		 pCurrentPPM_Frame->frameDuration         = chTimeDiffX(pCurrentPPM_Frame->start,pCurrentPPM_Frame->end);
		 pCurrentPPM_Frame->frameDurationMicroSec = chTimeI2US(pCurrentPPM_Frame->frameDuration);
	  }
	  pCurrentPPM_Frame->gap    = last_period;
      if ( invalidFrame )
		  ++incompleteFrame;
	  else{
		  if ( currentFrame >= MAX_FRAMES_TO_COLLECT ){
			 currentBatch      = currentBatch==0?1:0;
			 currentFrame      = 0;
			 if ( currentPPMDecoderState ){
				 chSysLockFromISR();
				 chThdResumeI((thread_reference_t*)&ppmDecoderThdRef,0);
				 chSysUnlockFromISR();
			 }
		  }
  	      pCurrentPPM_Frame    = &PPM_Frame[currentBatch*MAX_FRAMES_TO_COLLECT+currentFrame++];
	  }
      currentChannel =0;
	  memset(pCurrentPPM_Frame,0,sizeof(PPM_FRAME_TYPDEF));
	  pCurrentPPM_Frame->start   = chVTGetSystemTimeX();
	  pCurrentPPM_Frame->frameID = invalidFrame?frame:++frame;
 }
  else{
	  currentChannel = currentChannel >= (MAX_CHANNELS -1)?0:currentChannel;
	  pCurrentPPM_Frame->valueInCycles[currentChannel]  = last_width;
	  pCurrentPPM_Frame->period[currentChannel] 		= last_period;
	  currentChannel++;
 }
}
#endif

static systime_t            lastPulseEndTime;

static void icuwidthcbV2(ICUDriver *icup) {
  last_width = icuGetWidthX(icup);
  systime_t currentPulseTime = chVTGetSystemTimeX();
  int       ticksBtwnLastTowPluses  = currentPulseTime - lastPulseEndTime;
  lastPulseEndTime = currentPulseTime;
  icucnt_t lastWidth=last_width;
  uint8_t  localCurrentChannel =currentChannel;

  if ( last_width >= PPM_MIN_GAP_BTWN_FRAMES ){
	  pCurrentPPM_Frame->channelsCollected = currentChannel;
	  //Make sure at least we've collected enough channels 1st
	  bool invalidFrame = currentChannel < (MAX_CHANNELS -4);
	  pCurrentPPM_Frame->end = chVTGetSystemTimeX();
	  if ( pCurrentPPM_Frame->start > 0 ){
		 pCurrentPPM_Frame->frameDuration         = chTimeDiffX(pCurrentPPM_Frame->start,pCurrentPPM_Frame->end);
		 pCurrentPPM_Frame->frameDurationMicroSec = chTimeI2US(pCurrentPPM_Frame->frameDuration);
	  }
	  pCurrentPPM_Frame->gap    = last_width;
      if ( invalidFrame ){
		  ++incompleteFrame;
		  mostRecentFrame2 = *pCurrentPPM_Frame;
		  if ( incompleteFrame % MAX_FRAMES_TO_COLLECT == 0 ){
			 chSysLockFromISR();
			 chThdResumeI((thread_reference_t*)&ppmDecoderThdRef,1);
			 chSysUnlockFromISR();
		  }
      }
	  else{
		  if ( currentFrame >= MAX_FRAMES_TO_COLLECT ){
			 currentBatch      = currentBatch==0?1:0;
			 currentFrame      = 0;
			 if ( currentPPMDecoderState ){
				 chSysLockFromISR();
				 chThdResumeI((thread_reference_t*)&ppmDecoderThdRef,0);
				 chSysUnlockFromISR();
			 }
		  }
  	      pCurrentPPM_Frame    = &PPM_Frame[currentBatch*MAX_FRAMES_TO_COLLECT+currentFrame++];
	  }

      currentChannel =0;
	  memset(pCurrentPPM_Frame,0,sizeof(PPM_FRAME_TYPDEF));
	  pCurrentPPM_Frame->start   = chVTGetSystemTimeX();
	  pCurrentPPM_Frame->frameID = invalidFrame?frame:++frame;
	  pCurrentPPM_Frame->source = PPM_SIGNAL_SRC;
 }
 else
 if ( IS_VALID_PPM_VALUE(last_width) ){
	  currentChannel = currentChannel > (MAX_CHANNELS -1)?0:currentChannel;
	  pCurrentPPM_Frame->valueInCycles[currentChannel]  = last_width;
	  pCurrentPPM_Frame->period[currentChannel] 		= ticksBtwnLastTowPluses;
	  if (currentChannel >= 6 )
		 currentChannel =currentChannel;
	  currentChannel++;
 }
 else
	 currentChannel =currentChannel;
}
#endif

//A weak/virtual function that could be overridden in other modules
CC_WEAK void onChannelPPMValueChange(uint8_t ch, PPM_FRAME_TYPDEF   *pCurrentPPMFrame, PPM_FRAME_TYPDEF   *pLastPPMFrame ){
	printPPMValueChange(ch,pCurrentPPMFrame,pLastPPMFrame);
}

void printPPMValueChange(uint8_t ch, PPM_FRAME_TYPDEF   *pCurrentPPMFrame, PPM_FRAME_TYPDEF   *pLastPPMFrame ){
#if S4E_USE_IBUS != 0
  dbgprintf("iBus OnChangeChannel (%d) Value: %d / %d\t%d / %d\r\n", ch, pCurrentPPMFrame->rawValue[ch], pLastPPMFrame->rawValue[ch], pCurrentPPMFrame->valueInCycles[ch], pLastPPMFrame->valueInCycles[ch]);
#else
  dbgprintf("PPM OnChangeChannel (%d) Value: %d\t%d\r\n", ch, pCurrentPPMFrame->valueInCycles[ch], pLastPPMFrame->valueInCycles[ch]);
#endif
}
void toggleEnableDisablePPMDecoder(void){
	enablePPMDecoder(currentPPMDecoderState?false:true);
}
static bool ppmDebug = false;
void toggleDebugPPMDecoder(void){
	ppmDebug = !ppmDebug;
}
void enablePPMDecoder(bool enable){
	if ( enable)
		icuEnableNotifications(&RC_ICUD);
	else
		icuDisableNotifications(&RC_ICUD);

	currentPPMDecoderState = enable;
	dbgprintf("PPMDecoder State: %d\r\n", enable);
}



#if S4E_USE_IBUS != 0
uint16_t channel[MAX_CHANNELS] = {0};

static const SerialConfig iBusSerialcfg = {
  115200,
  0,
  USART_CR2_STOP1_BITS,
  0
};

#else
static ICUConfig icucfg = {
  .mode=ICU_INPUT_ACTIVE_HIGH,
  .frequency=PPM_FREQUENCY_USED,  //50kHz ICU clock frequency
  .width_cb=icuwidthcbV2,
  .period_cb=NULL,//icuperiodcb,
  .overflow_cb=NULL,
  .channel=RC_ICU_CHANNEL,
  .dier=0,
  .arr=0xFFFFFFFFU
};
#endif

#if S4E_USE_IBUS != 0
const double freqInSec = (double)((double)PPM_FREQUENCY_USED/1000000);

static void handleIBusComand40(uint8_t *rx_buffer){
	uint16_t checksum_cal, checksum_ibus;
	uint16_t channelRawValues[IBUS_MAX_CHANNLES] = {0};

	checksum_cal = 0xffff - rx_buffer[0] - rx_buffer[1];
	for(int i = 0; i < IBUS_MAX_CHANNLES; i++){
		channelRawValues[i] = (uint16_t)(rx_buffer[i * 2 + 3] << 8 | rx_buffer[i * 2 + 2]);
		checksum_cal = checksum_cal - rx_buffer[i * 2 + 3] - rx_buffer[i * 2 + 2];

	}
	checksum_ibus = rx_buffer[IBUS_BUFF_LEN - 1] << 8 | rx_buffer[IBUS_BUFF_LEN-2];

	PPM_FRAME_TYPDEF   *pLastPPMFrame = &PPM_Frame[0];

	pLastPPMFrame->source = IBUS_SIGNAL_SRC;
	if(checksum_cal == checksum_ibus){
		for(int i = 0; i < MAX_CHANNELS; i++){
			pLastPPMFrame->rawValue[i]      = channelRawValues[i];
			pLastPPMFrame->valueInCycles[i] = (uint8_t)(channelRawValues[i]*freqInSec);

			int delta = pLastPPMFrame->valueInCycles[i] - mostRecentFrame.valueInCycles[i];
			if ( pLastPPMFrame->valueInCycles[i] > 0 && delta != 0 && abs(delta) > RC_ERR_MARGIN  )
				 onChannelPPMValueChange(i,&mostRecentFrame,pLastPPMFrame);
		}
		mostRecentFrame = *pLastPPMFrame;
	}

	if ( ppmDebug ){
	    dbgprintf("RC Channel Values:");
		for(int i = 0; i < IBUS_MAX_CHANNLES; i++){
		    dbgprintf("(%d)%d\t", i+1, channelRawValues[i]);
		}
	    dbgprintf("\r\n");
	}

	if ( IBUS_SLEEP_BTWN_MSGS > 0 ){
	   chThdSleepMilliseconds(IBUS_SLEEP_BTWN_MSGS);
	   chSysLock();
	   oqResetI(&IBUS_SD.iqueue);
	   chSchRescheduleS();
	   chSysUnlock();
	}
}
#else

void printPPMFrame(PPM_FRAME_TYPDEF   *pCurrentPWM, int ch ){
	 dbgprintf("Collected:%d;Duration: %d;GAP:%d;NDX:%d\t",pCurrentPWM->channelsCollected,pCurrentPWM->frameDurationMicroSec,pCurrentPWM->gap,ch);
	 for(int i = 0; i < MAX_CHANNELS; ++i){
		 if ( i < 8 )
			 dbgprintf("(%d)-->%d-%d-%d\t", (i+1), pCurrentPWM->valueInCycles[i],TimeFromCycles2US(pCurrentPWM->valueInCycles[i]), pCurrentPWM->period[i]  );
		 else
		 if ( i < 8 )
			 dbgprintf("%d-%d\t", getRCButtonStatus(pCurrentPWM->valueInCycles[i]), TimeFromCycles2US(pCurrentPWM->period[i])  );
		 else
		 if ( i < 10 )
			 dbgprintf("%d-%d\t", TimeFromCycles2US(pCurrentPWM->valueInCycles[i]), TimeFromCycles2US(pCurrentPWM->period[i])  );
	 }
	 dbgprintf("\r\n");
}
#endif

static THD_WORKING_AREA(wappmDecoderThread, PPM_THD_STACK_SIZE);
static THD_FUNCTION(ppmDecoderThread, arg) {(void)arg;
    chRegSetThreadName("ppmDecoderThread");

#if S4E_USE_IBUS != 0
	palSetLineMode(IBUS_UART_RX, IBUS_PIN_MODE);
	sdStart(&IBUS_SD, &iBusSerialcfg);

#else
	icuStart(&RC_ICUD, &icucfg);
	palSetLineMode(RC_ICU_LINE, PAL_MODE_ALTERNATE(RC_ICU_AF));

	icuStartCapture(&RC_ICUD);
	icuEnableNotifications(&RC_ICUD);
	uint8_t lastValue = MAX_FRAMES_TO_COLLECT > 1?MAX_FRAMES_TO_COLLECT-1:0;
#endif
	while (true) {
		if ( currentPPMDecoderState ){
#if S4E_USE_IBUS != 0
			uint8_t rx_buffer[IBUS_BUFF_LEN] = {0};

			size_t byteRead = sdReadTimeout(&IBUS_SD,rx_buffer, IBUS_BUFF_LEN, TIME_MS2I(100));
			if ( byteRead == 0 )
				continue;
			else
			if( IS_IBUS_CH_COMMAND40(rx_buffer) )
				handleIBusComand40(rx_buffer);
			else{
				//TODO:process other messages
				;
			}
#else
			 chSysLock();
			 msg_t reInit = chThdSuspendS((thread_reference_t *)&ppmDecoderThdRef);
			 chSysUnlock();
			 uint8_t filledBatch = !currentBatch;
			 uint8_t base = filledBatch * MAX_FRAMES_TO_COLLECT;
			 //Print invalid-frame
			 if ( reInit ){
				 printPPMFrame(&mostRecentFrame2,16);
				 continue;
			 }
			 for(int i = 0;ppmDebug && i < MAX_FRAMES_TO_COLLECT; ++i){
				 printPPMFrame(&PPM_Frame[base+i],i);
			 }

			 PPM_FRAME_TYPDEF   *pLastPPMFrame = &PPM_Frame[base+lastValue];

			 pLastPPMFrame->source = PPM_SIGNAL_SRC;
			 for(int j = 0;j < MAX_CHANNELS; ++j){
				 int delta = pLastPPMFrame->valueInCycles[j] - mostRecentFrame.valueInCycles[j];
				 if ( pLastPPMFrame->valueInCycles[j] > 0 && delta != 0 && abs(delta) > RC_ERR_MARGIN  )
					 onChannelPPMValueChange(j,&mostRecentFrame,pLastPPMFrame);
			 }

			 mostRecentFrame = *pLastPPMFrame;
#endif
		}
		else
			chThdSleepMilliseconds(1000);
	}

	return;
}

void initPPMFrameDecoder(void){
    chThdCreateStatic(wappmDecoderThread,  sizeof(wappmDecoderThread),  NORMALPRIO+2,     ppmDecoderThread, NULL);
}

#endif
