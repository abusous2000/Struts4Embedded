#ifndef WifiCommunicationThd_h
#define WifiCommunicationThd_h

#include "Strust4EmbeddedConf.h"

#if S4E_USE_WIFI_MODULE_THD == 1
#include "JsonParser.h"
#ifndef WIFI_SD
#define WIFI_SD                 SD5
#endif
#define MAX_PAYLOAD_SIZE        512
#define WIFI_SERIALBAUD_RATE    115200
#define PROPERTIES_HOST		    "raw.githubusercontent.com"
#define PROPERTIES_URL          "/abusous2000/Struts4Embedded/master/NVP.json"
#define PROPERTIES_FP		    "70 94 DE DD E6 C4 69 48 3A 92 70 A1 48 56 78 2D 18 64 E0 B7"


#ifdef __cplusplus
 extern "C" {
#endif
void sendGetPropertiesMsgToWifiModule(char *host, char *url, char *fp);
void initWifiCommunicationThd(void);
void sendWifiModuleMsg(char *msg);
void initWifiModuleServer(void);
void onUpdateSysProperties(char *payload);
#ifdef __cplusplus
}
#endif

extern thread_t *pWifiThd;
#endif/*S4E_USE_WIFI_MODULE_THD*/

#endif
