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
#include "w25qxx.h"
#include "IRReceiver.h"
#include "CanBus.h"
#include "AEShell.h"
#if S4E_USE_USB_HID != 0
static void initUsbHid(void);
#include "usb_hid.h"
#endif
#if INCLUDE_SEGGER_JLINK != 0
#include "SEGGER_SYSVIEW_ChibiOS.h"
#include "SEGGER_RTT_streams.h"
#endif

#ifdef USE_USBCFG
#include "usbcfg.h"
static void initUSBCFG(void);
#endif

static void initDrivers(void);

#if DEBUG_TRACE_PRINT != 0
/*VCP Serial configuration*/
static const SerialConfig myserialcfg = {
  115200,
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
 SYSVIEW_ChibiOS_Start(STM32_SYSCLK, STM32_SYSCLK, "I#44=OSTick,I#53=USART1,I#54=USART2,I#76=ADC,I#22=K_UP,I#23=Button,I#26=K0,I#77=ETH/MAC");
// SEGGER_SYSVIEW_NameMarker(1,"setVolume");
#endif
}
#endif

int main(void) {
  initMain();
#if HAL_USE_SERIAL != 0
	#ifdef USE_USBCFG
	  initUSBCFG();
	#else
      #if (defined(BOARD_SEEED_ARCH_MAX) || defined(BOARD_BLACKBOARD_INDUSTRIAL2)) && STM32_SERIAL_USE_USART1 != 0
		  palSetLineMode(LINE_UART_SD_TX, PAL_MODE_ALTERNATE(7)| PAL_STM32_OSPEED_HIGHEST | PAL_STM32_OTYPE_PUSHPULL);
		  palSetLineMode(LINE_UART_SD_RX, PAL_MODE_ALTERNATE(7)| PAL_STM32_OSPEED_HIGHEST | PAL_STM32_OTYPE_PUSHPULL);
      #endif
      #if DEBUG_TRACE_PRINT != 0
	      sdStart(&PORTAB_SD, &myserialcfg);
      #endif
	#endif
#endif
  initDrivers();
  initActonEventThd();

  initButtonsLEDs();
#if S4E_USE_BLINKER_THD != 0
  initBlinkerThd(pGreenLedPAL);
#endif

#if S4E_USE_WIFI_MODULE_THD != 0
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
#if S4E_USE_IR_RECEIVER != 0
  initIRReeceiver();
#endif

#if S4E_USE_CAN_BUS != 0
  initCanBusThreads();
#endif

#if USE_AE_SHELL != 0
 initAEShell();
#endif
#if S4E_USE_USB_HID != 0
initUsbHid();
#endif

 while (true) {
	  chThdSleepMilliseconds(1500);
	  #if S4E_USE_MQTT != 0
	  if ( !isDefaultMQTTBrokerConnected() )
		  reconnectDefaultMQTTBroker();
	  #endif

	  #if S4E_USE_USB_HID != 0
	  if (usbhidcfg.usbp->state == USB_ACTIVE) {
	      uint8_t report;
	      size_t  n = hidGetReport(0, &report, sizeof(report));
	      hidWriteReport(&UHD, &report, n);
	      n = hidReadReportt(&UHD, &report, sizeof(report), TIME_IMMEDIATE);
	      if (n > 0)
	        hidSetReport(0, &report, n);
	  }
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
#if USE_W25Q_XXXX != 0
  uint8_t datatemp[20] = {0};
  W25QXX_Init();
  W25QXX_Erase_Sector(0);
//  W25QXX_Erase_Chip();
  W25QXX_SectorWrite((uint8_t*)"Hello World!",0,14);
//  W25QXX_Write_Page((uint8_t*)"Hello World!2",0,14);

  W25QXX_Read(datatemp,0,12);
  dbgprintf("From Flash:%s\r\n",datatemp);
#endif
#if USERLIB_USE_RF != 0
  initNRF2401();
#endif
#if HAL_USE_RTC != 0
    RTCInit();
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
       #if INCLUDE_SEGGER_JLINK != 0
	   SEGGER_SYSVIEW_PrintfHost("RTT %d\r\n", i);
       #endif
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

#if S4E_USE_USB_HID != 0
static void initUsbHid(void){
	  /*
	   * Initializes a USB HID driver.
	   */
	  hidObjectInit(&UHD);
	  hidStart(&UHD, &usbhidcfg);

	  /*
	   * Activates the USB driver and then the USB bus pull-up on D+.
	   * Note, a delay is inserted in order to not have to disconnect the cable
	   * after a reset.
	   */

	  usbDisconnectBus(usbhidcfg.usbp);
	  chThdSleepMilliseconds(1000);
	  usbStart(usbhidcfg.usbp, &usb_hid);
	  chThdSleepMilliseconds(1000);
	  usbConnectBus(usbhidcfg.usbp);
}
#endif

#if INCLUDE_SEGGER_JLINK != 0
extern void tname(void *arg);
extern void irReceiverThd(void *arg);
extern void BlinkerThd(void *arg);
extern void eByteLoraThread(void *arg);
extern void can_rx(void *arg);
extern void dummyDataSenderThd(void *arg);
extern void BlinkerThd(void *arg);
extern void lwip_thread(void *arg);

char *_getThreadName(tfunc_t pf){
	if ( pf == ActonEventThd)
		return "ActonEventThd";
#if S4E_USE_IR_RECEIVER != 0
	if ( pf == irReceiverThd)
		return "irReceiverThd";
#endif
#if S4E_USE_CAN_BUS != 0
	if ( pf == dummyDataSenderThd)
		return "dummyDataSenderThd";

	if ( pf == can_rx)
		return "can_rx";
#endif
#if S4E_USE_ETHERNET != 0
	if ( pf == lwip_thread)
		return "lwip_thread";
#endif
#if S4E_USE_BLINKER_THD != 0
	if ( pf == BlinkerThd)
		return "BlinkerThd";
#endif
#if S4E_USE_EBYTE_LORA != 0
	if ( pf == eByteLoraThread)
		return "eByteLoraThread";
#endif
#if USE_AE_SHELL != 0
	if ( pf == shellThread)
		return "shellThread";
#endif
	   return "noname";
}
#endif
