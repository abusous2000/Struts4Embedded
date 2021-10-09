/*
 * WifiCommunicationThd.c
 *
 *  Created on: Sep 8, 2019
 *      Author: abusous2000
 */
#include "Strust4EmbeddedConf.h"
#if S4E_USE_WIFI_MODULE_THD != 0
#include "ch.h"
#include "hal.h"
#include "Strust4Embedded.h"
#define CONNECTED_TO_WIFI "WiFi connected to:"
static char *wifiLine=NULL;//[MAX_PAYLOAD_SIZE];
static char *jsonPayload=NULL;//[MAX_PAYLOAD_SIZE];
static THD_WORKING_AREA(waWifiCommunicationThd, 1024);
static BaseSequentialStream *pWiFiChannel = (BaseSequentialStream *)&WIFI_SD;
thread_t *pWifiThd;

#if !defined(LINE_WIFI_RST) || !defined(LINE_WIFI_RX) || !defined(LINE_WIFI_TX)
#error Plz define LINE_WIFI_RST, LINE_WIFI_RX, & LINE_WIFI_TX
#endif

#if defined(BOARD_BLACKBOARD_INDUSTRIAL2) && S4E_USE_ETHERNET == 1
#error With industrial board #2 you cannot use WIFI & MAC at the same time; there is pin conflict; i.e PA2. Remove MAC &LWIP from Hal.h & make files
#endif


static THD_FUNCTION(WifiCommunicationThd, arg) {(void)arg;
  chRegSetThreadName("WifiCommunicationThd");
  /*
  **It should be noted that we are using UART 5 with AF=8--
  **and all pins (LINE_WIFI_RX,LINE_WIFI_TX,LINE_WIFI_RST,LINE_EXT_RST)
  **have been already set to default in the board.c file
  **Note that SERIAL_BUFFERS_SIZE has been c=increased in halconf.h to handle large payloads smoothly, else
  **     you'll be skipping messages.Pay attention how the WIFI rest line is being bounced with delay of 100ms
   */
  //Clearing + Delay (100)+setting +delay(500) the WIFI_RST is critical
  pWifiThd = chThdGetSelfX();
#if defined(BOARD_BLACKBOARD_INDUSTRIAL2)
palSetLineMode(LINE_WIFI_TX, LINE_WIFI_MODE);
palSetLineMode(LINE_WIFI_RX, LINE_WIFI_MODE);
#endif
  palSetLineMode(LINE_WIFI_RST, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
  palClearLine(LINE_WIFI_RST);
  chThdSleepMilliseconds(100);
  palSetLine(LINE_WIFI_RST);
  chThdSleepMilliseconds(500);

  while (true) {
    uint8_t size = getSDLine(pWiFiChannel,wifiLine, MAX_PAYLOAD_SIZE);
    if ( size > 0 ){
         bool foundPayload = findJsonPayload(wifiLine,jsonPayload,MAX_PAYLOAD_SIZE);
         if ( foundPayload )
        	 handleActionPayloads(jsonPayload,SOURCE_EVENT_WIFI);
         if ( !foundPayload){
              dbgprintf("Debug from ESP8266:%s\r\n",wifiLine);
         }
     }
  }
}

void initWifiCommunicationThd(void){
   chDbgCheck( isActonEventThdInitialized() != false);
   wifiLine = chHeapAlloc(NULL,MAX_PAYLOAD_SIZE);
   chDbgCheck( wifiLine != NULL);
   jsonPayload = chHeapAlloc(NULL,MAX_PAYLOAD_SIZE);
   chDbgCheck( jsonPayload != NULL);
   chThdCreateStatic(waWifiCommunicationThd,  sizeof(waWifiCommunicationThd),  NORMALPRIO+10,     WifiCommunicationThd, "WifiCommunicationThd");
}
//Make sure one thread at a time sends a message to the wifi module
void sendWifiModuleMsg(char *msg){
	chprintf(pWiFiChannel,"%s", msg);
}
static char *tmpPayload = NULL;
void sendGetPropertiesMsgToWifiModule(char *host, char *url, char *fp){
	if ( tmpPayload == NULL )
		tmpPayload = chHeapAlloc(NULL,MAX_PAYLOAD_SIZE);

  chsnprintf(tmpPayload,MAX_PAYLOAD_SIZE,"%s{\"actionEvent\":\"postExternally\",\"host\":\"%s\",\"url\":\"%s\",\"httpMethod\":\"GET\",\"contentType\":\"text/html\",\"port\":\"443\",\"secure\":\"yes\",\"fp\":\"%s\",\"payload\":\"{}\"}%s",
                                          PAYLOAD_PREFIX,
                                          host,
                                          url,
                                          fp,
                                          PAYLOAD_SUFFIX);
  sendWifiModuleMsg(tmpPayload);

}
__attribute__((weak)) void initWifiModuleServer(void){
   char  payload[MAX_PAYLOAD_SIZE]	= {0};

   chsnprintf(payload,sizeof(payload),"%s{\"%s\":\"%s\",\"%s\":\"%s\",\"%s\":\"%s\",\"%s\":\"%s\"}%s",
		                                                                         PAYLOAD_PREFIX,
																				 APP_NAME_KEY,
																				 APP_NAME_VALUE,
																				 RELOAD_HOST_KEY,
																				 RELOAD_HOST_VALUE,
																				 RELOAD_URL_KEY,
																				 RELOAD_URL_VALUE,
																				 FP_KEY,
																				 FP_VALUE,
																				 PAYLOAD_SUFFIX);
   dbgprintf("%s\r\n",payload);
   sendWifiModuleMsg(payload);
}

#endif
