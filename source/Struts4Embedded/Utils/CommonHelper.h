#ifndef CommonHelper_h
#include "ActionEventsThd.h"
#include "hashmap.h"
#define NULL_INT_VALUE  			-1
#define IS_CHAR_STR_NULL(_x)       (_x == NULL || _x[0] == 0)
#define PLACE_IN_RAM_SECTION(__x)    __attribute__((section("__x")))   __attribute__((aligned(4)))
#define UNUSED(X) 				   (void)X      /* To avoid gcc/g++ warnings */
typedef struct HeapUsageInfo_S{
	size_t fragmantation;
	size_t total;
	size_t largest;
}HeapUsageInfoTypeDef;

#ifdef __cplusplus
 extern "C" {
#endif
HeapUsageInfoTypeDef *getHeapUsageInfo(void);
uint8_t getSDLine(BaseSequentialStream *chp, char *line, unsigned int size);
int32_t indexOf(char *source, char *in);
void substring(char s[], char sub[], int p, int l);
bool findJsonPayload(char *envelope, char *jsonPL, int size);
int freeNVP(any_t arg, any_t item);
void freeNVPMap(map_t mymap);
void initStruts4EmbeddedFramework(void);
char *getSysProperty(const char *key);
void putSysProperty(NameValuePairStaticTypeDef *pNVP);
char *getSysPropertyWithDefault(const char *key, char *defaultValue);
int getSysPropertyInt(const char *key);
int getSysPropertyIntWithDefault(const char *key, int defaultValue);
void onUpdateSysProperties(char *payload);
void handlePayload(ActionEventPayload_Typedef *pAEPayload, char *eventSouce);
bool handleActionPayloads(char *jsonPayload, char *eventSouce);
void *chibios_calloc(int num,int size);
void removeChar(char *str, char garbage);
bool isNumeric(const char *str);
#ifdef __cplusplus
}
#endif

#endif
