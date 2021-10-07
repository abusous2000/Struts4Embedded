/*
 * RTCHelper.h
 *
 *  Created on: Sep 14, 2020
 *      Author: abusous2000
 */

#ifndef SOURCE_STRUTS4EMBEDDED_UTILS_RTCHELPER_H_
#define SOURCE_STRUTS4EMBEDDED_UTILS_RTCHELPER_H_

#if HAL_USE_RTC != 0
#ifdef __cplusplus
 extern "C" {
#endif
void onRTCSleep(void);
void RTCInit(void);
void rtcDumpStorage(void);
void rtcSetTimeUnixSecFromString(char *unixtimeStr);
char *rtcGetTimeAsString(void);
void rtcSetTimeUnixSec(time_t unix_time);
void rtcGoToSleep(void);
void onRTCSystemWakeup(void);
int16_t getGoingToSleepCnt(void);
bool getRTCSystemWakeup(void);
void setRTCSystemWakeup(bool value);
#ifdef __cplusplus
 }
#endif

#endif

#endif /* SOURCE_STRUTS4EMBEDDED_UTILS_RTCHELPER_H_ */
