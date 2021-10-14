/*
 * Strust4Embedded.h
 *
 *  Created on: Dec 12, 2019
 *      Author: abusous2000
 */

#ifndef CFG_STRUST4EMBEDDEDCONF_H_
#define CFG_STRUST4EMBEDDEDCONF_H_
/*
 *DP83848/STM32F4
 *ETH_MDIO -------------------------> PA2
 *ETH_MDC --------------------------> PC1
 *ETH_RMII_REF_CLK------------------> PA1
 *ETH_RMII_CRS_DV/RMII_RX_DV--------> PA7
 *ETH_RMII_RXD0 --------------------> PC4
 *ETH_RMII_RXD1 --------------------> PC5
 *ETH_RMII_TX_EN -------------------> PG11
 *ETH_RMII_TXD0 --------------------> PG13
 *ETH_RMII_TXD1 --------------------> PG14
*/
//#define MAX_ACTION_EVENTS                6
#define BLINKER_THD_STACK_SIZE             1024//512
#define S4E_USE_PPM_FRAME_DECODER 		0
#define S4E_USE_EBYTE_LORA 		0
#define EBYTE_LORA_SERVER       0

#ifndef USE_LCD_TFT
#define USE_LCD_TFT 			0
#endif
#ifndef S4E_USE_JOYSTICK
#define S4E_USE_JOYSTICK        1
#define LINE_JOY_UP             PAL_LINE(GPIOF, 9)//PF9-KEY0
#define LINE_JOY_DOWN           PAL_LINE(GPIOF, 7)//PF7-KEY2
#define LINE_JOY_LEFT           PAL_LINE(GPIOF, 8)//PF8-KEY1
#define LINE_JOY_RIGHT          PAL_LINE(GPIOF, 6)//PF9-KEY3

//#define LINE_JOY_CENTER         PAL_LINE(GPIOF, 8)//PF8-KEY5

#endif
#ifndef S4E_USE_BLINKER_THD
#define S4E_USE_BLINKER_THD     1
#endif
#ifndef S4E_USE_WIFI_MODULE_THD
//1-Turn off MAC in halcof.h & makefile if WIFI is on; both have pin conflict (PA2)
//2-Flash 1st without adding plugging in the WiFi module; else debugger won't work
//3-WiFi & SDCard use more power than USB can provide; therefore power directly from power supply
//4-Ground your TTL with MCU else serial output becomes garbage
//5- GPIO0 ---> configure as output HIGH
//  -GPIO2 & CH_PD ---> input & keep floating HIZ
//6-Remove all jumpers from P5 pin group
//7-If you use local mobile hotspot as ur router, plz run it with 2.4GHz ONLY on channel 6
#define S4E_USE_WIFI_MODULE_THD 0
#define WIFI_SD                 SD4
#define LINE_WIFI_AF            8U
#define LINE_WIFI_MODE          PAL_MODE_ALTERNATE(LINE_WIFI_AF)| PAL_STM32_OSPEED_HIGHEST | PAL_STM32_OTYPE_PUSHPULL
#define LINE_WIFI_RST           PAL_LINE(GPIOF, 14U)//PF14--EXT_RST
#define LINE_WIFI_TX            PAL_LINE(GPIOA, 0U)//PA0
#define LINE_WIFI_RX            PAL_LINE(GPIOA, 1U)//PA1
#define LINE_WIFI_CH_PD         PAL_LINE(GPIOC, 1U)//PC1-high-RMII_MDC-EXT-SDA-G6
#define LINE_WIFI_GPIO0         PAL_LINE(GPIOA, 7U)//PA7-low- RMII RX_DV
#define LINE_WIFI_GPIO2         PAL_LINE(GPIOA, 2U)//PA2-low-GPIO2 EXT-SCL-G3

#endif
#ifndef S4E_USE_SSD1306_LCD
#define S4E_USE_SSD1306_LCD     0
#endif
#ifndef S4E_USE_POT
#define S4E_USE_POT             1
#define TIM_TGRO_EVENT_EXTSEL 	0x08 //See p. 398 in RM
#define ADC_POT_TIM             GPTD3 ///See p. 398 in RM
#define ADC_POT                 ADCD1
#define ADC_CHANNEL_IN 			ADC_CHANNEL_IN10//ADC_CHANNEL_IN3//See Table 10 in user manual or datasheet
#define ADC_SMPR2_SMP           ADC_SMPR1_SMP_AN10(ADC_SAMPLE_144)//ADC_SMPR2_SMP_AN3(ADC_SAMPLE_144)

//#define LINE_ARD_D2           PAL_LINE(GPIOA, 10U)
#define POT_GPIO_PIN            PAL_LINE(GPIOC, 0U)//PC0-PAL_LINE(GPIOA, 3U)//

#endif

#define S4E_USE_IR_RECEIVER		1

#define PORTABLE_ICU_LINE 		PAL_LINE(GPIOC, 6U) //PC6
#define PORTABLE_ICUD 			ICUD8
#define PORTABLE_ICU_AF  		3
#define PORTABLE_ICU_CHANNEL 	ICU_CHANNEL_1


