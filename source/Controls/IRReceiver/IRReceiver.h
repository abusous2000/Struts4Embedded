/*
 * IRReceiver.h
 *
 *  Created on: Sep 19, 2020
 *      Author: abusous2000
 */
#if S4E_USE_IR_RECEIVER != 0
#ifndef SOURCE_IRRECEIVER_IRRECEIVER_H_
#define SOURCE_IRRECEIVER_IRRECEIVER_H_

/*===========================================================================*/
/* IR remote related code.                                                   */
/*===========================================================================*/
#define RAW_IR2
#define IR_FREQUENCY_USED        		10000

#define DELTA          5
#define START_PULSE    46
#define END_PULSE      402
#define RPT_CMD_PULSE  959
#define ZERO_PULSE     6
#define ONE_PULSE      18
#define COMMA_PULSE    23

/**
 * @brief  IR transmitter commands
 */
typedef enum {
  BTN_POWER= 0x00FFA25D,
  BTN_MODE = 0x00FF629D,
  BTN_MUTE = 0x00FFE21D,
  BTN_PLAY_PAUSE = 0x00FF22DD,
  BTN_PREV = 0x00FF02FD,
  BTN_NEXT = 0x00FFC23D,
  BTN_EQ = 0x00FFE01F,
  BTN_VOL_DOWN = 0x00FFA857,
  BTN_VOL_UP  = 0x00FF906F,
  BTN_0 = 0x00FF6897,
  BTN_LOOP = 0x00FF9867,
  BTN_USD = 0x00FFB04F,
  BTN_1 = 0x00FF30CF,
  BTN_2 = 0x00FF18E7,
  BTN_3 = 0x00FF7A85,
  BTN_4 = 0x00FF10EF,
  BTN_5 = 0x00FF38C7,
  BTN_6 = 0x00FF5AA5,
  BTN_7 = 0x00FF42BD,
  BTN_8 = 0x00FF4AB5,
  BTN_9 = 0x00FF52AD
}IR_CMD_t;

#ifdef __cplusplus
 extern "C" {
#endif
void initIRReeceiver(void);
void handlIREvent(IR_CMD_t command, bool repeatFlag);
#ifdef __cplusplus
 }
#endif

#endif /* SOURCE_IRRECEIVER_IRRECEIVER_H_ */


#endif
