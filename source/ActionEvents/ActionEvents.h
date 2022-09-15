#ifndef ACTIONEVENTS_H_
#define ACTIONEVENTS_H_
#include "ActionEventsThd.h"


#define TOGGLE_MUTE_AE_NAME                 "toggleMute"
#define NEXT_TRACK_AE_NAME                  "nextTrack"
#define PREV_TRACK_AE_NAME                  "prevTrack"
#define TOGGLE_PAUSE_AE_NAME                "togglePause"
#define VOLUME_UP_AE_NAME                   "volumeUp"
#define VOLUME_DOWN_AE_NAME                 "volumeDown"
#define SET_VOLUME_AE_NAME                  "setVolume"
#define SET_UNIX_TIME_AE_NAME				"setUnixtime"
#define GO_TO_SLEEP_AE_NAME                 "goToSleep"
#define UPDATE_WIFI_HTML_AE_NAME            "updateWiFi"
#define LOAD_SYS_PROPERTIES_AE_NAME         "loadSysProperties"
#define SET_RGB_LED_AE_NAME                 "setRGBLED"
#define SET_MOTORS_PARAMS_AE_NAME           "setMotorsParams"
#define NEW_HTML_LOADED_AE_NAME				"newHTMLLoaded"
#define PERFORMANCE_INFO_AE_NAME			"performanceInfo"
#define TOGGLE_BUZZER_AE_NAME               "toggleBuzzer"
#define TEST_SDCARD                         "testSDCard"
#define CAN_BUS_CONTROL_AE_NAME             "canBusControl"
#define CAN_BUS_SEND_MSG_AE_NAME            "canBusSendMsg"
#define CAN_BUS_ADD_FILTER_AE_NAME          "canBusAddFilter"
#define TOGGLE_ENABLE_MODULE_AE_NAME        "toggleModule"
#define TOGGLE_RELAY_MODULE_AE_NAME         "toggleRelay"

#ifdef __cplusplus
 extern "C" {
#endif
int8_t getCurrentVolume(void);
int32_t volumeView(ActionEvent_Typedef 	*pActionEvent);
int32_t toggleMuteView(ActionEvent_Typedef 	*pActionEvent);
int8_t getCurrentMute(void);
int8_t getCurrentVolume(void);
int32_t loadSysPropertiesView(ActionEvent_Typedef 	*pActionEvent);
int32_t updateWifiHtmlView(ActionEvent_Typedef 	*pActionEvent);
int32_t setRGBLEDView(ActionEvent_Typedef 	*pActionEvent);
int32_t setPWMParamsView(ActionEvent_Typedef 	*pActionEvent);
int8_t isPaused(void);
void eByteLoraSendFrame(int8_t buttonPressed);
#ifdef __cplusplus
}
#endif
#endif /* ACTIONEVENTS_H_ */
