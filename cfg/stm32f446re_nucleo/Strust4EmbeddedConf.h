/*
 * Strust4Embedded.h
 *
 *  Created on: Dec 12, 2019
 *      Author: abusous2000
 */

#ifndef CFG_STRUST4EMBEDDEDCONF_H_
#define CFG_STRUST4EMBEDDEDCONF_H_

#ifndef S4E_USE_SDCARD
#define S4E_USE_SDCARD 			0
#endif

//#define MAX_ACTION_EVENTS                6
#define BLINKER_THD_STACK_SIZE             1024//512
#ifndef USE_LCD_TFT
#define USE_LCD_TFT 			0
#endif

#ifndef S4E_USE_JOYSTICK
#define S4E_USE_JOYSTICK        1
#endif

#ifndef S4E_USE_BLINKER_THD
#define S4E_USE_BLINKER_THD     1
#endif
#ifndef S4E_USE_SSD1306_LCD
#define S4E_USE_SSD1306_LCD     1
#endif

#ifndef S4E_USE_WIFI_MODULE_THD
#define S4E_USE_WIFI_MODULE_THD 0
#endif
#ifndef S4E_USE_POT
#define S4E_USE_POT             1
#endif

#ifndef S4E_USE_PWM
#define S4E_USE_PWM             0
#endif

#ifndef S4E_USE_RGB
#define S4E_USE_RGB             1
#endif

#ifndef S4E_USE_MQTT
#define S4E_USE_MQTT            0
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

#define PORTAB_SD                       SD2
#define MQTT_CMD_SUBSCRIBE_TOPIC   		"dev/cmd/STM32F446RE/mp3Player"
#define DEFAULT_MQTT_PUBLISH_TOPIC 		"dev/update/STM32F446RE/mp3Player"
#define MQTT_CLIENT_ID                  "Struts4EmbeddedWithStm32F446RE"


#define SSD1306_I2C_AF       			4
#define SSD1306_I2C_SCL_PORT 			GPIOB
#define SSD1306_I2C_SCL_PIN  			GPIOB_ARD_D15
#define SSD1306_I2C_SDA_PORT 			GPIOB
#define SSD1306_I2C_SDA_PIN  			GPIOB_ARD_D14
#define SSD1306_SA0_PORT 				GPIOA
#define SSD1306_SA0_PIN    				GPIOA_ARD_D8
#define SSD1306_RESET_PORT				GPIOA
#define SSD1306_RESET_PIN				GPIOA_ARD_D7

#define TIM_TGRO_EVENT_EXTSEL 		    0x08 //See p. 386 in RM
#define ADC_POT_TIM                     GPTD3 ///See p. 386 in RM
#define ADC_POT                     	ADCD1
#define ADC_CHANNEL_IN 					ADC_CHANNEL_IN0
#endif /* CFG_STRUST4EMBEDDED_H_ */
