#include "Strust4EmbeddedConf.h"
#if USE_AE_SHELL != 0
#include "ch.h"
#include "hal.h"
#include "Strust4Embedded.h"
#include "ccportab.h"
#include "RTCHelper.h"
#include "AEShell.h"

CC_WEAK void cmd_invokeActionEvent(BaseSequentialStream *chp, int argc, char *argv[]) {
  (void)argv;
  if (argc == 0) {
    chprintf(chp, "Usage: <action event name> <data>\r\n");
    return;
  }
  char   *pAEName = argv[0];
  char   *pAEData = NULL;
  uint32_t data   = 0;
  if ( argc >=2 ){
	  pAEData = argv[1];
	  if ( isNumeric(pAEData) ){
		  data  = atoi(pAEData);
		  pAEData = NULL;
	  }
  }
  triggerActionEvent(pAEName,pAEData,data,"serial");
}

static const ShellCommand commands[] = {
  {"AE", cmd_invokeActionEvent},
  {NULL, NULL}
};

static const ShellConfig shell_cfg1 = {
  (BaseSequentialStream  *)&PORTAB_SD,
  commands
};
static THD_WORKING_AREA(waShell, 1024);


void initAEShell(void){
	  shellInit();
	  chThdCreateStatic(waShell, sizeof(waShell), NORMALPRIO,       shellThread, (void *)&shell_cfg1);
}
#endif
