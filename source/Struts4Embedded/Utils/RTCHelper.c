/*
 * RTCHelper.c
 *
 *  Created on: Sep 14, 2020
 *      Author: abusous2000
 */
#include "Strust4EmbeddedConf.h"
#include "ch.h"
#include "hal.h"
#if HAL_USE_RTC != 0
#include "stdlib.h"
#include "string.h"
#include "ccportab.h"
#include "Strust4Embedded.h"


#ifndef BACKUP_CCM_RAM_SECTION
#error Plz define this macro BACKUP_CCM_RAM_SECTION
#endif
#ifndef GO_TO_SLEEP_MACROS
#error Plz define this macro GO_TO_SLEEP_MACROS
#endif


static uint8_t goingToSleepCnt PLACE_IN_RAM_SECTION(BACKUP_CCM_RAM_SECTION)  = 0;
static time_t unix_time;
static RTCDateTime timespec;



#ifdef RTC_ALARM_1_FLAGS
static const RTCAlarm alarm1 = {
	RTC_ALARM_1_FLAGS
 };
#endif
#ifdef RTC_ALARM_2_FLAGS
static const RTCAlarm alarm2 = {
	RTC_ALARM_2_FLAGS
};
#endif

uint8_t getGoingToSleepCnt(void){
	return goingToSleepCnt;
}
CC_WEAK void onRTCSleep(void){
	dbgprintf("Going sleep in 10ms...:goingToSleepCnt:%d\r\n",getGoingToSleepCnt());
    chThdSleepMilliseconds(10);
}
void rtcGoToSleep(void) {
 ++(goingToSleepCnt);
  onRTCSleep();
  chSysLock();
  GO_TO_SLEEP_MACROS
  __WFI();
}

static time_t rtcGetTimeUnixSec(void) {
  struct tm tim;

  rtcGetTime(&RTCD1, &timespec);
  rtcConvertDateTimeToStructTm(&timespec, &tim, NULL);
  return mktime(&tim);
}

static void rtcGetTimeTm(struct tm *timp) {
  rtcGetTime(&RTCD1, &timespec);
  rtcConvertDateTimeToStructTm(&timespec, timp, NULL);
}

void rtcSetTimeUnixSec(time_t unix_time) {
  struct tm tim;
  struct tm *canary;

  /* If the conversion is successful the function returns a pointer
     to the object the result was written into.*/
  canary = localtime_r(&unix_time, &tim);
  osalDbgCheck(&tim == canary);

  rtcConvertStructTmToDateTime(&tim, 0, &timespec);
  rtcSetTime(&RTCD1, &timespec);
}

char *rtcGetTimeAsString(void){
    unix_time = rtcGetTimeUnixSec();
    char *timeStr = NULL;

    if (unix_time == -1){
    	dbgprintf("incorrect time in RTC cell\r\n");
    }
    else{
      struct tm timp = {0};
      rtcGetTimeTm(&timp);
      timeStr = asctime(&timp);
    }

    return timeStr;
}
void rtcSetTimeUnixSecFromString(char *unixtimeStr) {
	 unix_time = atol(unixtimeStr);
	 rtcSetTimeUnixSec(unix_time);
}

#if RTC_HAS_STORAGE
void rtcDumpStorage(void) {
  size_t storage_size = psGetStorageSize(&RTCD1);
  ps_offset_t i;

  for (i = 0U; i < (ps_offset_t)storage_size; i++) {
    uint8_t val;
    psRead(&RTCD1, i, 1U, &val);
    dbgprintf("%02x ", val);
    if (((i + 1) & 15) == 0U) {
    	dbgprintf("\r\n");
    }
  }
}
#endif
CC_WEAK void onRTCAlarmA(void){

}
CC_WEAK void onRTCAlarmB(void){

}
static uint32_t systemWakeup  = false;

bool getRTCSystemWakeup(void){
	return systemWakeup;
}

void setRTCSystemWakeup(bool value){
	systemWakeup = value;
	if ( !value )
		goingToSleepCnt = 0;
}
CC_WEAK void onRTCSystemWakeup(void){
	setRTCSystemWakeup(true);
}

/*
 * RTC callback.
 */
static void alarmcb(RTCDriver *rtcp, rtcevent_t event) {
  (void)rtcp;
  switch((uint8_t)event){
  	  case RTC_EVENT_WAKEUP:
  		  onRTCSystemWakeup();
		  break;
	  case RTC_EVENT_ALARM_A:
		  onRTCAlarmA();
		  break;
	  case RTC_EVENT_ALARM_B:
		  onRTCAlarmB();
		  break;
  }

  return;
}


void RTCInit(void){
	systemWakeup = WAKEUP_HARD_REST_CHECK;
	dbgprintf("Hard Rest(0)/Wakeup(2):%d\r\n",  systemWakeup);
	CLEAR_WAKEUP_FLAG;
	#ifdef RTC_ALARM_1_FLAGS
	rtcSetAlarm(&RTCD1, 0, &alarm1);
	#endif
	#ifdef RTC_ALARM_1_FLAGS
	rtcSetAlarm(&RTCD1, 1, &alarm2);
	#endif
	rtcSetCallback(&RTCD1, alarmcb);

	#if RTC_HAS_STORAGE
	  psWrite(&RTCD1, 0U, 12U, (const uint8_t *)"Hello World!");
	#endif

}

#endif
