#include "ch.h"
#include "hal.h"
#include "Strust4Embedded.h"

static mutex_t printfMutex;
mutex_t *pPrintfMutex = NULL;

void removeChar(char *str, char garbage) {

    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != garbage) dst++;
    }
    *dst = '\0';
}
void _print_message(const char *str){
	dbgprintf("_____SYSTEM____MESSAGE_____\r\n");
	dbgprintf("%s\r\n",str);
	dbgprintf("_____SYSTEM____MESSAGE_____\r\n");
}

void *chibios_calloc(int num,int size){
  unsigned char *ptr;
  int i;
  ptr=chHeapAlloc(NULL, size*num);
  if (!ptr) return NULL;
  for(i=0;i<num*size;i++) ptr[i]=0;
  return ptr;
}

//C substring function definition
void substring(char s[], char sub[], int p, int l) {
   int c = 0;

   while (c < l) {
      sub[c] = s[p+c-1];
      c++;
   }
   sub[c] = '\0';
}
int32_t indexOf(char *source, char *in){
  const char *ptr = strstr(source, in);
  return ptr != NULL? (ptr-source): -1;
}
uint8_t getSDLine(BaseSequentialStream *chp, char *line, unsigned int size){
  char *p = line;
  uint8_t i = 0;

  for( i = 0; i < (size -1);){
    char c;
    if (streamRead(chp, (uint8_t *)&c, 1) == 0)
      return true;
    if (c == '\r' || c == '\n') {
      *p = 0;
      break;
    }
    //ignore below 0x20
    if (c >= 0x20){
      *p++ = (char)c;
       ++i;
    }
  }

  if ( *p != 0 )
    *(++p) = 0;
  return i;
}
int freeNVP(any_t arg, any_t item){(void)arg;
	NameValuePairTypeDef *hashEntry =	(NameValuePairTypeDef*)item;

	free(item);
	return MAP_OK;
}

void freeNVPMap(map_t mymap){
    hashmap_iterate(mymap,freeNVP,NULL);
    hashmap_free(mymap);
}
map_t *sysProperties = NULL;

static NameValuePairStaticTypeDef reloadHost=  {.key=RELOAD_HOST_KEY,				.value=RELOAD_HOST_VALUE};
static NameValuePairStaticTypeDef reloadURL =  {.key=RELOAD_URL_KEY,				.value=RELOAD_URL_VALUE};
static NameValuePairStaticTypeDef app 		=  {.key=APP_NAME_KEY,					.value=APP_NAME_VALUE};
static NameValuePairStaticTypeDef fp 	    =  {.key=FP_KEY,						.value=FP_VALUE};
static NameValuePairStaticTypeDef ssid 		=  {.key=SSID_KEY,						.value=SSID_VALUE};
static NameValuePairStaticTypeDef pwd 		=  {.key=PWD_KEY,						.value=PWD_VALUE};
static NameValuePairStaticTypeDef workerThds=  {.key=NUM_OF_WORKER_THD_KEY,			.value=NUM_OF_WORKER_THD_VALUE};
void initStruts4EmbeddedFramework(void){
	if ( sysProperties != NULL )
		return;
	sysProperties = hashmap_new(20);
#if !defined(STM32F407xx) || defined(BOARD_SEEED_ARCH_MAX)
	pPrintfMutex = &printfMutex;
	chMtxObjectInit(pPrintfMutex);
#endif
	putSysProperty(&ssid);
	putSysProperty(&pwd);
	putSysProperty(&app);
	putSysProperty(&reloadURL);
	putSysProperty(&reloadHost);
	putSysProperty(&fp);
	putSysProperty(&workerThds);
#if HAL_USE_RTC != 0
    RTCInit();
#endif
	return;
}
void putSysProperty(NameValuePairStaticTypeDef *pNVP){
	hashmap_put(sysProperties, (char*)pNVP->key, 		(any_t)pNVP);
}

char *getSysProperty(const char *key){
	NameValuePairStaticTypeDef *hashEntry = NULL;
	hashmap_get(sysProperties,(char*)key,(any_t)&hashEntry);

	return hashEntry!=NULL?(char*)hashEntry->value:(char*)NULL;
}
char *getSysPropertyWithDefault(const char *key, char *defaultValue){
	char *value = getSysProperty(key);

	return value!=NULL?value:defaultValue;
}
int getSysPropertyInt(const char *key){
	char *value = getSysProperty(key);

	return value!=NULL?atoi(value):NULL_INT_VALUE;
}
int getSysPropertyIntWithDefault(const char *key, int defaultValue){
	int value = getSysPropertyInt(key);

	return value!=NULL_INT_VALUE?value:defaultValue;
}


static HeapUsageInfoTypeDef heapUsageInfo={0};
HeapUsageInfoTypeDef *getHeapUsageInfo(void) {
	heapUsageInfo.fragmantation = chHeapStatus(NULL, &heapUsageInfo.total , &heapUsageInfo.largest);

	dbgprintf("core free memory : %u bytes\r\n", chCoreGetStatusX());

	dbgprintf("Core Memory (contiguous unallocated space)   : 	%u\r\n",       chCoreGetStatusX());
	dbgprintf("heap fragments   : 								%u\r\n",       heapUsageInfo.fragmantation);
	dbgprintf("heap free total  : 								%u bytes\r\n", heapUsageInfo.total);
	dbgprintf("heap free largest: 								%u bytes\r\n", heapUsageInfo.largest);

	return &heapUsageInfo;
}
void handlePayload(ActionEventPayload_Typedef *pAEPayload, char *eventSource){
   ActionEvent_Typedef   *pAE = getActionEvent(pAEPayload->actionEvent);
   if ( pAE!=NULL)
	 triggerActionEvent(pAEPayload->actionEvent,pAEPayload->data,0,eventSource);
   else
   if ( pAEPayload->actionEvent[0] != 0)
	 dbgprintf("AE 404:%s\r\n",pAEPayload->actionEvent);
}

bool handleActionPayloads(char *jsonPayload, char *eventSouce ){
	bool handled = false;
   	if ( indexOf(jsonPayload,"actionEvent") > 0){
		ActionEventPayload_Typedef aePayload = {0};

		if( parseActionEventPayload(jsonPayload,&aePayload) != NULL){
			handlePayload(&aePayload,eventSouce);
			handled = true;
		}
	}
	else
	if ( indexOf(jsonPayload,"nameValuePair") > 0){
		onUpdateSysProperties(jsonPayload);
		handled = true;
	}

   	return handled;
}
__attribute__((weak)) void onUpdateSysProperties(char *payload){
    map_t *mymap = hashmap_new(16);
	parseMap(payload,mymap);

	NameValuePairTypeDef *hashEntry = NULL;
	hashmap_get(mymap,(any_t)"ssid",(any_t)&hashEntry);
	freeNVPMap(mymap);
}
