/*
 * Strust4Embedded.h
 *
 *  Created on: Dec 12, 2019
 *      Author: abusous2000
 */

#ifndef CFG_STRUST4EMBEDDEDCONF_H_
#define CFG_STRUST4EMBEDDEDCONF_H_

#define S4E_USE_PPM_FRAME_DECODER	0
#define S4E_USE_IR_RECEIVER     0

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

#ifndef S4E_USE_WIFI_MODULE_THD
#define S4E_USE_WIFI_MODULE_THD 0
#endif

#ifndef S4E_USE_SSD1306_LCD
#define S4E_USE_SSD1306_LCD     0
#define SSD1306_I2C_SCL_LINE    PAL_LINE(GPIOB, GPIOB_ARD_D15)
#define SSD1306_I2C_SDA_LINE   	PAL_LINE(GPIOB, GPIOB_ARD_D14)
#define SSD1306_SA0_LINE        PAL_LINE(GPIOA, GPIOA_ARD_D8)
#define SSD1306_RESET_LINE      PAL_LINE(GPIOA, GPIOA_ARD_D7)
#define SSD1306_I2C_AF       	4
#define SSD1306_I2C_LINE_MODE	PAL_MODE_ALTERNATE(SSD1306_I2C_AF)  | PAL_STM32_OSPEED_HIGHEST|  PAL_STM32_OTYPE_OPENDRAIN
#define SSD1306_LINE_MODE   	PAL_STM32_MODE_OUTPUT  | PAL_STM32_PUPDR_PULLUP

#endif

#ifndef S4E_USE_POT
#define S4E_USE_POT             1
#endif
#ifndef S4E_USE_PWM
#define PWM_LINE1 		        LINE_ARD_D0
#define PWM_LINE2 				LINE_ARD_D1
#define S4E_USE_PWM             0
#endif

#ifndef S4E_USE_BUZZER
#define S4E_USE_BUZZER			0
#endif

#ifndef S4E_USE_RGB

#define LINE_IO_DATA			LINE_ARD_D5
#define LINE_IO_CLOCK			LINE_ARD_D6
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

#define PORTAB_SD                       SD3
#define MQTT_CMD_SUBSCRIBE_TOPIC   		"dev/cmd/STM32F429ZI/mp3Player"
#define DEFAULT_MQTT_PUBLISH_TOPIC 		"dev/update/STM32F429ZI/mp3Player"
#define MQTT_CLIENT_ID                  "Struts4EmbeddedWithStm32F429ZI"

#define LINE_LED_GREEN                  LINE_LED1
#define LINE_LED_BLUE                   LINE_LED2
#define LINE_LED_RED                    LINE_LED3

#define TIM_TGRO_EVENT_EXTSEL 		    0x08 //See p. 386 in RM
#define ADC_POT_TIM                     GPTD3 ///See p. 386 in RM
#define ADC_POT                     	ADCD1
#define ADC_CHANNEL_IN 					ADC_CHANNEL_IN3//See Table 10 in user manual or datasheet

#define MPU_SCL_PIN						PAL_LINE(GPIOB, 10U)//LINE_ARD_D15
#define MPU_SDA_PIN						PAL_LINE(GPIOB, 11U)//LINE_ARD_D14
#define MPU_I2C_AF						4
#define MPU_I2CD						I2CD2
//#define LINE_ARD_D2                 PAL_LINE(GPIOA, 10U)
#define MPU_INT_PORT					GPIOA
#define MPU_INT_PIN						10

#define PPM_DECODING_DEBUG				0
#define MAX_FRAMES_TO_COLLECT     		10
#define PORTABLE_ICU_LINE 				PAL_LINE(GPIOA, 0U) //PA0-Right pin Rail..3rd left from bottom
#define PORTABLE_ICUD 					ICUD5
#define PORTABLE_ICU_AF  				2
#define PORTABLE_ICU_CHANNEL 			ICU_CHANNEL_1

//This is the receiver/client
#define S4E_USE_EBYTE_LORA              1
#define EBYTE_LORA_SERVER               0
#define PORTAB_EBYTE_LORA_SD 			SD6

#define EBYTE_LORA_TX 	    			LINE_ARD_D1 //PG14
#define EBYTE_LORA_RX 	    			LINE_ARD_D0 //PG9
#define EBYTE_LORA_M0 	    			LINE_ARD_D4
#define EBYTE_LORA_M1 	    			LINE_ARD_D5
#define EBYTE_LORA_AUX 	    			LINE_ARD_D6
#define EBYTE_LORA_AF       			8


#define DEFAULT_OPTION_FIXED_TRANS		1
#if DEFAULT_OPTION_FIXED_TRANS != 0
#define DEFAULT_ADDRESS_HIGH			2
#define DEFAULT_ADDRESS_LOW				0
#define DEFAULT_CHANNEL					6
#endif

#define DEFAULT_TO_ADDRESS_HIGH			1
#define DEFAULT_TO_ADDRESS_LOW			0
#define DEFAULT_TO_CHANNE				5

#define EBYTE_LORA_HOST_ID              2
#define EBYTE_LORA_SAVE_PARAMS		    1
#define EBYTE_LORA_100MW

#define GO_TO_SLEEP_MACROS      	   SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;\
									   PWR->CR  |= (PWR_CR_PDDS | PWR_CR_LPDS | PWR_CR_CSBF  | PWR_CR_CWUF);\
									   PWR->CSR |= (PWR_CSR_WUF |  PWR_CSR_EWUP );\
									   RTC->ISR &= ~(RTC_ISR_ALRBF | RTC_ISR_ALRAF | RTC_ISR_WUTF | RTC_ISR_TAMP1F |\
													RTC_ISR_TSOVF | RTC_ISR_TSF);

#define RTC_ALARM_1_FLAGS2   			RTC_ALRM_MSK4  |\
										RTC_ALRM_MSK3  |\
										RTC_ALRM_MSK2  |\
										RTC_ALRM_ST(0) |\
										RTC_ALRM_SU(0)

#define RTC_ALARM_2_FLAGS2  				RTC_ALRM_MSK4  |\
										RTC_ALRM_MSK3  |\
										RTC_ALRM_MSK2  |\
										RTC_ALRM_ST(5) |\
										RTC_ALRM_SU(1)
#define WAKEUP_HARD_REST_CHECK          (uint32_t)(PWR->CSR & PWR_CSR_SBF)
#define CLEAR_WAKEUP_FLAG			    PWR->CR  	|= (PWR_CR_CSBF)



#define BACKUP_CCM_RAM_SECTION 			ram5

#endif /* CFG_STRUST4EMBEDDED_H_ */
