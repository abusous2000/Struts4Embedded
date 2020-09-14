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
#include "Strust4Embedded.h"

static uint8_t rebooted PLACE_IN_RAM_SECTION(CCM_RAM_SECTION)  = 0;
static time_t unix_time;
static RTCDateTime timespec;

#ifndef GO_TO_SLEEP_MACROS
#error Plz define this macro GO_TO_SLEEP_MACROS
#endif


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

void rtcGoToSleep(void) {
  dbgprintf("Going sleep...%d\r\n",++(rebooted));
  chThdSleepMilliseconds(500);
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
/*
 * RTC callback.
 */
static void alarmcb(RTCDriver *rtcp, rtcevent_t event) {
  (void)rtcp;

  if (event == RTC_EVENT_ALARM_A) {
	  NULL;
  }
  else if (event == RTC_EVENT_ALARM_B) {
	  NULL;
  }
}


void s4eRTCInit(void){
	#ifdef RTC_ALARM_1_FLAGS
	rtcSetAlarm(&RTCD1, 0, &alarm1);
	rtcSetCallback(&RTCD1, alarmcb);
	#endif
	#ifdef RTC_ALARM_1_FLAGS
	rtcSetAlarm(&RTCD1, 1, &alarm2);
	rtcSetCallback(&RTCD1, alarmcb);
	#endif

	#if RTC_HAS_STORAGE
	  psWrite(&RTCD1, 0U, 12U, (const uint8_t *)"Hello World!");
	#endif

}

#endif
