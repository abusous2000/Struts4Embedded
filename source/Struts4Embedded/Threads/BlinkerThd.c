#include "Strust4EmbeddedConf.h"
#if S4E_USE_BLINKER_THD != 0
#include "ch.h"
#include "hal.h"
#include "Strust4Embedded.h"
#include "ccportab.h"

static CPalTypedef  			*pLED =NULL;

//A weak/virtual function that could be overridden in other modules & called once every 1 seconds
CC_WEAK void periodicSysTrigger(uint32_t i){(void)i;
}
/*
 * If you are short on memory in RAM3 section, uncomment the below line
 */
//PLACE_IN_RAM_SECTION(.ram0.waBlinkerThd)
static THD_WORKING_AREA(waBlinkerThd, BLINKER_THD_STACK_SIZE);
THD_FUNCTION(BlinkerThd, arg) {
  (void)arg;
  chRegSetThreadName("blinker");

  for (uint32_t i = 0; true; ++i) {
      chThdSleepMilliseconds(BLINKER_THD_FREQ);
      if ( pLED != NULL){
    	  pLED->toggle(pLED);
      }
   	  periodicSysTrigger(i);
   }
}

void initBlinkerThd(CPalTypedef  *pPal){
//	chDbgCheck( isActonEventThdInitialized() != false);
    pLED = pPal;
    chThdCreateStatic(waBlinkerThd,      	sizeof(waBlinkerThd),        NORMALPRIO, BlinkerThd,     NULL);
}
#endif
