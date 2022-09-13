/*
 * PPMFrameDecoder.h
 *
 *  Created on: Aug 27, 2020
 *      Author: abusous2000
 */
#if S4E_USE_PPM_FRAME_DECODER != 0
#include "Strust4EmbeddedConf.h"
#ifndef SOURCE_CONTROLS_PPMFRAMEDECODER_H_
#define SOURCE_CONTROLS_PPMFRAMEDECODER_H_
#ifndef PPM_THD_STACK_SIZE
#define PPM_THD_STACK_SIZE             	512
#endif
#ifndef PPM_FREQUENCY_USED
#define PPM_FREQUENCY_USED          	50000
#endif
#ifndef PERIOD_USED
#define PERIOD_USED          		    50000
#endif
#ifndef RC_ERR_MARGIN
#define RC_ERR_MARGIN                   2
#endif

#if S4E_USE_IBUS != 0
#define RC_BUTTON_ADJ                  25
#else
#define RC_BUTTON_ADJ                  0
#endif

#ifndef RC_BUTTON_MIN_VALUE
#define RC_BUTTON_MIN_VALUE            (25+RC_BUTTON_ADJ)
#endif
#ifndef RC_BUTTON_MAX_VALUE
#define RC_BUTTON_MAX_VALUE            (75+RC_BUTTON_ADJ)
#endif
#ifndef RC_BUTTON_MID_VALUE
#define RC_BUTTON_MID_VALUE            (50+RC_BUTTON_ADJ)
#endif

#define RC_CH1							0
#define RC_CH2							1
#define RC_CH3							2
#define RC_CH4							3

#ifndef RC_SWA
#define RC_SWA 							4
#endif

#ifndef RC_SWB
#define RC_SWB 							5
#endif

#ifndef RC_SWC
#define RC_SWC 							6
#endif

#ifndef RC_SWD
#define RC_SWD 							7
#endif

#ifndef RC_SWE
#define RC_SWE 							8
#endif

#ifndef RC_SWF
#define RC_SWF 							9
#endif

#define TimeFromCycles2US(x)        	((x * 1000000)/PPM_FREQUENCY_USED)
#ifndef MAX_CHANNELS
#define MAX_CHANNELS   					8
#endif
#ifndef MAX_FRAMES_TO_COLLECT
#define MAX_FRAMES_TO_COLLECT     		10
#endif
#ifndef PPM_MIN_GAP_BTWN_FRAMES
#define PPM_MIN_GAP_BTWN_FRAMES     	230
#endif

#ifndef PPM_CHANNEL_WIDTH
#define PPM_CHANNEL_WIDTH        		100
#endif
#define PPM_WITHIN_MARGIN(x,y)          (x >= (y-RC_ERR_MARGIN) && x <= (y+RC_ERR_MARGIN))
#define IS_VALID_PPM_VALUE(x)           (x >= (RC_BUTTON_MIN_VALUE-RC_ERR_MARGIN) && x <= (81+RC_ERR_MARGIN))

#define IBUS_LENGTH                     0x20
#define IBUS_COMMAND40                  0x40
#define IBUS_MAX_CHANNLES               14
#define IBUS_BUFF_LEN                   32
#define IBUS_SLEEP_BTWN_MSGS            150

#define IS_IBUS_CH_COMMAND40(Y)         (Y[0] == IBUS_LENGTH && Y[1] == IBUS_COMMAND40)
enum ButtonStats {
  BUTTON_STATE_LOW     = 0,
  BUTTON_STATE_MID     = 1,
  BUTTON_STATE_HIGH    = 2,
  BUTTON_STATE_UNKNOWN = 3
};
typedef enum ButtonStats ButtonStats_Typedef;

enum RC_SIGNAL_SRC {
  IBUS_SIGNAL_SRC    = 0,
  PPM_SIGNAL_SRC     = 1
};
typedef enum RC_SIGNAL_SRC RC_SIGNAL_SRC_Typedef;

typedef struct {
	uint8_t   		valueInCycles[MAX_CHANNELS];
	uint32_t   		rawValue[MAX_CHANNELS];
	uint8_t  		period[MAX_CHANNELS];
	uint32_t  		gap;
	bool      		processed;
	uint8_t  		channelsCollected;
	uint32_t  		frameID;
	systime_t   	start;
	systime_t   	end;
	sysinterval_t	frameDuration;
	uint32_t 	    frameDurationMicroSec;
	RC_SIGNAL_SRC_Typedef   source;
}PPM_FRAME_TYPDEF;

#ifdef __cplusplus
 extern "C" {
#endif
void printPPMValueChange(uint8_t ch, PPM_FRAME_TYPDEF   *pCurrentPPMFrame, PPM_FRAME_TYPDEF   *pLastPPMFrame );
void onChannelPPMValueChange(uint8_t ch, PPM_FRAME_TYPDEF   *pCurrentPPMFrame, PPM_FRAME_TYPDEF   *pLastPPMFrame );
void enablePPMDecoder(bool enable);
void initPPMFrameDecoder(void);
void toggleDebugPPMDecoder(void);
void toggleEnableDisablePPMDecoder(void);
ButtonStats_Typedef getRCButtonStatus(uint8_t value);

#ifdef __cplusplus
}
#endif

#endif /* SOURCE_CONTROLS_PPMFRAMEDECODER_H_ */

#endif
