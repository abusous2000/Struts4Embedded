#ifndef __W25QXX_H
#define __W25QXX_H

#if USE_W25Q_XXXX != 0

#include "ch.h"
#include "hal.h"
#include "board.h"
#define W25Q08 							0XEF13
#define W25Q16 							0XEF14
#define W25Q32 							0XEF15
#define W25Q64 							0XEF16
#define W25Q128							0XEF17


#define W25X_CMD_WriteEnable		  	0x06
#define W25X_CMD_WriteDisable		  	0x04
#define W25X_CMD_ReadStatusReg			0x05
#define W25X_CMD_WriteStatusReg			0x01
#define W25X_CMD_ReadData			    0x03 
#define W25X_CMD_FastReadData		  	0x0B
#define W25X_CMD_FastReadDual		  	0x3B
#define W25X_CMD_PageProgram		  	0x02
#define W25X_CMD_BlockErase			  	0xD8
#define W25X_CMD_SectorErase		  	0x20
#define W25X_CMD_ChipErase			  	0xC7
#define W25X_CMD_PowerDown			  	0xB9
#define W25X_CMD_ReleasePowerDown	  	0xAB
#define W25X_CMD_DeviceID			    0xAB 
#define W25X_CMD_ManufactDeviceID		0x90
#define W25X_CMD_JedecDeviceID			0x9F


#if defined(STM32F407xx)

/** @defgroup SPI_data_direction
  * @{
  */

#define SPI_Direction_2Lines_FullDuplex 0x0000
#define SPI_Direction_2Lines_RxOnly     0x0400
#define SPI_Direction_1Line_Rx          0x8000
#define SPI_Direction_1Line_Tx          0xC000
#define IS_SPI_DIRECTION_MODE(MODE) (((MODE) == SPI_Direction_2Lines_FullDuplex) || \
                                     ((MODE) == SPI_Direction_2Lines_RxOnly) || \
                                     ((MODE) == SPI_Direction_1Line_Rx) || \
                                     ((MODE) == SPI_Direction_1Line_Tx))
/**
  * @}
  */

/** @defgroup SPI_mode
  * @{
  */

#define SPI_Mode_Master                 0x0104
#define SPI_Mode_Slave                  0x0000
#define IS_SPI_MODE(MODE) (((MODE) == SPI_Mode_Master) || \
                           ((MODE) == SPI_Mode_Slave))
/**
  * @}
  */

/** @defgroup SPI_data_size
  * @{
  */

#define SPI_DataSize_16b                0x0800
#define SPI_DataSize_8b                 0x0000
#define IS_SPI_DATASIZE(DATASIZE) (((DATASIZE) == SPI_DataSize_16b) || \
                                   ((DATASIZE) == SPI_DataSize_8b))
/**
  * @}
  */

/** @defgroup SPI_Clock_Polarity
  * @{
  */

#define SPI_CPOL_Low                    0x0000
#define SPI_CPOL_High                   0x0002
#define IS_SPI_CPOL(CPOL) (((CPOL) == SPI_CPOL_Low) || \
                           ((CPOL) == SPI_CPOL_High))
/**
  * @}
  */

/** @defgroup SPI_Clock_Phase
  * @{
  */

#define SPI_CPHA_1Edge                  0x0000
#define SPI_CPHA_2Edge                  0x0001
#define IS_SPI_CPHA(CPHA) (((CPHA) == SPI_CPHA_1Edge) || \
                           ((CPHA) == SPI_CPHA_2Edge))
/**
  * @}
  */

/** @defgroup SPI_Slave_Select_management
  * @{
  */

#define SPI_NSS_Soft                    0x0200
#define SPI_NSS_Hard                    0x0000
#define IS_SPI_NSS(NSS) (((NSS) == SPI_NSS_Soft) || \
                         ((NSS) == SPI_NSS_Hard))
/**
  * @}
  */

/** @defgroup SPI_BaudRate_Prescaler
  * @{
  */

#define SPI_BaudRatePrescaler_2         0x0000
#define SPI_BaudRatePrescaler_4         0x0008
#define SPI_BaudRatePrescaler_8         0x0010
#define SPI_BaudRatePrescaler_16        0x0018
#define SPI_BaudRatePrescaler_32        0x0020
#define SPI_BaudRatePrescaler_64        0x0028
#define SPI_BaudRatePrescaler_128       0x0030
#define SPI_BaudRatePrescaler_256       0x0038
#define IS_SPI_BAUDRATE_PRESCALER(PRESCALER) (((PRESCALER) == SPI_BaudRatePrescaler_2) || \
                                              ((PRESCALER) == SPI_BaudRatePrescaler_4) || \
                                              ((PRESCALER) == SPI_BaudRatePrescaler_8) || \
                                              ((PRESCALER) == SPI_BaudRatePrescaler_16) || \
                                              ((PRESCALER) == SPI_BaudRatePrescaler_32) || \
                                              ((PRESCALER) == SPI_BaudRatePrescaler_64) || \
                                              ((PRESCALER) == SPI_BaudRatePrescaler_128) || \
                                              ((PRESCALER) == SPI_BaudRatePrescaler_256))
/**
  * @}
  */

/** @defgroup SPI_MSB_LSB_transmission
  * @{
  */

#define SPI_FirstBit_MSB                0x0000
#define SPI_FirstBit_LSB                0x0080
#define IS_SPI_FIRST_BIT(BIT) (((BIT) == SPI_FirstBit_MSB) || \
                               ((BIT) == SPI_FirstBit_LSB))
#endif

void W25QXX_Init(void);
uint16_t  W25QXX_ReadID(void);
uint8_t	 W25QXX_ReadSR(void);
void W25QXX_Write_SR(uint8_t sr);
void W25QXX_Write_Enable(void);
void W25QXX_Write_Disable(void);

void W25QXX_Erase_Chip(void);
void W25QXX_Erase_Sector(uint32_t First_Addr);
int W25QXX_Wait_Busy(void);
void W25QXX_PowerDown(void);
void W25QXX_WAKEUP(void);

void W25QXX_Read(uint8_t* DataBuffer,uint32_t StartAddress,uint16_t ByteCount);
void W25QXX_Write_Page(uint8_t* DataBuffer,uint32_t StartAddress,uint16_t ByteCount);

void W25QXX_PageWrite(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
void W25QXX_SectorWrite(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
uint16_t getW25QXX_ID(void);


#endif

#endif

