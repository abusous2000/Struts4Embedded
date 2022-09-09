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
#define PPM_FREQUENCY_USED          		60000
#endif
#ifndef PERIOD_USED
#define PERIOD_USED          		    60000
#endif
#ifndef RC_ERR_MARGIN
#define RC_ERR_MARGIN                   2
#endif
#ifndef RC_MIN_VALUE
#define RC_MIN_VALUE                    36
#endif
#ifndef RC_MAX_VALUE
#define RC_MAX_VALUE                    96
#endif
#ifndef RC_MID_VALUE
#define RC_MID_VALUE                    66
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
#define PPM_MIN_GAP_BTWN_FRAMES     	450
#endif

#ifndef PPM_CHANNEL_WIDTH
#define PPM_CHANNEL_WIDTH        		100
#endif
#define PPM_WITHIN_MARGIN(x,y)          (x >= (y-RC_ERR_MARGIN) &&  x <= (y+RC_ERR_MARGIN))

#define IBUS_LENGTH                     0x20
#define IBUS_COMMAND40                  0x40
#define IBUS_MAX_CHANNLES               14
#define IBUS_PIN_MODE                   PAL_MODE_ALTERNATE(8)| PAL_STM32_OSPEED_HIGHEST | PAL_STM32_OTYPE_PUSHPULL
#define IBUS_SLEEP_BTWN_MSGS            150
enum ButtonStats {
  BUTTON_STATE_LOW     = 0,
  BUTTON_STATE_MID     = 1,
  BUTTON_STATE_HIGH    = 2,
  BUTTON_STATE_UNKNOWN = 3
};

typedef enum ButtonStats ButtonStats_Typedef;

typedef struct {
	uint8_t   		valueInCycles[MAX_CHANNELS];
	uint8_t  		period[MAX_CHANNELS];
	uint32_t  		gap;
	bool      		processed;
	uint32_t  		frameID;
	systime_t   	start;
	systime_t   	end;
	sysinterval_t	frameDuration;
	uint32_t 	    frameDurationMicroSec;
}PPM_FRAME_TYPDEF;

#ifdef __cplusplus
 extern "C" {
#endif
void onChannelPPMValueChange(uint8_t ch, uint8_t currentValue, uint8_t newValue);
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
