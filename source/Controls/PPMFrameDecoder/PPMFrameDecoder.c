/*
 * PPMFrameDecoder.c

 *
 *  Created on: Aug 27, 2020
 *      Author: abusous2000
 */

#include "Strust4EmbeddedConf.h"
#if PPM_FRAME_DECODER != 0
#include "ch.h"
#include "hal.h"
#include <string.h>
#include "dbgtrace.h"
#include "PPMFrameDecoder.h"
#include "ccportab.h"

ButtonStats_Typedef getRCButtonStatus(uint8_t value){
	if ( BUTTON_IN_MARGIN(value,RC_MIN_VALUE) )
		return BUTTON_STATE_LOW;
	else
	if ( BUTTON_IN_MARGIN(value,RC_MID_VALUE) )
		return BUTTON_STATE_MID;
	else
	if ( BUTTON_IN_MARGIN(value,RC_MAX_VALUE) )
		return BUTTON_STATE_HIGH;
	else
		return BUTTON_STATE_UNKNOWN;
}
#ifdef PORTABLE_PWM_LINE
static void pwmpcb(PWMDriver *pwmp) {
  (void)pwmp;
  palClearLine(LINE_LED_RED);
}

static void pwmc1cb(PWMDriver *pwmp) {
  (void)pwmp;
  palSetLine(LINE_LED_RED);
}

static PWMConfig pwmcfg = {
  .frequency=FREQUENCY_USED,                                    /* 10kHz PWM clock frequency.   */
  .period=PERIOD_USED,                                    /* Initial PWM period 1S.       */
  .callback=pwmpcb,
  .channels={
	CH1_CB,
	CH2_CB,
	CH3_CB,
	CH4_CB
  },
  .cr2=0,
  .dier=0
};
#endif
static icucnt_t last_width=0, last_period=0;
static uint8_t currentChannel = 0;
static uint8_t currentFrame = 0;
static uint32_t frame = 0;
static uint32_t incompleteFrame = 0;


static PPM_FRAME_TYPDEF   	PPM_Frame[MAX_FRAMES_TO_COLLECT*2] = {0};
static PPM_FRAME_TYPDEF   	*pCurrentPPM_Frame                 =  &PPM_Frame[0];
static PPM_FRAME_TYPDEF 	mostRecentFrame 				   = {0};
static uint8_t              currentBatch 					   = 0;
static bool 				currentPPMDecoderState 			   = true;


thread_reference_t 	ppmDecoderThdRef 	= NULL;
static void icuwidthcb(ICUDriver *icup) {
  palSetLine(LINE_LED_GREEN);
  last_width = icuGetWidthX(icup);
}

static void icuperiodcb(ICUDriver *icup) {
  palClearLine(LINE_LED_GREEN);
  last_period = icuGetPeriodX(icup);
  if ( last_period >= MIN_GAP ){
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
static void icuovfcb(ICUDriver *icup) {
  (void)icup;
}

static ICUConfig icucfg = {
  .mode=ICU_INPUT_ACTIVE_HIGH,
  .frequency=FREQUENCY_USED,                                    /* 10kHz ICU clock frequency.   */
  .width_cb=icuwidthcb,
  .period_cb=icuperiodcb,
  .overflow_cb=icuovfcb,
  .channel=PORTABLE_ICU_CHANNEL,
  .dier=0,
  .arr=0xFFFFFFFFU
};

//A weak/virtual function that could be overridden in other modules
CC_WEAK void onChannelPPMValueChange(uint8_t ch, uint8_t currentValue, uint8_t newValue){
  dbgprintf("OnChangeChannelValue: %d\t%d\t%d\r\n", ch, currentValue, newValue);
}

void enablePPMDecoder(bool enable){
	if ( enable)
		icuEnableNotifications(&PORTABLE_ICUD);
	else
		icuDisableNotifications(&PORTABLE_ICUD);

	currentPPMDecoderState = enable;
	dbgprintf("PPMDecoder State: %d\r\n", enable);
}
static THD_WORKING_AREA(wappmDecoderThread, PPM_THD_STACK_SIZE);
static THD_FUNCTION(ppmDecoderThread, arg) {(void)arg;

	#ifdef PORTABLE_PWM_LINE
	pwmStart(&PORTABLE_PWMD, &pwmcfg);
	pwmEnablePeriodicNotification(&PORTABLE_PWMD);
	palSetLineMode(PORTABLE_PWM_LINE, PAL_MODE_ALTERNATE(PORTABLE_PWM_AF));
	#endif
	icuStart(&PORTABLE_ICUD, &icucfg);
	palSetLineMode(PORTABLE_ICU_LINE, PAL_MODE_ALTERNATE(PORTABLE_ICU_AF));

	#ifdef PORTABLE_PWM_LINE
	pwmEnableChannel(&PORTABLE_PWMD, PORTABLE_PWM_CHANNEL, PWM_PERCENTAGE_TO_WIDTH(&PORTABLE_PWMD, FREQUENCY_USED * .75));
	pwmEnableChannelNotification(&PORTABLE_PWMD, PORTABLE_PWM_CHANNEL);
    #endif
	icuStartCapture(&PORTABLE_ICUD);
	icuEnableNotifications(&PORTABLE_ICUD);
	uint8_t lastValue = MAX_FRAMES_TO_COLLECT > 1?MAX_FRAMES_TO_COLLECT-1:0;
	while (true) {
		if ( currentPPMDecoderState ){
			 chSysLock();
			 msg_t reInit = chThdSuspendS((thread_reference_t *)&ppmDecoderThdRef);
			 chSysUnlock();
			 uint8_t filledBatch = !currentBatch;
			 uint8_t base = filledBatch * MAX_FRAMES_TO_COLLECT;
			 #if PPM_DECODING_DEBUG != 0
			 for(int i = 0; i < MAX_FRAMES_TO_COLLECT; ++i){
				 PPM_FRAME_TYPDEF   *pCurrentPWM = &PPM_Frame[base+i];
				 dbgprintf("Frame:%d\tDuration: %d\tNDX:%d\t",pCurrentPWM->frameID,pCurrentPWM->frameDurationMicroSec,i);
				 for(int j = 0;j < MAX_CHANNELS; ++j){
					 if ( j < 4 )
						 dbgprintf("%d\t", TimeFromCycles2US(pCurrentPWM->valueInCycles[j]) );
					 else
					 if ( j < 8 )
						 dbgprintf("%d\t", getRCButtonStatus(pCurrentPWM->valueInCycles[j]) );
					 else
					 if ( j < 10 )
						 dbgprintf("%d\t", TimeFromCycles2US(pCurrentPWM->valueInCycles[j]) );
				 }
				 dbgprintf("\r\n");
			 }
			 #endif
			 PPM_FRAME_TYPDEF   *pLastPPMFrame = &PPM_Frame[base+lastValue];
			 for(int j = 0;j < MAX_CHANNELS; ++j){
				 if ( pLastPPMFrame->valueInCycles[j] > 0 && pLastPPMFrame->valueInCycles[j] != mostRecentFrame.valueInCycles[j] )
					 onChannelPPMValueChange(j,mostRecentFrame.valueInCycles[j],pLastPPMFrame->valueInCycles[j]);
			 }
			 mostRecentFrame = *pLastPPMFrame;
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
