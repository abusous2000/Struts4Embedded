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
MDIO	<=>		PA2 --black   -12L
MDC	<=>			PC1---white   -16R
nINT/RETCLK	<=>	PA1---Eggplant-13R
CRS	<=>			PA7---Gray    -10R
RX0	<=>			PC4---blue    -9L
RX1	<=>			PC5-- green   -9R
TX_EN	<=>		PB11--orange  -2L
TX0	<=>			PB12--yellow  -2R
TX1	<=>			PB13--Eggplant-1L
 */
//#define MAX_ACTION_EVENTS                6
#define BLINKER_THD_STACK_SIZE             1024//512
//#define BLINKER_THD_FREQ	    2500

#define S4E_USE_EBYTE_LORA 		0
#define EBYTE_LORA_SERVER       0
#define USERLIB_USE_RF          0

#ifndef USE_AE_SHELL
//set USE_AE_SHELL = "yes" in make file
#define USE_AE_SHELL 			1
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
#if S4E_USE_POT != 0
#define TIM_TGRO_EVENT_EXTSEL 	0x08 //See p. 398 in RM
#define ADC_POT_TIM             GPTD3 ///See p. 398 in RM
#define ADC_POT                 ADCD1
#define ADC_CHANNEL_IN 			ADC_CHANNEL_IN3//See Table 10 in user manual or datasheet
#define ADC_SMPR2_SMP           ADC_SMPR2_SMP_AN3(ADC_SAMPLE_144)

//#define LINE_ARD_D2           PAL_LINE(GPIOA, 10U)
#define POT_GPIO_PIN            PAL_LINE(GPIOA, 3U)
#endif

#ifndef S4E_USE_PWM
#define S4E_USE_PWM             0
#endif

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
//set USE_MAC = "yes" in make file
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
//Note that for this board, serial output uses PA9 & PA10 (w/ AF 7) which is in conflict with Ethernet with PA2
#define PORTAB_SD                       SD1
#define LINE_UART_SD_TX                 PAL_LINE(GPIOA, 9)//PA9
#define LINE_UART_SD_RX                 PAL_LINE(GPIOA, 10)//PA10
#define MQTT_CMD_SUBSCRIBE_TOPIC   		"dev/cmd/STM32F407BlackBoard/mp3Player"
#define DEFAULT_MQTT_PUBLISH_TOPIC 		"dev/update/STM32F407BlackBoard/mp3Player"
#define MQTT_CLIENT_ID                  "Struts4EmbeddedWithSTM32F407BlackBoard"

// #define LINE_LED_GREEN                  PAL_LINE(GPIOE, 7U)//PE
// #define LINE_LED_RED                    PAL_LINE(GPIOE, 13U)//PE
// #define LINE_LED_RED2                   PAL_LINE(GPIOE, 14U)//PE
// #define LINE_LED_BLUE                   PAL_LINE(GPIOE, 2U)//PE

#define LINE_LED_GREEN                  PAL_LINE(GPIOE, 13U)//PE13
#define LINE_LED_RED                    PAL_LINE(GPIOE, 14U)//PE14
#define LINE_LED_BLUE                   PAL_LINE(GPIOE, 15U)//PE15

#ifndef LINE_JOY_UP
#define USER_BUTTON                     PAL_LINE(GPIOE, 10U)//PE10
#define USER2_BUTTON                    PAL_LINE(GPIOE, 11U)//PE11
#define USER3_BUTTON                    PAL_LINE(GPIOE, 12U)//PE12
#endif

#ifndef S4E_USE_SDCARD
//set USE_FATFS = "yes" in make file
#define S4E_USE_SDCARD 			1
#endif
#if S4E_USE_SDCARD != 0
#include "board.h"
#define SDMMCD0_LINE 					LINE_SD_D0		//PC8
#define SDMMCD1_LINE 					LINE_SD_D1		//PC9
#define SDMMCD2_LINE 					LINE_SD_D2		//PC10
#define SDMMCD3_LINE 					LINE_SD_D3		//PC11
#define SDMMCCK_LINE 					LINE_SD_CLK		//PC12
#define SDMMCMD_LINE 					LINE_SD_CMD		//PD2
//#define SDMMMC_DETECT   				LINE_SD_DETECT	////PAL_LINE(GPIOI,15)
#define SDMMC_ALREADY_CONFIG            1
#endif//ENDS-S4E_USE_SDCARD

