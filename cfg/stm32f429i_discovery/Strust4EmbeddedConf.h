/*
 * Strust4Embedded.h
 *
 *  Created on: Dec 12, 2019
 *      Author: abusous2000
 */

#ifndef CFG_STRUST4EMBEDDEDCONF_H_
#define CFG_STRUST4EMBEDDEDCONF_H_

#define S4E_USE_PPM_FRAME_DECODER 		0
#define S4E_USE_EBYTE_LORA 		0
#define EBYTE_LORA_SERVER       0
#define S4E_USE_IR_RECEIVER		0

#ifndef S4E_USE_SDCARD
#define S4E_USE_SDCARD 			0
#endif
//#define MAX_ACTION_EVENTS                6
#define BLINKER_THD_STACK_SIZE             1024//512
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
#define PWM_LINE1 		        LINE_ARD_D0
#define PWM_LINE2 				LINE_ARD_D1
#define PWM_LINE1_CH    		0
#define PWM_LINE2_CH    		1
#define PWM_LINE_AF   			3
#define PWM_DRIVER 				PWMD8
#endif


#ifndef S4E_USE_BUZZER
#define S4E_USE_BUZZER			0
#endif

#ifndef S4E_USE_RGB
#define S4E_USE_RGB             0
#endif
/*
 * Without removing/de-soldering the gyro on rhe STM32F429i-disc board,
 * it will be hard to make ethernet work, see this for details. There are pin conflicts
 * https://www.emcraft.com/stm32f429discovery/connecting-to-ethernet-on-stm32f429
 */
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

#define PORTAB_SD                       SD1
#define MQTT_CMD_SUBSCRIBE_TOPIC   		"dev/cmd/STM32F429ZI/mp3Player"
#define DEFAULT_MQTT_PUBLISH_TOPIC 		"dev/update/STM32F429ZI/mp3Player"
#define MQTT_CLIENT_ID                  "Struts4EmbeddedWithStm32F429ZI"

#define SSD1306_I2C_AF       			4
#define SSD1306_I2C_SCL_PORT 			GPIOB
#define SSD1306_I2C_SCL_PIN  			GPIOB_ARD_D15
#define SSD1306_I2C_SDA_PORT 			GPIOB
#define SSD1306_I2C_SDA_PIN  			GPIOB_ARD_D14
#define SSD1306_SA0_PORT 				GPIOF
#define SSD1306_SA0_PIN    				GPIOF_ARD_D8
#define SSD1306_RESET_PORT				GPIOF
#define SSD1306_RESET_PIN				GPIOF_ARD_D7

#define LINE_LED_GREEN                  PAL_LINE(GPIOG, GPIOG_LED3_GREEN)
#define LINE_LED_RED                    PAL_LINE(GPIOG, GPIOG_LED4_RED)

#define TIM_TGRO_EVENT_EXTSEL 		    0x08 //See p. 386 in RM
#define ADC_POT_TIM                     GPTD3 ///See p. 386 in RM
#define ADC_POT                     	ADCD1
#define ADC_CHANNEL_IN 					ADC_CHANNEL_IN3//See Table 10 in user manual or datasheet
#define ADC_SMPR2_SMP                   ADC_SMPR2_SMP_AN3(ADC_SAMPLE_144)


#define MPU_SCL_PIN						PAL_LINE(GPIOB, 10U)//LINE_ARD_D15
#define MPU_SDA_PIN						PAL_LINE(GPIOB, 11U)//LINE_ARD_D14
#define MPU_I2C_AF						4
#define MPU_I2CD						I2CD2
//#define LINE_ARD_D2                 PAL_LINE(GPIOA, 10U)
#define POT_GPIO_PIN                    PAL_LINE(GPIOA, 3U)
#define MPU_INT_PORT					GPIOA
#define MPU_INT_PIN						10
#ifndef LINE_JOY_UP
#define USER_BUTTON                     PAL_LINE(GPIOA, 0U)
#endif
#endif /* CFG_STRUST4EMBEDDED_H_ */
