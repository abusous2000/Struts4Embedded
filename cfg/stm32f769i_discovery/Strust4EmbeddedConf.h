/*
 * Strust4Embedded.h
 *
 *  Created on: Dec 12, 2019
 *      Author: abusous2000
 */

#ifndef CFG_STRUST4EMBEDDEDCONF_H_
#define CFG_STRUST4EMBEDDEDCONF_H_

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
#ifndef S4E_USE_WIFI_MODULE_THD
#define S4E_USE_WIFI_MODULE_THD 1
#endif
#ifndef S4E_USE_SSD1306_LCD
#define S4E_USE_SSD1306_LCD     1
#endif
#ifndef S4E_USE_POT
#define S4E_USE_POT             1
#endif
#ifndef S4E_USE_PWM
#define S4E_USE_PWM				1
#endif
#ifndef S4E_USE_RGB
#define S4E_USE_RGB				1
#endif
#ifndef S4E_USE_MQTT
#define S4E_USE_MQTT			1
#endif
#ifndef S4E_USE_WEB_SERVER
#define S4E_USE_WEB_SERVER		0
#endif
#ifndef S4E_USE_FAULT_HANDLER
#define S4E_USE_FAULT_HANDLER   1
#endif
#if S4E_USE_WEB_SERVER == 1 || S4E_USE_MQTT  == 1
#define S4E_USE_ETHERNET		1
#endif
#define SSID_KEY						"ssid"
#define SSID_VALUE						"ROR194"
#define APP_NAME_KEY					"appName"
#define APP_NAME_VALUE					"Struts4Embedded Demo!"
#define PWD_KEY							"pwd"
#define PWD_VALUE						"salah987"
#define RELOAD_HOST_KEY					"reloadHost"
#define RELOAD_HOST_VALUE				"raw.githubusercontent.com"
#define RELOAD_URL_KEY					"reloadURL"
#define RELOAD_URL_VALUE				"/abusous2000/Struts4Embedded/master/source/ESP8266WiFiModule/defaultPage.html"
#define FP_KEY							"fp"
#define FP_VALUE						"CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33"
#define NUM_OF_WORKER_THD_KEY			"numOfWorkerThds"
#define NUM_OF_WORKER_THD_VALUE 		"1"
#define DEFAULT_NUM_OF_WORKER_THDS 		1
#define MAX_THD_NAME_SIZE          		25
#define MAX_NUM_OF_WORKER_THDS			4

#ifndef PORTAB_SD
#define PORTAB_SD                      SD1
#endif

#define MQTT_CMD_SUBSCRIBE_TOPIC    	"dev/cmd/STM32F769i/mp3Player"
#define DEFAULT_MQTT_PUBLISH_TOPIC      "dev/update/STM32F769i/mp3Player"
#define MQTT_CLIENT_ID                  "Struts4EmbeddedWithStm32F769i"


#define SSD1306_I2C_AF       			4
#define SSD1306_I2C_SCL_PORT 			GPIOB
#define SSD1306_I2C_SCL_PIN  			GPIOB_ARD_D15
#define SSD1306_I2C_SDA_PORT 			GPIOB
#define SSD1306_I2C_SDA_PIN  			GPIOB_ARD_D14
#define SSD1306_SA0_PORT 				GPIOJ
#define SSD1306_SA0_PIN    				GPIOJ_ARD_D8
#define SSD1306_RESET_PORT				GPIOJ
#define SSD1306_RESET_PIN				GPIOJ_ARD_D7

#define TIM_TGRO_EVENT_EXTSEL 		    12 //See table 98 in RM on pages 451 & 471 for details
#define ADC_POT                     	ADCD2
#define ADC_CHANNEL_IN 					ADC_CHANNEL_IN6
#endif /* CFG_STRUST4EMBEDDED_H_ */