#ifndef USE_W25Q_XXXX
#define USE_W25Q_XXXX  					1
#endif
#if USE_W25Q_XXXX != 0
#define W25QXX_SPI_SCK_LINE   			PAL_LINE(GPIOB, 10)//PB10
#define W25QXX_SPI_MISO_LINE  			PAL_LINE(GPIOC, 2) //PC2
#define W25QXX_SPI_MOSI_LINE  			PAL_LINE(GPIOC, 3) //PC3
#define W25QXX_SPI_CS_LINE    			PAL_LINE(GPIOE, 3) //PE3
#define W25QXX_SPI_MODE       			PAL_MODE_ALTERNATE(5)    |    PAL_STM32_OSPEED_HIGHEST
#define W25QXX_SPI_CS_MODE    			PAL_MODE_OUTPUT_PUSHPULL |    PAL_STM32_OSPEED_HIGHEST
#define W25QXX_SPID						SPID2
#endif//Ends-USE_W25Q_XXXX


#ifndef S4E_USE_IR_RECEIVER
#define S4E_USE_IR_RECEIVER		        0
#endif
#if S4E_USE_IR_RECEIVER != 0
#define PORTABLE_ICU_LINE 		        PAL_LINE(GPIOC, 6U) //PC6-DCMI-D0
#define PORTABLE_ICUD 					ICUD8
#define PORTABLE_ICU_AF  				3
#define PORTABLE_ICU_CHANNEL 			ICU_CHANNEL_1
#endif

#ifndef S4E_USE_PPM_FRAME_DECODER
#define S4E_USE_PPM_FRAME_DECODER		1
#endif

//For some reason the blinking LED stops working once this is hooked up
//All else is working fine
#if S4E_USE_PPM_FRAME_DECODER != 0
#define PPM_DECODING_DEBUG				0
#define FREQUENCY_USED          		50000
#define RC_MIN_VALUE                    30
#define RC_MID_VALUE                    55
#define RC_MAX_VALUE                    80
#define MAX_FRAMES_TO_COLLECT     		10
#if 0
#define PORTABLE_PWM_LINE 				LINE_ARD_D5 //PC8
#define PORTABLE_PWMD 					PWMD8
#define PORTABLE_PWM_AF  				2
#define PORTABLE_PWM_CHANNEL			2
#define CH1_CB 							{PWM_OUTPUT_DISABLED, NULL}
#define CH2_CB 							{PWM_OUTPUT_DISABLED, NULL}
#define CH3_CB 							{PWM_OUTPUT_ACTIVE_HIGH, pwmc1cb}
#define CH4_CB 							{PWM_OUTPUT_DISABLED, NULL}
#endif

#define RC_ICU_LINE 				    PAL_LINE(GPIOE, 6)//PE6-FSMC_D7
#define RC_ICUD 					    ICUD9
#define RC_ICU_AF  				        3
#define RC_ICU_CHANNEL 			        ICU_CHANNEL_2
#endif//Ends:S4E_USE_PPM_FRAME_DECODER

#define GO_TO_SLEEP_MACROS      	    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;\
									    PWR->CR  |= (PWR_CR_PDDS | PWR_CR_LPDS | PWR_CR_CSBF  | PWR_CR_CWUF);\
									    PWR->CSR |= (PWR_CSR_WUF |  PWR_CSR_EWUP );\
									    RTC->ISR &= ~(RTC_ISR_ALRBF | RTC_ISR_ALRAF | RTC_ISR_WUTF | RTC_ISR_TAMP1F |\
													RTC_ISR_TSOVF | RTC_ISR_TSF);

#define RTC_ALARM_1_FLAGS2   			RTC_ALRM_MSK4  |\
										RTC_ALRM_MSK3  |\
										RTC_ALRM_MSK2  |\
										RTC_ALRM_ST(0) |\
										RTC_ALRM_SU(0)

#define RTC_ALARM_2_FLAGS2  			RTC_ALRM_MSK4  |\
										RTC_ALRM_MSK3  |\
										RTC_ALRM_MSK2  |\
										RTC_ALRM_ST(5) |\
										RTC_ALRM_SU(1)
#define WAKEUP_HARD_REST_CHECK          (uint32_t)(PWR->CSR & PWR_CSR_SBF)
#define CLEAR_WAKEUP_FLAG			    PWR->CR  	|= (PWR_CR_CSBF)



#define BACKUP_CCM_RAM_SECTION 			ram5


#endif /* CFG_STRUST4EMBEDDED_H_ */
