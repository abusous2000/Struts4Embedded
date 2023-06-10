#ifndef ACTIONEVENTSTHD_H_
#define ACTIONEVENTSTHD_H_
#include "ActionEventsThd.h"

#ifndef BLINKER_THD_STACK_SIZE
#define BLINKER_THD_STACK_SIZE	           256
#endif
#ifndef MAIN_QUEUE_SIZE
#define MAIN_QUEUE_SIZE						10
#endif

#define MAIN_ACTION_EVENTS_QUEUE            "mainActionEventsQueue"
#ifndef PROCESS_ACTION_EVENTS_THD_WA
#define PROCESS_ACTION_EVENTS_THD_WA   		1024
#endif
#ifndef MAX_ACTION_EVENTS
#define MAX_ACTION_EVENTS                   40
#endif
#define CHAR_DTYPE							1
#define INT_DTYPE							0
#define SOURCE_EVENT_WIFI					"WiFi"
#define SOURCE_EVENT_MQTT					"MQTT"
#define SOURCE_EVENT_LCD					"LCD"
#define SOURCE_EVENT_IR					    "IR"
#define SOURCE_EVENT_RC						"RC"
#define SOURCE_EVENT_LORA					"LoRa"
#define SOURCE_USB_HID   					"UsbHid"
#define SOURCE_EVENT_CAN_BUS				"CanBus"

#ifndef NONE_BUTTON
#define NONE_BUTTON   		                0b0
#define TOGGLE_MUTE_BUTTON 	            	0b1
#define TOGGLE_PAUSE_BUTTON             	0b10
#define NEXT_TRACK_BUTTON               	0b100
#define PREV_TRACK_BUTTON   	        	0b1000
#define VOLUME_UP_BUTTON    		    	0b10000
#define VOLUME_DOWN_BUTTON  		    	0b100000
#define TOGGLE_BUZZER_BUTTON  		    	0b1000000
#endif


#define DATA_FIELD_WAS_LT_MAX               100
struct ActionEvent_Typedef;
typedef int32_t (*onActionEventCallback_t)(struct ActionEvent_Typedef *data);
typedef struct ActionEvent_Typedef {
	uint8_t                     dataType;
	union{
		char                   	*pData;
		uint32_t				data;
	} u;
	char                        *eventSource;
	char                        *name;
	char                        *nextActionEventName;
	uint8_t                     priority;
	uint8_t                     processed;
	uint8_t                     ndx;
	uint8_t                     error;
	uint32_t                    time;
	onActionEventCallback_t 	action;
	onActionEventCallback_t 	view;
}ActionEvent_Typedef;
#ifndef ACTION_EVENT_DATA_MAX_SIZE
#define ACTION_EVENT_DATA_MAX_SIZE          40
#endif
#ifndef ACTION_EVENT_NAME_SIZE
#define ACTION_EVENT_NAME_SIZE              25
#endif
#define MAX_DATA_POOL_BUFFERS 				MAIN_QUEUE_SIZE

#ifndef MAX_POOL_BUFFER_SIZE
#define MAX_POOL_BUFFER_SIZE   				ACTION_EVENT_DATA_MAX_SIZE
#endif
typedef struct ActionEventPayload {
    char                        actionEvent[ACTION_EVENT_NAME_SIZE];
    char                        data[ACTION_EVENT_DATA_MAX_SIZE];
}ActionEventPayload_Typedef;

#ifdef __cplusplus
 extern "C" {
#endif
void setTriggerUpdateLCD(void);
int8_t getCurrentVolume(void);
ActionEvent_Typedef *getActionEvent(char *aeName);
void ActonEventThd(void *arg);
int8_t getActionEventNdx(char *aeName);
ActionEventPayload_Typedef *parseActionEventPayload(char *payload, ActionEventPayload_Typedef *pAEPayload);
ActionEvent_Typedef *sendActionEventI(char *aeName);
ActionEvent_Typedef *triggerActionEventFromISR(char *aeName, char *pData,  uint32_t data,char *eventSource);
ActionEvent_Typedef *triggerActionEvent(char *aeName, char *pData,  uint32_t data, char *eventSource);
void initActonEventThd(void);
bool isActonEventThdInitialized(void);
#ifdef __cplusplus
}
#endif
#endif /* ACTIONEVENTS_H_ */
