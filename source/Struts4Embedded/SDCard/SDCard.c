/*
 * SDCard.c
 *
 *  Created on: Aug 2, 2019
 *      Author: abusous2000
 */
#include "Strust4EmbeddedConf.h"
#if S4E_USE_SDCARD != 0
#include "ch.h"
#include "hal.h"
#include "Strust4Embedded.h"
#include "SDCard.h"
#include "ccportab.h"

CC_WEAK FRESULT processFile (char *filePath){(void)filePath;

  dbgprintf("processFile: %s\r\n", filePath);
  return FR_OK;
}

#if HAL_USE_MMC_SPI
static FATFS MMC_FS;
//This variable is expected to be name this way to work with FatFS
MMCDriver MMCD1;

/* Maximum speed SPI configuration (18MHz, CPHA=0, CPOL=0, MSb first).*/
static SPIConfig hs_spicfg 	= {.circular=FALSE, .end_cb=NULL, .ssport=PAL_PORT(SD_CARD_NSS_LINE), .sspad=PAL_PAD(SD_CARD_NSS_LINE), .cr1=0, .cr2=0};
/* Low speed SPI configuration (281.250kHz, CPHA=0, CPOL=0, MSb first).*/
static SPIConfig ls_spicfg 	= {.circular=FALSE, .end_cb=NULL, .ssport=PAL_PORT(SD_CARD_NSS_LINE), .sspad=PAL_PAD(SD_CARD_NSS_LINE), .cr1=SPI_CR1_BR_2 | SPI_CR1_BR_1, .cr2=0};
static MMCConfig MMCDrv_Config={.spip=&PROTAB_MMCSPI, .lscfg=&ls_spicfg, .hscfg=&hs_spicfg} ;

bool my_mmc_lld_is_card_inserted(MMCDriver *sdcp){(void)sdcp;
 return palReadLine(SD_CARD_NSS_LINE);
}
#else
static FATFS SDC_FS;
#endif

static FRESULT disconnectSDCard(SDCardDriverITF_Typedef *pSDCardDriverITF);
static FRESULT processFiles(SDCardDriverITF_Typedef *pSDCardDriverITF);
static FRESULT mountSDCard(SDCardDriverITF_Typedef *pSDCardDriverITF);
static FRESULT initMMC(SDCardDriverITF_Typedef *pSDCardDriverITF);
SDCardDriverITF_Typedef  SDCardDriverImpl = {  .readFilesFromFolder="/music",
		                                       	      .init=initMMC,
													  .disconnect=disconnectSDCard,
													  .mount=mountSDCard,
													  .processFiles=processFiles,
													  .processFile=processFile};



SDCardDriverITF_Typedef *getSDCardDriver(void){
	return &SDCardDriverImpl;
}

