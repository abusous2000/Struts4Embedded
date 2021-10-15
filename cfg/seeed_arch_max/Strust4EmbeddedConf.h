/*
 * Strust4Embedded.h
 *
 *  Created on: Dec 12, 2019
 *      Author: abusous2000
 */

#ifndef CFG_STRUST4EMBEDDEDCONF_H_
#define CFG_STRUST4EMBEDDEDCONF_H_
#include "board.h"
//#define MAX_ACTION_EVENTS                6
#define BLINKER_THD_STACK_SIZE             1024//512
/*
 * Pins Used by Ethernet
 *  PA1  - ETH_RMII_REF_CLK
 *  PA2  - ETH_RMII_MDIO
 *  PA3  - ETH_RMII_MDINT
 *	PB11 - GPIOB_PIN11_ETH_TX_EN
 *	PB12 - GPIOB_PIN12_ETH_TX0
 *	PB13 - GPIOB_PIN13_ETH_TX1
 *  PC1  - ETH_RMII_MDC
 *  PC4  - ETH_RMII_RXD0
 *  PC5  - ETH_RMII_RXD1
 */

#define S4E_USE_PPM_FRAME_DECODER 		0
#define S4E_USE_EBYTE_LORA 		0
#define EBYTE_LORA_SERVER       0
#define S4E_USE_IR_RECEIVER		0
#define USE_W25Q_XXXX           0
#define USERLIB_USE_RF          0

#ifndef USE_LCD_TFT
#define USE_LCD_TFT 			0
#endif

#ifndef S4E_USE_JOYSTICK
#define LINE_JOY_UP             LINE_ARD_A1//PAL_LINE(GPIOA, 3U)-PA3
#define LINE_JOY_RIGHT          LINE_ARD_A2//PAL_LINE(GPIOA, 4U)-PA4
#define LINE_JOY_LEFT           LINE_ARD_A3//PAL_LINE(GPIOA, 5U)-PA5
#define LINE_JOY_CENTER         LINE_ARD_A4//PAL_LINE(GPIOA, 6U)-PA6
#define LINE_JOY_DOWN           LINE_ARD_A5//PAL_LINE(GPIOB, 0U)-PB0
#define S4E_USE_JOYSTICK        1
#endif

#ifndef S4E_USE_BLINKER_THD
#define S4E_USE_BLINKER_THD     1
#endif

#ifndef S4E_USE_WIFI_MODULE_THD
#define S4E_USE_WIFI_MODULE_THD 0
#endif

#ifndef S4E_USE_SSD1306_LCD
#define S4E_USE_SSD1306_LCD     1

#define SSD1306_I2C_SCL_LINE    PAL_LINE(GPIOB, GPIOB_ARD_D15)
#define SSD1306_I2C_SDA_LINE   	PAL_LINE(GPIOB, GPIOB_ARD_D14)
#define SSD1306_SA0_LINE        PAL_LINE(GPIOB, GPIOB_ARD_D8)
#define SSD1306_RESET_LINE      PAL_LINE(GPIOB, GPIOB_ARD_D7)
#define SSD1306_I2C_AF       	4
#define SSD1306_I2C_LINE_MODE	PAL_MODE_ALTERNATE(SSD1306_I2C_AF)  | PAL_STM32_OSPEED_HIGHEST|  PAL_STM32_OTYPE_OPENDRAIN
#define SSD1306_LINE_MODE   	PAL_STM32_MODE_OUTPUT  | PAL_STM32_PUPDR_PULLUP

#endif

#ifndef S4E_USE_POT
#define TIM_TGRO_EVENT_EXTSEL 	0x08 //See p. 386 in RM
#define ADC_POT_TIM             GPTD3 ///See p. 386 in RM
#define ADC_POT                 ADCD1
#define ADC_CHANNEL_IN 			ADC_CHANNEL_IN0//See Table 7 in user manual or datasheet
#define POT_GPIO_PIN            LINE_ARD_A0//PAL_LINE(GPIOA, 0U)-PA0
#define S4E_USE_POT             1
#endif

