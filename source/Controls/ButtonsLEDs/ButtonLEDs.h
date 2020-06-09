#ifndef SOURCE_BUTTONSLEDS_BUTTONLEDS_H_
#define SOURCE_BUTTONSLEDS_BUTTONLEDS_H_
#include "CPal.h"
#define BUZZER_LINE                 LINE_ARD_D11 //D11
#define RGB_LINE                    LINE_ARD_D12 //D12

#ifndef S4E_USE_BUZZER
#define S4E_USE_BUZZER				0
#endif
#ifndef LINE_LED_GREEN
#define LINE_LED_GREEN              LINE_ARD_D13
#endif

#ifndef LINE_JOY_UP
#define LINE_JOY_UP                 LINE_ARD_A1//PAL_LINE(GPIOA, 4U)  //A1
#define LINE_JOY_RIGHT              LINE_ARD_A2//PAL_LINE(GPIOC, 2U)  //A2
#define LINE_JOY_LEFT               LINE_ARD_A3//GPIOF, 10U) //A3
#define LINE_JOY_CENTER             LINE_ARD_A4//PAL_LINE(GPIOF, 8U)  //A4
#define LINE_JOY_DOWN               LINE_ARD_A5//PAL_LINE(GPIOF, 9U)  //A5
#endif
#define BUTTON_MODE1                PAL_STM32_MODE_INPUT  | PAL_STM32_OTYPE_PUSHPULL| PAL_STM32_OSPEED_HIGHEST| PAL_STM32_PUPDR_FLOATING
#define BUTTON_MODE2                PAL_STM32_MODE_INPUT  | PAL_STM32_OTYPE_PUSHPULL| PAL_STM32_OSPEED_HIGHEST| PAL_STM32_PUPDR_PULLUP
#define LED_MODE                    PAL_STM32_MODE_OUTPUT | PAL_STM32_OTYPE_PUSHPULL| PAL_STM32_OSPEED_MID1|    PAL_STM32_PUPDR_FLOATING

#ifdef __cplusplus
 extern "C" {
#endif
extern CPalTypedef 		  			*pGreenLedPAL, *pRedLedPAL,*pCenterKey, *pUpKey,
                                    *pDownKey,     *pLeftKey,*pRightKey,*pBuzzer, *pRgb, *pEasyLinkKey;
void initButtonsLEDs(void);
#ifdef __cplusplus
}
#endif
#endif /* SOURCE_BUTTONSLEDS_BUTTONLEDS_H_ */