FRESULT initMMC(SDCardDriverITF_Typedef *pSDCardDriverITF) {
	if ( pSDCardDriverITF->currentState != SDCARD_DISCONNECTED )
		  return FR_OK;
#if HAL_USE_MMC_SPI

  palSetLineMode(SD_CARD_NSS_LINE,  PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
#ifndef SDMMC_ALREADY_CONFIG
  palSetLineMode(SD_CARD_SCK_LINE,  MMC_SPI_PIN_MODE );
  palSetLineMode(SD_CARD_MISO_LINE, MMC_SPI_PIN_MODE_MISO );
  palSetLineMode(SD_CARD_MOSI_LINE, MMC_SPI_PIN_MODE );
#endif
  palSetLine(SD_CARD_NSS_LINE); // set NSS high

  // initialize MMC driver
  mmcObjectInit(&MMCD1);
  //MMCD1.vmt->is_inserted=my_mmc_lld_is_card_inserted;
  mmcStart(&MMCD1, &MMCDrv_Config);
#elif HAL_USE_SDC == TRUE
  #ifndef SDMMC_ALREADY_CONFIG
  palSetLineMode(SDMMMC_DETECT,  PAL_MODE_INPUT_PULLUP); // SCK-white
  palSetLineMode(SDMMCD0_LINE,  PAL_MODE_ALTERNATE(SDMMCD0_LINE_AF) 	 | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_OTYPE_PUSHPULL);//AF11
  palSetLineMode(SDMMCD1_LINE,  PAL_MODE_ALTERNATE(SDMMCD1_LINE_AF) 	 | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_OTYPE_PUSHPULL);//AF11
  palSetLineMode(SDMMCD2_LINE,  PAL_MODE_ALTERNATE(SDMMCD2_LINE_AF) 	 | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_OTYPE_PUSHPULL);//AF10
  palSetLineMode(SDMMCD3_LINE,  PAL_MODE_ALTERNATE(SDMMCD3_LINE_AF) 	 | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_OTYPE_PUSHPULL);//AF10
  palSetLineMode(SDMMCCK_LINE,  PAL_MODE_ALTERNATE(SDMMCCK_LINE_AF) 	 | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_OTYPE_PUSHPULL);//AF11
  palSetLineMode(SDMMCMD_LINE,  PAL_MODE_ALTERNATE(SDMMCMD_LINE_AF) 	 | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_OTYPE_PUSHPULL);//AF11
  #endif

  sdcStart(&PROTTABLE_SDC, NULL);
#endif


  pSDCardDriverITF->currentState = SDCARD_INIT;
  pSDCardDriverITF->readFilesFromFolder = getSysProperty(READ_FILES_FROM_FOLDER);

  return FR_OK;
}

FRESULT mountSDCard(SDCardDriverITF_Typedef *pSDCardDriverITF){
  FRESULT err;
 if ( pSDCardDriverITF->currentState == SDCARD_MOUNTED ||  pSDCardDriverITF->currentState == SDCARD_PROCESSING)
	return FR_OK;
#if HAL_USE_MMC_SPI
  if(mmcConnect(&MMCD1)) {
	dbgprintf("mountSDCard: Failed to connect to card\r\n");
	return -1;
  }
  else
	dbgprintf( "mountSDCard: Connected to card\r\n");

  err = f_mount(&MMC_FS, "/",1);
  if(err != FR_OK){
     dbgprintf("mountSDCard: f_mount() failed %d\r\n", err);
	 mmcDisconnect(&MMCD1);
	 return err;
  }
  else
	 dbgprintf( "mountSDCard: File system mounted\r\n");
#elif HAL_USE_SDC
  if (sdcConnect(&PROTTABLE_SDC))
    return FR_INT_ERR;
  err = f_mount(&SDC_FS, "/", 1);
  if (err != FR_OK) {
    sdcDisconnect(&PROTTABLE_SDC);
    return err;
  }
#endif
  pSDCardDriverITF->currentState = SDCARD_MOUNTED;
  return FR_OK;
}
static char 	fpath[150];
static FRESULT processFiles(SDCardDriverITF_Typedef *pSDCardDriverITF){
  FILINFO 		fno;
  DIR 			dir;

  dbgprintf("processFiles: Playing all files in: \"%s\"\r\n", pSDCardDriverITF->readFilesFromFolder);
  FRESULT res = f_opendir(&dir, pSDCardDriverITF->readFilesFromFolder);
  if(res != FR_OK){
	  dbgprintf("processFiles: f_opendir failed %d\r\n", res);
	  return res;
  }
  res = f_readdir(&dir, &fno);
  while((res == FR_OK) && (fno.fname[0] != 0)) {
	  pSDCardDriverITF->currentState = SDCARD_PROCESSING;
      strlcpy(fpath, pSDCardDriverITF->readFilesFromFolder,sizeof(fpath));
      if ( strcmp(pSDCardDriverITF->readFilesFromFolder,"/") != 0 )
    	  strcat(fpath, "/");
      strcat(fpath, fno.fname);
      if(fno.fattrib & AM_DIR)
    	  dbgprintf("--->processFiles: DIR \"%s\"\r\n", fpath);
      else
      if ( pSDCardDriverITF->processFile != NULL) {
    	  res = pSDCardDriverITF->processFile(fpath);
     	  if ( res != FR_OK)
    		  break;
        }
      res = f_readdir(&dir, &fno);
  }
  dbgprintf("--->processFiles: Finished playing all files in: %s\r\n", pSDCardDriverITF->readFilesFromFolder);

  return res;
}
static FRESULT disconnectSDCard(SDCardDriverITF_Typedef *pSDCardDriverITF){
	pSDCardDriverITF->currentState = SDCARD_DISCONNECTED;
#if HAL_USE_MMC_SPI
	return mmcDisconnect(&MMCD1)?FR_OK:FR_INT_ERR;
#elif HAL_USE_SDC
	return sdcDisconnect(&PROTTABLE_SDC)?FR_OK:FR_INT_ERR;
#endif

}

#endif
