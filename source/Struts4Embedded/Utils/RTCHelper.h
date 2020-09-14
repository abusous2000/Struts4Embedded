/*
 * RTCHelper.h
 *
 *  Created on: Sep 14, 2020
 *      Author: abusous2000
 */

#ifndef SOURCE_STRUTS4EMBEDDED_UTILS_RTCHELPER_H_
#define SOURCE_STRUTS4EMBEDDED_UTILS_RTCHELPER_H_


#ifdef __cplusplus
 extern "C" {
#endif
void s4eRTCInit(void);
void rtcDumpStorage(void);
void rtcSetTimeUnixSecFromString(char *unixtimeStr);
char *rtcGetTimeAsString(void);
void rtcSetTimeUnixSec(time_t unix_time);
void rtcGoToSleep(void);
#ifdef __cplusplus
 }
#endif
#endif /* SOURCE_STRUTS4EMBEDDED_UTILS_RTCHELPER_H_ */
