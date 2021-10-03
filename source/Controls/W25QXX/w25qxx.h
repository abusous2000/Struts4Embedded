#ifndef __W25QXX_H
#define __W25QXX_H

#if USE_W25Q_XXXX != 0
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