#ifndef S4E_USE_PWM
#define S4E_USE_PWM             0
#define PWM_LINE1 				PAL_LINE(GPIOC, 6U)//PC6
#define PWM_LINE2 				PAL_LINE(GPIOC, 7U)//PC7
#define PWM_LINE1_CH 			0
#define PWM_LINE2_CH 			1
#define PWM_LINE_AF   			3
#define PWM_DRIVER 				PWMD8

#endif
//With This board, you cannot used ARD_11 & ETH, there is a conflict
#ifndef S4E_USE_BUZZER
#define S4E_USE_BUZZER			0
#endif

#ifndef S4E_USE_RGB
#define LINE_IO_DATA		    LINE_ARD_D5//PAL_LINE(GPIOA, 8U)-PD5
#define LINE_IO_CLOCK		    LINE_ARD_D6//PAL_LINE(GPIOB, 6U)-PD6
#define S4E_USE_RGB             1
#endif

#ifndef S4E_USE_MQTT
#define S4E_USE_MQTT            1
#endif

#ifndef S4E_USE_WEB_SERVER
#define S4E_USE_WEB_SERVER      0
#endif

#ifndef S4E_USE_FAULT_HANDLER
#define S4E_USE_FAULT_HANDLER   1
#endif

#if S4E_USE_WEB_SERVER == 1 || S4E_USE_MQTT  == 1
#define S4E_USE_ETHERNET		1
#else
#define S4E_USE_ETHERNET		0
#endif

#define SSID_KEY						"ssid"
#define SSID_VALUE						"ROR194"
#define APP_NAME_KEY					"appName"
#define APP_NAME_VALUE					"MP3Player w/ S4E"
#define PWD_KEY							"pwd"
#define PWD_VALUE						"salah987"
#define RELOAD_HOST_KEY					"reloadHost"
#define RELOAD_HOST_VALUE				"raw.githubusercontent.com"
#define RELOAD_URL_KEY					"reloadURL"
#define RELOAD_URL_VALUE				"/abusous2000/MP3PlayerUsingSTM32F7AndChibiOS/master/source/ESP8266WiFiModule/defaultPage.html"
#define FP_KEY							"fp"
#define FP_VALUE						"CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33"
#define NUM_OF_WORKER_THD_KEY			"numOfWorkerThds"
#define NUM_OF_WORKER_THD_VALUE 		"1"
#define DEFAULT_NUM_OF_WORKER_THDS 		1
#define MAX_THD_NAME_SIZE          		25
#define MAX_NUM_OF_WORKER_THDS			4
//Note that for this board, serial output uses PA2 & PA3 (w/ AF 7) which is in conflict with Ethernet with PA2
#define PORTAB_SD                       SD6
#define MQTT_CMD_SUBSCRIBE_TOPIC   		"dev/cmd/STM32F407BlackBoard/mp3Player"
#define DEFAULT_MQTT_PUBLISH_TOPIC 		"dev/update/STM32F407BlackBoard/mp3Player"
#define MQTT_CLIENT_ID                  "Struts4EmbeddedWithSeeedArchMax"

#//brown, black, red, & orange
#define LINE_LED_GREEN                  LINE_LED//PAL_LINE(GPIOA, 6U)-PA6
//#define LINE_LED_RED                    PAL_LINE(GPIOA, 7U)
//For this to work, plz connect Pin20 to Pin13 in the Zigbee header on the waveshare accessory sheild
#define EASYLINK_BUTTON		            LINE_ARD_D9//PB14

#ifndef S4E_USE_SDCARD
#define S4E_USE_SDCARD 			        1

#define MMC_SPI_PIN_MODE                PAL_MODE_ALTERNATE(MMC_SPI_AF) 	  |  PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUPDR_PULLUP
#define MMC_SPI_PIN_MODE_MISO           MMC_SPI_PIN_MODE
#define PROTAB_MMCSPI                   SPID2
#define MMC_SPI_AF						5
#define SD_CARD_NSS_LINE 				PAL_LINE(GPIOE, 2U)//PE2
#define SD_CARD_SCK_LINE 				PAL_LINE(GPIOB, 10U)//PB10
#define SD_CARD_MISO_LINE 				PAL_LINE(GPIOC, 2U)//PC2
#define SD_CARD_MOSI_LINE 				PAL_LINE(GPIOC, 3U)//PC3
#endif

#endif /* CFG_STRUST4EMBEDDED_H_ */
