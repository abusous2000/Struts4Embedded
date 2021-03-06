#include "Strust4EmbeddedConf.h"
#include "ch.h"
#include "hal.h"
#include "Strust4Embedded.h"
#include "ButtonLEDs.h"



#define DEBOUNCING_PULSE_WIDTH_MIN 10
static systime_t lastPulseTime;
static void *lastButton;
static void joystickCBHandler(void *arg) {
	systime_t currentPulseTime = chVTGetSystemTimeX();
	int       delta            = currentPulseTime - lastPulseTime;
    bool      ignore           = (lastButton ==  arg && TIME_I2MS(delta) <= DEBOUNCING_PULSE_WIDTH_MIN);//handles debouncing logic,aka.. happy finger

    if ( !ignore)
	   triggerActionEventFromISR((char*)arg,NULL,0,NULL);
    lastButton    = arg;
    lastPulseTime = currentPulseTime;
}
//#define EASYLINK_BUTTON		LINE_ARD_D9 //pin 13
#define EASYLINK_BUTTON0	LINE_ARD_D0 //pin 2
#define EASYLINK_BUTTON1	LINE_ARD_D1 //pin 2
CPalTypedef 		  			*pGreenLedPAL = NULL, *pCenterKey= NULL, *pUpKey= NULL,
                                *pDownKey= NULL, *pLeftKey= NULL,*pRightKey= NULL,*pBuzzer= NULL, *pRgb= NULL, *pEasyLinkKey= NULL,*pEasyLinkKey0= NULL,*pEasyLinkKey1= NULL, *pYellow= NULL;

#if S4E_USE_BUZZER != 0
static CPalTypedef buzzer={     .line=BUZZER_LINE,          .mode=LED_MODE};
#endif
#if S4E_USE_RGB != 0
static CPalTypedef rgb={        .line=RGB_LINE,             .mode=LED_MODE};
#endif
//#endif
static CPalTypedef greenLedPAL={.line=LINE_LED_GREEN,     	.mode=LED_MODE};
#if defined(LINE_LED_RED)
CPalTypedef 		  			*pRedLedPAL;
static CPalTypedef redLedPAL={  .line=LINE_LED_RED,       	.mode=LED_MODE};
#endif
#if S4E_USE_JOYSTICK != 0
static CPalTypedef centerKey={  .line=LINE_JOY_CENTER,      .mode=BUTTON_MODE2, .risingfallingEdge=PAL_EVENT_MODE_RISING_EDGE, .cb=joystickCBHandler, .arg=(void*)TOGGLE_MUTE_AE_NAME};
static CPalTypedef upKey={      .line=LINE_JOY_UP,          .mode=BUTTON_MODE2, .risingfallingEdge=PAL_EVENT_MODE_RISING_EDGE, .cb=joystickCBHandler, .arg=(void*)NEXT_TRACK_AE_NAME};
static CPalTypedef downKey={    .line=LINE_JOY_DOWN,        .mode=BUTTON_MODE2, .risingfallingEdge=PAL_EVENT_MODE_RISING_EDGE, .cb=joystickCBHandler, .arg=(void*)TOGGLE_PAUSE_AE_NAME};
static CPalTypedef leftKey={    .line=LINE_JOY_LEFT,        .mode=BUTTON_MODE2, .risingfallingEdge=PAL_EVENT_MODE_RISING_EDGE, .cb=joystickCBHandler, .arg=(void*)VOLUME_DOWN_AE_NAME};
static CPalTypedef rightKey={   .line=LINE_JOY_RIGHT,       .mode=BUTTON_MODE2, .risingfallingEdge=PAL_EVENT_MODE_RISING_EDGE, .cb=joystickCBHandler, .arg=(void*)VOLUME_UP_AE_NAME};

