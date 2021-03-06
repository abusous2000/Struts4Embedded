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
#include "EByteLora.h"
#include "RTCHelper.h"


#if INCLUDE_SEGGER_JLINK != 0
#include "SEGGER_SYSVIEW_ChibiOS.h"
#include "SEGGER_RTT_streams.h"
#endif

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

#if DEBUG_TRACE_PRINT != 0
BaseSequentialStream *GlobalDebugChannel = (BaseSequentialStream *)&PORTAB_SD;
#endif
void initWifiModuleServer(void);

void initMain(void);
#ifndef __cplusplus
void initMain(void){
 halInit();
 chSysInit();

#if INCLUDE_SEGGER_JLINK != 0
 /*
  * This is optional. You need to initialize this if you will be using the
  * RTT sequential streams (RTTD0). You *don't* need to use them; you can
  * use RTT directly (call SEGGER_RTT_xxxx) without any initialization.
  *
  *
  * Initialize RTT (channel 0). The output can be seen on Telnet por 19201.
  */
 rttInit();
 rttSetUpFlags(&RTTD0, RTT_MODE_FLAGS_BLOCK_IF_FIFO_FULL);

 /*
  * Start SystemView
  */
 SYSVIEW_ChibiOS_Start(STM32_SYSCLK, STM32_SYSCLK, "I#44=OSTick,I#53=USART1,I#54=USART2,I#76=ADC,I#56=Button,I#77=ETH/MAC");
#endif
}
#endif
int main(void) {
  initMain();
#if HAL_USE_SERIAL != 0
	#ifdef USE_USBCFG
	  initUSBCFG();
	#else
      #if defined(BOARD_SEEED_ARCH_MAX) && STM32_SERIAL_USE_USART1 != 0
	  palSetLineMode(LINE_UART_SD_TX,  PAL_MODE_ALTERNATE(7)| PAL_STM32_OSPEED_HIGHEST | PAL_STM32_OTYPE_PUSHPULL);
	  palSetLineMode(LINE_UART_SD_RX, PAL_MODE_ALTERNATE(7)| PAL_STM32_OSPEED_HIGHEST | PAL_STM32_OTYPE_PUSHPULL);
      #endif
	  sdStart(&PORTAB_SD, &myserialcfg);
	#endif
#endif
  initDrivers();
  initActonEventThd();

  initButtonsLEDs();
#if S4E_USE_BLINKER_THD != 0
  initBlinkerThd(pGreenLedPAL);
#endif

#if S4E_USE_WIFI_MODULE_THD != 0
  sdStart(&WIFI_SD, &wifiSerialvfg);
  initWifiCommunicationThd();
#endif
#if S4E_USE_PWM != 0
  initPWM();
#endif
#if S4E_USE_MQTT != 0
  initMQTTClient();
#endif

#if S4E_USE_SDCARD != 0
  SDCardDriverITF_Typedef *pSDCardDriverITF 			= getSDCardDriver();

  pSDCardDriverITF->init(pSDCardDriverITF);
  if ( !pSDCardDriverITF->mount(pSDCardDriverITF))
      pSDCardDriverITF->processFiles(pSDCardDriverITF);
  else
	  dbgprintf("Failed to mount SD card\r\n");
#endif

#if S4E_USE_PPM_FRAME_DECODER != 0
 initPPMFrameDecoder();
#endif

#if S4E_USE_EBYTE_LORA != 0
  initEByteLoraThread();
#endif

  while (true) {
	  chThdSleepMilliseconds(1500);
	  #if S4E_USE_MQTT != 0
	  if ( !isDefaultMQTTBrokerConnected() )
		  reconnectDefaultMQTTBroker();
	  #endif
  }
}
#if S4E_USE_SDCARD != 0
static NameValuePairStaticTypeDef readFilesFromFolder=  {.key=READ_FILES_FROM_FOLDER,	.value="/music"};
#endif
static void initDrivers(void){
  initStruts4EmbeddedFramework();
#if S4E_USE_SDCARD != 0
  putSysProperty(&readFilesFromFolder);
#endif
#if S4E_USE_RGB != 0
  initP9813RGBDriver(TOTAL_NUM_OF_LEDS);
#endif
#if S4E_USE_POT != 0
  initPotReader();
#endif
#if S4E_USE_SSD1306_LCD != 0
  ssd130InitAndConfig("Struts4Embedded!");
#endif
}
void publishStatusToBroker(void);
void periodicSysTrigger(uint32_t i){(void)i;

   #if S4E_USE_POT == TRUE
   checkOnPotVolumeChange();
   #endif

   if ( i > 0 && i % 8 == 0 ){
	   #if S4E_USE_MQTT != 0
	   publishStatusToBroker();
	   #endif
	   #if S4E_USE_EBYTE_LORA != 0 && EBYTE_LORA_SERVER != 0
	   eByteLoraSendFrame(NONE_BUTTON);
       #endif
	   SEGGER_SYSVIEW_PrintfHost("RTT %d\r\n", i);
   }

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
