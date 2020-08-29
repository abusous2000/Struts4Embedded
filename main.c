#include "Strust4EmbeddedConf.h"
#include "ch.h"
#include "hal.h"
#include "Strust4Embedded.h"
#include "PWM.h"
#include "P9813_RGB_Driver.h"
#include "ssd1306.h"
#include "ButtonLEDs.h"
#include "PotReader.h"
#include "MQTTClient.h"
#include "SDCard.h"
#include "PPMFrameDecoder.h"


#ifdef USE_USBCFG
#include "usbcfg.h"
static void initUSBCFG(void);
#endif

static void initDrivers(void);


/*VCP Serial configuration*/
static const SerialConfig myserialcfg = {
  115200,
  0,
  USART_CR2_STOP1_BITS,
  0
};
#if S4E_USE_WIFI_MODULE_THD
/* WiFi Serial configuration. */
static const SerialConfig wifiSerialvfg = {
  WIFI_SERIALBAUD_RATE,
  0,
  USART_CR2_STOP1_BITS,
  0
};
#endif

#if DEBUG_TRACE_PRINT == 1
BaseSequentialStream *GlobalDebugChannel = (BaseSequentialStream *)&PORTAB_SD;
#endif
void initWifiModuleServer(void);

void initMain(void);
#ifndef __cplusplus
void initMain(void){
 halInit();
 chSysInit();
}
#endif
int main(void) {
  initMain();
#if HAL_USE_SERIAL == 1
	#ifdef USE_USBCFG
	  initUSBCFG();
	#else
	  sdStart(&PORTAB_SD, &myserialcfg);
	#endif
#endif
  initDrivers();
  initActonEventThd();

  initButtonsLEDs();
#if S4E_USE_BLINKER_THD == TRUE
  initBlinkerThd(pGreenLedPAL);
#endif

#if S4E_USE_WIFI_MODULE_THD == TRUE
  sdStart(&WIFI_SD, &wifiSerialvfg);
  initWifiCommunicationThd();
#endif
#if S4E_USE_PWM == TRUE
  initPWM();
#endif
#if S4E_USE_ETHERNET == 1
  initMQTTClient();
#endif
#if PPM_FRAME_DECODER != 0
 initPPMFrameDecoder();
#endif
#if S4E_USE_SDCARD == 1
  SDCardDriverITF_Typedef *pSDCardDriverITF 			= getSDCardDriver();

  pSDCardDriverITF->init(pSDCardDriverITF);
  if ( !pSDCardDriverITF->mount(pSDCardDriverITF))
      pSDCardDriverITF->processFiles(pSDCardDriverITF);
  else
	  dbgprintf("Failed to mount SD card\r\n");
#endif
  while (true) {
      chThdSleepMilliseconds(250);
  }
}
#if S4E_USE_SDCARD == 1
static NameValuePairStaticTypeDef readFilesFromFolder=  {.key=READ_FILES_FROM_FOLDER,	.value="/music"};
#endif
static void initDrivers(void){
  initStruts4EmbeddedFramework();
#if S4E_USE_SDCARD == 1
  putSysProperty(&readFilesFromFolder);
#endif
#if S4E_USE_RGB == 1
  initP9813RGBDriver(TOTAL_NUM_OF_LEDS);
#endif
#if S4E_USE_POT == TRUE
  initPotReader();
#endif
#if S4E_USE_SSD1306_LCD == TRUE
  ssd130InitAndConfig("Struts4Embedded!");
#endif
}
void publishStatusToBroker(void);
void periodicSysTrigger(uint32_t i){(void)i;
#if S4E_USE_POT == TRUE
   checkOnPotVolumeChange();
#endif
#if S4E_USE_MQTT == 1
   if ( i > 0 && i % 8 == 0 )
	   publishStatusToBroker();
#endif
}
#ifdef USE_USBCFG
static void initUSBCFG(void){
	/*
	* Initializes a serial-over-USB CDC driver.
	*/
	sduObjectInit(&PORTAB_SDU);
	sduStart(&PORTAB_SDU, &serusbcfg);
	chThdSleepMilliseconds(300);

	usbDisconnectBus(serusbcfg.usbp);
	chThdSleepMilliseconds(1500);
	usbStart(serusbcfg.usbp, &usbcfg);
	usbConnectBus(serusbcfg.usbp);

	sdStart(&PORTAB_SD_VCP, &myserialcfg);
}
#endif

