/*
 * ActionEventsViews.c
 *
 *  Created on: Dec 27, 2019
 *      Author: abusous2000
 */
#include "Strust4EmbeddedConf.h"
#include "ch.h"
#include "hal.h"
#include "ActionEvents.h"
#include "ssd1306.h"
#include "Strust4Embedded.h"

void updateScreen(void){
#if S4E_USE_SSD1306_LCD != 0
   char  buff[20]={0};

   chsnprintf(buff, sizeof(buff),"Vol:%d",getCurrentVolume());
   LCD_Display_String(buff,30, false);
   chsnprintf(buff, sizeof(buff),"Mute:%d",getCurrentMute());
   LCD_Display_String(buff,44, false);
   LCD_Display_Update();
#endif
   return;
}
int32_t loadSysPropertiesView(ActionEvent_Typedef 	*pActionEvent){(void)pActionEvent;
   dbgprintf("***sent loadSysPropertiesView\r\n");

   return MSG_OK;
}
int32_t updateWifiHtmlView(ActionEvent_Typedef 	*pActionEvent){(void)pActionEvent;
   dbgprintf("***sent WifiModule msg to refresh its config properties\r\n");
   return MSG_OK;
}
int32_t toggleMuteView(ActionEvent_Typedef 	*pActionEvent){(void)pActionEvent;
	dbgprintf("***Setting Mute: %d\r\n",getCurrentMute());
	updateScreen();

	return MSG_OK;
}
int32_t volumeView(ActionEvent_Typedef 	*pActionEvent){(void)pActionEvent;
   dbgprintf("Vol.: %d\r\n", getCurrentVolume());
   updateScreen();

   return MSG_OK;
}
int32_t setRGBLEDView(ActionEvent_Typedef 	*pActionEvent){(void)pActionEvent;
   dbgprintf("Set RGB.: %s\r\n", pActionEvent->u.pData);

   return MSG_OK;
}
int32_t setPWMParamsView(ActionEvent_Typedef 	*pActionEvent){(void)pActionEvent;
   dbgprintf("Set Duty Cycle.: %s\r\n", pActionEvent->u.pData);

   return MSG_OK;
}