#ifndef S4E_USE_PWM
#define S4E_USE_PWM             0
#endif
#ifndef S4E_USE_BUZZER
#define S4E_USE_BUZZER			1
#define BUZZER_LINE             PAL_LINE(GPIOG, 7)//PG7
#endif
#ifndef S4E_USE_RGB
#define S4E_USE_RGB             0
#endif
#ifndef S4E_USE_MQTT
//Remember this requires change MAC option hal.h, configuring pins in board.h, & enabling LWIP in makefile
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
//Note that for this board, serial output uses PA9 & PA10 (w/ AF 7) which is in conflict with Ethernet with PA2
#define PORTAB_SD                       SD1
#define LINE_UART_SD_TX                 PAL_LINE(GPIOA, 9)//PA9
#define LINE_UART_SD_RX                 PAL_LINE(GPIOA, 10)//PA10
#define MQTT_CMD_SUBSCRIBE_TOPIC   		"dev/cmd/STM32F407BlackBoard/mp3Player"
#define DEFAULT_MQTT_PUBLISH_TOPIC 		"dev/update/STM32F407BlackBoard/mp3Player"
#define MQTT_CLIENT_ID                  "Struts4EmbeddedWithSTM32F407BlackBoard"

//brown, black, red, & orange
#define LINE_LED_GREEN                  PAL_LINE(GPIOE, 3U)//PE3
// #define LINE_LED_RED                    PAL_LINE(GPIOE, 4U)//PE4



#ifndef S4E_USE_SDCARD
#define S4E_USE_SDCARD 			        1
#endif
#include "board.h"
#define SDMMCD0_LINE 					PAL_LINE(GPIOC, 8U) //PC8
#define SDMMCD1_LINE 					PAL_LINE(GPIOC, 9U) //PC9
#define SDMMCD2_LINE 					PAL_LINE(GPIOC, 10U)//PC10
#define SDMMCD3_LINE 					PAL_LINE(GPIOC, 11U)//PC11
#define SDMMCCK_LINE 					PAL_LINE(GPIOC, 12U)//PC12
#define SDMMCMD_LINE 					PAL_LINE(GPIOD, 2U) //PD2
//#define SDMMMC_DETECT   				LINE_SD_DETECT	////PAL_LINE(GPIOI,15)


#define SDMMC_ALREADY_CONFIG            1


#define USE_W25Q_XXXX  					1
//If you use the recommended SPI1 it will run in conflict with SD Card.
//That is why I am using SPID3 instead of SPID1. See mcuconf.h for more details
#define W25QXX_SPID						SPID3

#define W25QXX_SPI_SCK_LINE   			PAL_LINE(GPIOB, 3)//PB3
#define W25QXX_SPI_MISO_LINE  			PAL_LINE(GPIOB, 4)//PB4
#define W25QXX_SPI_MOSI_LINE  			PAL_LINE(GPIOB, 5)//PB5
#define W25QXX_SPI_CS_LINE    			PAL_LINE(GPIOG, 8)//PG8
#define W25QXX_SPI_MODE       			PAL_MODE_ALTERNATE(6)    |    PAL_STM32_OSPEED_HIGHEST
#define W25QXX_SPI_CS_MODE    			PAL_MODE_OUTPUT_PUSHPULL |    PAL_STM32_OSPEED_HIGHEST


#define USERLIB_USE_RF                  0
#define TRANSMITTER                     1
#define NRF24L01_THD_STACK_SIZE         1024//512
#define NRF24L01_LINE_CE                PAL_LINE(GPIOF, 2)//PF2
#define NRF24L01_SPI_CNS                PAL_LINE(GPIOF, 3)//PF3
#define NRF24L01_SPI_IRQ_MODE    		PAL_MODE_INPUT           |    PAL_STM32_OSPEED_HIGHEST
#define NRF24L01_SPI_CS_MODE    		PAL_MODE_OUTPUT_PUSHPULL |    PAL_STM32_OSPEED_HIGHEST

#define NRF24L01_SPI_SCK                PAL_LINE(GPIOB, 3)//PB3
#define NRF24L01_SPI_MOSI               PAL_LINE(GPIOB, 5)//PB5-LINE_ARD_D11
#define NRF24L01_SPI_MISO               PAL_LINE(GPIOB, 4)//PB4-LINE_ARD_D12
#define NRF24L01_LINE_IRQ               PAL_LINE(GPIOF, 4)//PF4


#define NRF24L01_SPID				    SPID3
#define NRF24L01_SPI_MODE       		PAL_MODE_ALTERNATE(6)    |    PAL_STM32_OSPEED_HIGHEST
#define RF_PAYLEN                       NRF24L01_MAX_PAYLEN
#define RF_USE_MUTUAL_EXCLUSION     TRUE

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

#define RTC_ALARM_2_FLAGS2  			RTC_ALRM_MSK4  |\
										RTC_ALRM_MSK3  |\
										RTC_ALRM_MSK2  |\
										RTC_ALRM_ST(5) |\
										RTC_ALRM_SU(1)
#define WAKEUP_HARD_REST_CHECK          (uint32_t)(PWR->CSR & PWR_CSR_SBF)
#define CLEAR_WAKEUP_FLAG			    PWR->CR  	|= (PWR_CR_CSBF)



#define BACKUP_CCM_RAM_SECTION 			ram5


#endif /* CFG_STRUST4EMBEDDED_H_ */
