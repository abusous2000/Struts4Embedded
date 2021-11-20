#include "ch.h"
#include "hal.h"
#include "ccportab.h"
#include "Strust4Embedded.h"

extern ActionEvent_Typedef 	 *gActionEvents[];
static objects_fifo_t 		 *pMainQueue 				  = NULL;
static guarded_memory_pool_t guardedDataBufferMemoryPool  = {0};
static dyn_objects_fifo_t 	 *dynObjMainQueueFIFO		  = NULL;
static map_t                 *actionEventsMap             = NULL;

ActionEvent_Typedef *sendActionEvent(char *aeName){
  ActionEvent_Typedef     *tmp = getActionEvent(aeName);//This is more like memcpy the AE definition, & later override the pData field, see triggerActionEventNoLock()

  if ( tmp != NULL){
	  ActionEvent_Typedef     *pActionEvent = (ActionEvent_Typedef*)chFifoTakeObjectI(pMainQueue);//Get a free object from pool

	  pActionEvent->processed = 0;
	  //Do memory copy ONLY after you receive a NULL pointer.
	 *pActionEvent =    *tmp;
	 chFifoSendObjectI(pMainQueue,pActionEvent);

	 return pActionEvent;
  }

  return NULL;
}
ActionEvent_Typedef *getActionEventByNdx(int ndx){
   return gActionEvents[ndx];
}
ActionEvent_Typedef **getActionEvents(void){
   return gActionEvents;
}
ActionEvent_Typedef *getActionEvent(char *aeName){
   ActionEvent_Typedef *hashEntry = NULL;
   map_t t = actionEventsMap;
   hashmap_get(actionEventsMap,(any_t)aeName,(any_t)&hashEntry);

   return hashEntry;
}

int8_t getActionEventNdx(char *aeName){
   ActionEvent_Typedef *pAE= getActionEvent(aeName);

   return pAE!=NULL?pAE->ndx:-1;
}

static void initActionEvents(void){
   dynObjMainQueueFIFO = chFactoryCreateObjectsFIFO(MAIN_ACTION_EVENTS_QUEUE,sizeof(ActionEvent_Typedef),MAIN_QUEUE_SIZE,PORT_NATURAL_ALIGN);
   pMainQueue = &(dynObjMainQueueFIFO->fifo);

   initStruts4EmbeddedFramework();
   return;
}
ActionEvent_Typedef *triggerActionEventNoLock(char *aeName, char *pData, uint32_t data, char *eventSource){
  if ( !isActonEventThdInitialized() ){
	 chSysHalt("ActonEventThd hasn't been initialized. Plz call initActonEventThd() in your startup code");
  }
  /*
   * Note that after this method is executed; the thread is not released yet.
   * It will be once chSchRescheduleS(); & chSysUnlock(); are done
   */
  ActionEvent_Typedef *pAE = sendActionEvent(aeName);

  if ( pAE == NULL )
	  return NULL;
  //Use memory pool only when needed & defined as such. If that was the case, then
  if ( pAE->dataType == CHAR_DTYPE)
     pAE->dataType = IS_CHAR_STR_NULL(pData)?INT_DTYPE:CHAR_DTYPE;
  //Always deep copy when char data as the type, else memory corruption will happen
  if ( pAE->dataType == CHAR_DTYPE){
	  pAE->u.pData = (char*)chGuardedPoolAllocI(&guardedDataBufferMemoryPool);
	  if ( pAE->u.pData == NULL){
		  chSysHalt("DataBufferMemoryPool full");
	  }
	  strlcpy(pAE->u.pData,pData,MAX_POOL_BUFFER_SIZE);
	  pAE->error = strlen(pData)> MAX_POOL_BUFFER_SIZE?DATA_FIELD_WAS_LT_MAX:0;//just in case truncation occurred
  }
  else
	  pAE->u.data   = data;

  if ( eventSource !=NULL)
     pAE->eventSource = eventSource;

  return pAE;
}