//For this to work, plz connect Pin20 to Pin15 in the Zigbee header on the waveshare accessory shield
#ifdef EASYLINK_BUTTON
static CPalTypedef easyLinkKey={.line=EASYLINK_BUTTON,  	.mode=BUTTON_MODE2, .risingfallingEdge=PAL_EVENT_MODE_RISING_EDGE,.cb=joystickCBHandler, .arg=(void*)TOGGLE_PAUSE_AE_NAME};
#endif
//static CPalTypedef easyLinkKey0={.line=EASYLINK_BUTTON0,  	.mode=BUTTON_MODE2, .risingfallingEdge=PAL_EVENT_MODE_FALLING_EDGE,.cb=joystickCBHandler, .arg=(void*)TOGGLE_PAUSE_AE_NAME};//"EasyLink"};Pin3
//static CPalTypedef easyLinkKey1={.line=EASYLINK_BUTTON1,  	.mode=BUTTON_MODE2, .risingfallingEdge=PAL_EVENT_MODE_FALLING_EDGE,.cb=joystickCBHandler, .arg=(void*)VOLUME_DOWN_AE_NAME};//"EasyLink"};Pin2
#endif
#ifdef USER_BUTTON

static CPalTypedef centerKey={  .line=USER_BUTTON,      .mode=BUTTON_MODE1, .risingfallingEdge=PAL_EVENT_MODE_RISING_EDGE, .cb=joystickCBHandler, .arg=(void*)USER_BUTTON_EVENT};
   #if defined(USER2_BUTTON)
  static CPalTypedef upKey={      .line=USER2_BUTTON,          .mode=BUTTON_MODE2, .risingfallingEdge=PAL_EVENT_MODE_RISING_EDGE, .cb=joystickCBHandler, .arg=(void*)NEXT_TRACK_AE_NAME};
  #endif
  #if defined(USER3_BUTTON)
  static CPalTypedef downKey={    .line=USER3_BUTTON,        .mode=BUTTON_MODE2, .risingfallingEdge=PAL_EVENT_MODE_RISING_EDGE, .cb=joystickCBHandler, .arg=(void*)GO_TO_SLEEP_AE_NAME};
  #endif
#endif

void initButtonsLEDs(void) {
#if S4E_USE_BUZZER != 0
  pBuzzer      = initCPalInstance(&buzzer);pBuzzer->init(&buzzer);pBuzzer->clear(&buzzer);
#endif
#if S4E_USE_RGB != 0
  pRgb         = initCPalInstance(&rgb);pRgb->init(&rgb);pRgb->clear(&rgb);
#endif
  pGreenLedPAL = initCPalInstance(&greenLedPAL);pGreenLedPAL->init(&greenLedPAL);pGreenLedPAL->toggle(&greenLedPAL);
#if defined(LINE_LED_RED)
  pRedLedPAL 	   = initCPalInstance(&redLedPAL);pRedLedPAL->init(&redLedPAL);pRedLedPAL->toggle(&redLedPAL);
#endif

#if S4E_USE_JOYSTICK != 0
  pCenterKey   = initCPalInstance(&centerKey);pCenterKey->init(&centerKey);
  #ifndef STM32F446xx
  //These two lines, has interrupt conflict with STM32F446xx
  pUpKey       = initCPalInstance(&upKey);pUpKey->init(&upKey);
  pLeftKey     = initCPalInstance(&leftKey);pLeftKey->init(&leftKey);
  #endif
  pDownKey     = initCPalInstance(&downKey);pDownKey->init(&downKey);
  #if !defined(STM32F429xx) && !defined(BOARD_ST_NUCLEO144_F746ZG)
  pRightKey    = initCPalInstance(&rightKey);pRightKey->init(&rightKey);
  #endif
#ifdef EASYLINK_BUTTON
  pEasyLinkKey = initCPalInstance(&easyLinkKey);pEasyLinkKey->init(&easyLinkKey);pEasyLinkKey->clear(&easyLinkKey);
#endif
//  pEasyLinkKey0= initCPalInstance(&easyLinkKey0);pEasyLinkKey0->init(&easyLinkKey0);
//  pEasyLinkKey1= initCPalInstance(&easyLinkKey1);pEasyLinkKey1->init(&easyLinkKey1);
#elif defined(USER_BUTTON)
   pCenterKey   = initCPalInstance(&centerKey);pCenterKey->init(&centerKey);
   #if defined(USER2_BUTTON)
   pUpKey       = initCPalInstance(&upKey);pUpKey->init(&upKey);
   #endif

   #if defined(USER3_BUTTON)
   pDownKey     = initCPalInstance(&downKey);pDownKey->init(&downKey);
   #endif
#endif


  return;
}


