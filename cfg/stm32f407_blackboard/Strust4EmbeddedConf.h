/*
 * Strust4Embedded.h
 *
 *  Created on: Dec 12, 2019
 *      Author: abusous2000
 */

#ifndef CFG_STRUST4EMBEDDEDCONF_H_
#define CFG_STRUST4EMBEDDEDCONF_H_
/*
 * Pins Used by Ethernet
MDIO	<=>		PA2 --black
MDC	<=>			PC1---white
nINT/RETCLK	<=>	PA1---Eggplant
CRS	<=>			PA7---Gray
RX0	<=>			PC4---blue
RX1	<=>			PC5-- green
TX_EN	<=>		PB11--orange
TX0	<=>			PB12--yellow
TX1	<=>			PB13--Eggplant
 */
//#define MAX_ACTION_EVENTS                6
#define BLINKER_THD_STACK_SIZE             1024//512
#define S4E_USE_PPM_FRAME_DECODER 		0
#define S4E_USE_EBYTE_LORA 		0
#define EBYTE_LORA_SERVER       0
#define S4E_USE_IR_RECEIVER		0

#ifndef S4E_USE_SDCARD
#define S4E_USE_SDCARD 			1
#endif
#ifndef USE_LCD_TFT
#define USE_LCD_TFT 			0
#endif
#ifndef S4E_USE_JOYSTICK
#define S4E_USE_JOYSTICK        0
#endif
#ifndef S4E_USE_BLINKER_THD
#define S4E_USE_BLINKER_THD     1
#endif
#ifndef S4E_USE_WIFI_MODULE_THD
#define S4E_USE_WIFI_MODULE_THD 0
#endif
#ifndef S4E_USE_SSD1306_LCD
#define S4E_USE_SSD1306_LCD     0
#endif
#ifndef S4E_USE_POT
#define S4E_USE_POT             1
#endif
#ifndef S4E_USE_PWM
#define S4E_USE_PWM             0
#endif
//With This board, you cannot used ARD_11 & ETH, there is a conflict
#ifndef S4E_USE_BUZZER
#define S4E_USE_BUZZER			0
#endif
#ifndef S4E_USE_RGB
#define S4E_USE_RGB             0
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
#define PORTAB_SD                       SD2
#define MQTT_CMD_SUBSCRIBE_TOPIC   		"dev/cmd/STM32F407BlackBoard/mp3Player"
#define DEFAULT_MQTT_PUBLISH_TOPIC 		"dev/update/STM32F407BlackBoard/mp3Player"
#define MQTT_CLIENT_ID                  "Struts4EmbeddedWithSTM32F407BlackBoard"

#define SSD1306_I2C_AF       			4
#define SSD1306_I2C_SCL_PORT 			GPIOB
#define SSD1306_I2C_SCL_PIN  			GPIOB_ARD_D15
#define SSD1306_I2C_SDA_PORT 			GPIOB
#define SSD1306_I2C_SDA_PIN  			GPIOB_ARD_D14
#define SSD1306_SA0_PORT 				GPIOF
#define SSD1306_SA0_PIN    				GPIOF_ARD_D8
#define SSD1306_RESET_PORT				GPIOF
#define SSD1306_RESET_PIN				GPIOF_ARD_D7
//brown, black, red, & orange
#define LINE_LED_GREEN                  PAL_LINE(GPIOA, 6U)
//#define LINE_LED_RED                    PAL_LINE(GPIOA, 7U)

#define TIM_TGRO_EVENT_EXTSEL 		    0x08 //See p. 386 in RM
#define ADC_POT_TIM                     GPTD3 ///See p. 386 in RM
#define ADC_POT                     	ADCD1
#define ADC_CHANNEL_IN 					ADC_CHANNEL_IN3//See Table 10 in user manual or datasheet
//#define LINE_ARD_D2                 PAL_LINE(GPIOA, 10U)
#define POT_GPIO_PIN                    PAL_LINE(GPIOA, 3U)

#define MPU_SCL_PIN						PAL_LINE(GPIOB, 10U)//LINE_ARD_D15
#define MPU_SDA_PIN						PAL_LINE(GPIOB, 11U)//LINE_ARD_D14
#define MPU_I2C_AF						4
#define MPU_I2CD						I2CD2

#define MPU_INT_PORT					GPIOA
#define MPU_INT_PIN						10
#ifndef LINE_JOY_UP
#define USER_BUTTON                     PAL_LINE(GPIOA, 0U)
#define USER2_BUTTON                    PAL_LINE(GPIOE, 4U)//K0
#define USER3_BUTTON                    PAL_LINE(GPIOE, 3U)//K1

#include "board.h"
#define SDMMCD0_LINE 					LINE_SD_D0		//  PAL_LINE(GPIOG,9)
#define SDMMCD1_LINE 					LINE_SD_D1		//  PAL_LINE(GPIOG,10)
#define SDMMCD2_LINE 					LINE_SD_D2		//  PAL_LINE(GPIOB,3)
#define SDMMCD3_LINE 					LINE_SD_D3		//  PAL_LINE(GPIOB,4)
#define SDMMCCK_LINE 					LINE_SD_CLK		//  PAL_LINE(GPIOD,6)
#define SDMMCMD_LINE 					LINE_SD_CMD		//  PAL_LINE(GPIOD,7)
//#define SDMMMC_DETECT   				LINE_SD_DETECT	////PAL_LINE(GPIOI,15)


#define SDMMC_ALREADY_CONFIG            1
#endif
#endif /* CFG_STRUST4EMBEDDED_H_ */