ActionEvent_Typedef *triggerActionEvent(char *aeName, char *pData, uint32_t data, char *eventSource){
  chSysLock();
  ActionEvent_Typedef *pAE = triggerActionEventNoLock(aeName,pData,data,eventSource);
  /*##############IMPORTANT###################
   * YOU MUST CALL chSchRescheduleS() method else you might crash; see comment @chSysUnlock(void) in chsys.h for details
   * The following condition can be triggered by the use of i-class functions
   * in a critical section not followed by a chSchRescheduleS(), this means
   * that the current thread has a lower priority than the next thread in
   * the ready list.
   * ##############IMPORTANT###################
   */
  chSchRescheduleS();
  chSysUnlock();

  if ( pAE == NULL )
	  dbgprintf("AE % doesn't exists!!\r\n",aeName);
  return pAE;
}
ActionEvent_Typedef *triggerActionEventFromISR(char *aeName, char *pData, uint32_t data, char *eventSource){
  chSysLockFromISR();
  ActionEvent_Typedef *pAE = triggerActionEventNoLock(aeName,pData,data,eventSource);
  chSysUnlockFromISR();

  return pAE;
}
CC_WEAK
int32_t defaultAction(ActionEvent_Typedef 	*pActionEvent){(void)pActionEvent;



   return MSG_OK;
}
CC_WEAK
int32_t defaultView(ActionEvent_Typedef 	*pActionEvent){(void)pActionEvent;

   dbgprintf("Executing defaultView\r\n");

   return MSG_OK;
}

