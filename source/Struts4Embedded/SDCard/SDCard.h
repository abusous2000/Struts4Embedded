/*
 * SDCard.h
 *
 *  Created on: Aug 2, 2019
 *      Author: abusous2000
 */
#include "Strust4EmbeddedConf.h"
#if S4E_USE_SDCARD == 1

#ifndef SOURCE_SDCARD_SDCARD_H_
#define SOURCE_SDCARD_SDCARD_H_
#include "ch.h"
#include "hal.h"
#include <ff.h>
#include <dbgtrace.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef SDMMCD0_LINE
  //See stm32f765bg-Datasheet-AFTable.pdf page 88 to 100
#define SDMMCD0_LINE 	LINE_SD_D0		//  PAL_LINE(GPIOG,9)
#define SDMMCD1_LINE 	LINE_SD_D1		//  PAL_LINE(GPIOG,10)
#define SDMMCD2_LINE 	LINE_SD_D2		//  PAL_LINE(GPIOB,3)
#define SDMMCD3_LINE 	LINE_SD_D3		//  PAL_LINE(GPIOB,4)
#define SDMMCCK_LINE 	LINE_SD_CLK		//  PAL_LINE(GPIOD,6)
#define SDMMCMD_LINE 	LINE_SD_CMD		//  PAL_LINE(GPIOD,7)
#define SDMMMC_DETECT   LINE_SD_DETECT	////PAL_LINE(GPIOI,15)

#endif

#ifdef BOARD_ST_STM32F769I_DISCOVERY
#define SDMMCD0_LINE_AF 	11		//  PAL_LINE(GPIOG,9)
#define SDMMCD1_LINE_AF 	11		//  PAL_LINE(GPIOG,10)
#define SDMMCD2_LINE_AF 	10		//  PAL_LINE(GPIOB,3)
#define SDMMCD3_LINE_AF 	10		//  PAL_LINE(GPIOB,4)
#define SDMMCCK_LINE_AF 	11		//  PAL_LINE(GPIOD,6)
#define SDMMCMD_LINE_AF 	11		//  PAL_LINE(GPIOD,7)
#define SDMMMC_DETECT_AF    LINE_SD_DETECT	////PAL_LINE(GPIOI,15)

#elif defined(BOARD_ST_STM32F746G_DISCOVERY)
#define SDMMCD0_LINE_AF 	12		//  PAL_LINE(GPIOC, 8U)
#define SDMMCD1_LINE_AF  	12		//  PAL_LINE(GPIOC, 9U)
#define SDMMCD2_LINE_AF  	12		//  PAL_LINE(GPIOC, 10U)
#define SDMMCD3_LINE_AF  	12		//  PAL_LINE(GPIOC, 11U)
#define SDMMCCK_LINE_AF 	12		//  PAL_LINE(GPIOC, 12U)
#define SDMMCMD_LINE_AF 	12		//  PAL_LINE(GPIOC, 12U)
#define SDMMMC_DETECT_AF    12		//  PAL_LINE(GPIOC, 13U)

#endif
#if STM32_SDC_USE_SDMMC1 == 1
#define PROTTABLE_SDC SDCD1
#elif STM32_SDC_USE_SDMMC2 == 1
#define PROTTABLE_SDC SDCD2
#endif
#define SDIO_NOT_CONFIGURED TRUE
#define READ_FILES_FROM_FOLDER "readFilesFromFolder"


 enum SDCardState {
	 SDCARD_DISCONNECTED 	= 0,
	 SDCARD_INIT 			= 1,
	 SDCARD_MOUNTED 		= 2,
	 SDCARD_PROCESSING 		= 3,
 };
struct SDCardDriverITF;
typedef struct SDCardDriverITF SDCardDriverITF_Typedef;
typedef FRESULT	(*SDCardMethod)(SDCardDriverITF_Typedef *pSDCardDriverITF);

typedef struct SDCardDriverITF{
  char      		*readFilesFromFolder;
  enum SDCardState 	currentState;
  SDCardMethod      init;
  SDCardMethod      processFiles;
  FRESULT   		(*processFile) (char *filePath);
  SDCardMethod      mount;
  SDCardMethod      disconnect;
}SDCardDriverITF_Typedef;

#ifdef __cplusplus
 extern "C" {
#endif
 SDCardDriverITF_Typedef *getSDCardDriver(void);
#ifdef __cplusplus
}
#endif

#endif /* SOURCE_SDCARD_SDCARD_H_ */

#endif