static msg_t onProcessActionEvents(ActionEvent_Typedef 	*pAE){
   msg_t  retMsg = MSG_OK;

   dbgprintf("----Thd: %s is Processing AE: %s from:%s\r\n", chThdGetSelfX()->name, pAE->name, pAE->eventSource);
   if ( pAE->error == 0 ){
	   if ( pAE->action != NULL ){
		  retMsg = pAE->action(pAE);
		  if ( retMsg == MSG_OK){
			  if ( pAE->nextActionEventName != NULL ){
				  ActionEvent_Typedef *nextAE   = getActionEvent(pAE->nextActionEventName);
				  char 				  *tmpChar 	= pAE->dataType==CHAR_DTYPE?pAE->u.pData:NULL;
				  uint32_t  		  tmpData   = pAE->u.data;
				  uint8_t   		  dataType  = pAE->dataType;

				  *pAE  = *nextAE;//easiest way to memcpy() data structures
				  pAE->dataType = dataType;
				  if ( dataType==CHAR_DTYPE)
					  pAE->u.pData= tmpChar;
				  else
					  pAE->u.data = tmpData;

				  retMsg = pAE->action(pAE);
				  if ( pAE->nextActionEventName != NULL && retMsg == MSG_OK){
					  dbgprintf("chSysHalt: You can't chain more than 2 AEs, this feature not implemented yet!");
					  chSysHalt("chSysHalt: You can't chain more than 2 AEs, this feature not implemented yet!");
				  }
			  }
			  if ( retMsg == MSG_OK){
				  if ( pAE->view != NULL )
				     pAE->view(pAE);
			  }
		  }
	   }
	   else{
		   retMsg = defaultAction(pAE);
		   if ( retMsg == MSG_OK){
			   if ( pAE->view != NULL )
				   pAE->view(pAE);
			   else
				   defaultView(pAE);
		   }
	   }
   }
   else{
	   dbgprintf("\r\n***AE: %s was ignored because of error:%d\r\n", pAE->name, pAE->error);
	   switch(pAE->error){
	   	   case DATA_FIELD_WAS_LT_MAX:
	   		      dbgprintf("Possibly Data field was truncated to:%s\r\n", pAE->u.pData);
	   		    break;
	   }
   }

   pAE->processed = 1;
   if ( pAE->dataType == CHAR_DTYPE && pAE->u.pData != NULL){
	  chGuardedPoolFree(&guardedDataBufferMemoryPool, pAE->u.pData);
	  pAE->u.pData  = NULL;
	  pAE->dataType = INT_DTYPE;
   }
   chFifoReturnObject(pMainQueue,pAE);

   return retMsg;
}
static void blockUntilEventIsAvail(void){
    ActionEvent_Typedef   *pActionEvent = NULL;
    msg_t                 retMsg        = chFifoReceiveObjectTimeout(pMainQueue,(void **)&pActionEvent,TIME_INFINITE);

    if ( MSG_OK == retMsg)
        onProcessActionEvents(pActionEvent);

    return;
}
#define USE_HEAP_WORKER_THDS		FALSE
#if USE_HEAP_WORKER_THDS == FALSE
//PLACE_IN_RAM_SECTION(.nocache.waActonEventThd1)
static THD_WORKING_AREA(waActonEventThd1, PROCESS_ACTION_EVENTS_THD_WA);
#endif
THD_FUNCTION(ActonEventThd, arg) {
  char *threadName = (char *)arg;

  if ( chThdGetSelfX()->name ==  NULL || strcmp(chThdGetSelfX()->name,"noname") ==0 )
	  chRegSetThreadName(threadName);
  while (true) {
    blockUntilEventIsAvail();
  }
}
LinkedListElement* printAE(LinkedListElement* pItem, void *arg){(void)arg;
    ActionEvent_Typedef   *pActionEvent = (ActionEvent_Typedef*)pItem->value;
	dbgprintf("\r\n***AE: %s \r\n", pActionEvent->name);

	return NULL;
}
void initActonEventThd(void){
  initActionEvents();
  actionEventsMap 	= hashmap_new(MAX_ACTION_EVENTS);
  for(int i=0; i < MAX_ACTION_EVENTS; ++i){
	  if ( gActionEvents[i] != NULL){
		  ActionEvent_Typedef *ae= gActionEvents[i];
		  ae->ndx = i;
		  hashmap_put(actionEventsMap, gActionEvents[i]->name, ae);
	  }
  }
  void *dataPoolBuffer = chHeapAlloc(NULL,MAX_POOL_BUFFER_SIZE * MAX_DATA_POOL_BUFFERS);
  chGuardedPoolObjectInitAligned(&guardedDataBufferMemoryPool, MAX_POOL_BUFFER_SIZE, PORT_NATURAL_ALIGN);
  chGuardedPoolLoadArray(&guardedDataBufferMemoryPool, dataPoolBuffer, MAX_DATA_POOL_BUFFERS);
#if USE_HEAP_WORKER_THDS
  int maxNumOfThds = getSysPropertyIntWithDefault(NUM_OF_WORKER_THD_KEY,DEFAULT_NUM_OF_WORKER_THDS);
  maxNumOfThds = maxNumOfThds>MAX_NUM_OF_WORKER_THDS?MAX_NUM_OF_WORKER_THDS:maxNumOfThds;
  for(int i= 1; i <= maxNumOfThds; ++i){
	  char *thdName = chHeapAlloc(NULL,MAX_THD_NAME_SIZE);
	  chsnprintf(thdName,MAX_THD_NAME_SIZE,"ActonEventWorkerThd_%d",i);
	  chThdCreateFromHeap(NULL,THD_WORKING_AREA_SIZE(PROCESS_ACTION_EVENTS_THD_WA),thdName,  NORMALPRIO+11,    ActonEventThd, NULL);
  }
#else
  //Create as many Worker threads to handle AE as much as you need.
  //Plz note that for some reason, the stack got corrupted when I used the heap to create threads dynamically
  chThdCreateStatic(waActonEventThd1,  sizeof(waActonEventThd1),  NORMALPRIO+11,     ActonEventThd, "ActionEventWorkerThd_1");
  //chThdCreateStatic(waActonEventThd1,  sizeof(waActonEventThd2),  NORMALPRIO+11,     ActonEventThd, "ActonEventThd2");
  //chThdCreateStatic(waActonEventThd1,  sizeof(waActonEventThd3),  NORMALPRIO+11,     ActonEventThd, "ActonEventThd3");

#endif
}
bool isActonEventThdInitialized(void){
	return dynObjMainQueueFIFO != NULL;
}
