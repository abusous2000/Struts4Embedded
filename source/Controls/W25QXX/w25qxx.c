#include "Strust4EmbeddedConf.h"
#if USE_W25Q_XXXX != 0
#include "ch.h"
#include "hal.h"
#include "Strust4Embedded.h"
#include "w25qxx.h"  
//#define USES_SINGLE_BYTE


#if defined(BOARD_BLACKBOARD_INDUSTRIAL2) && !defined(USES_SINGLE_BYTE)
//For some reason I was ONLY able to get this boad to work with single byte mode!!! BOARD_BLACKBOARD_INDUSTRIAL was OK
#define USES_SINGLE_BYTE
#if 0
#define W25QXX_SPI_CR1    (uint16_t)((uint32_t)SPI_Direction_2Lines_FullDuplex | SPI_Mode_Master | SPI_DataSize_8b |\
                                               SPI_CPOL_High                   | SPI_CPHA_2Edge  | SPI_NSS_Soft    | \
											   SPI_BaudRatePrescaler_2         | SPI_FirstBit_MSB)
#endif
#endif


#ifndef W25QXX_SPI_CR1
#define W25QXX_SPI_CR1        0
#endif
#ifndef W25QXX_SPI_CR2
#define W25QXX_SPI_CR2        0
#endif


static SPIConfig hs_spicfg = {  .circular = FALSE,
		                        .end_cb = NULL,
							    .ssport =PAL_PORT(W25QXX_SPI_CS_LINE),
								.sspad = PAL_PAD(W25QXX_SPI_CS_LINE),
								.cr1=W25QXX_SPI_CR1,
								.cr2=W25QXX_SPI_CR2 };

uint8_t SPIX_ReadWriteByte(uint8_t tx) {
	uint8_t rxBuff[2] = {0};
	uint8_t txBuff[2] = {tx,0};

	spiExchange(&W25QXX_SPID, 1, &txBuff[0], &rxBuff[0]);

	return rxBuff[0];
}
void SPIX_ReadWriteBytes(uint8_t *txBuff, uint8_t *rxBuff, uint16_t size) {
//	spiAcquireBus(&W25QXX_SPID);
//	spiSelect(&W25QXX_SPID);
	spiExchange(&W25QXX_SPID, size, txBuff, rxBuff);
//	spiUnselect(&W25QXX_SPID);
//	spiReleaseBus(&W25QXX_SPID);

	return;
}

void SPIX_ReadWrite4Bytes(uint8_t cmd, uint32_t data, uint8_t *rxBuffIn){
	uint8_t rxBuff[4] = {0};
	uint8_t txBuff[4] = {cmd,(uint8_t)((data) >> 16),(uint8_t)((data) >> 8),(uint8_t)data};

	SPIX_ReadWriteBytes(txBuff, (rxBuffIn==NULL?rxBuff:rxBuffIn),sizeof(txBuff));
}


void SPIX_Init(void) {
	palSetLineMode(W25QXX_SPI_SCK_LINE, W25QXX_SPI_MODE); /* New SCK.     */
	palSetLineMode(W25QXX_SPI_MISO_LINE, W25QXX_SPI_MODE); /* New MISO.    */
	palSetLineMode(W25QXX_SPI_MOSI_LINE, W25QXX_SPI_MODE); /* New MOSI.    */
	palSetLineMode(W25QXX_SPI_CS_LINE, W25QXX_SPI_CS_MODE); /* New CS.      */
	palSetLine(W25QXX_SPI_CS_LINE);
#if defined(BOARD_BLACKBOARD_INDUSTRIAL2)
	W25QXX_SPID.spi->CRCPR = 7;
	W25QXX_SPID.spi->I2SCFGR &= (uint16_t)~((uint16_t)SPI_I2SCFGR_I2SMOD);
#endif
	spiStart(&W25QXX_SPID, &hs_spicfg); /* Setup transfer parameters.       */
	SPIX_ReadWriteByte(0xff);

	W25QXX_WAKEUP();
}

static uint16_t W25QXX_ID = 0;

void W25QXX_Init(void) {
	//SPI FLASH
	SPIX_Init();
	W25QXX_ID = W25QXX_ReadID();
}
uint16_t getW25QXX_ID(void){
	return W25QXX_ID;
}
uint8_t W25QXX_ReadSR(void) {
#ifdef USES_SINGLE_BYTE
	spiAcquireBus(&W25QXX_SPID);
	spiSelect(&W25QXX_SPID);

	SPIX_ReadWriteByte(W25X_CMD_ReadStatusReg);
	uint8_t byte = SPIX_ReadWriteByte(0Xff);

	spiUnselect(&W25QXX_SPID);
	spiReleaseBus(&W25QXX_SPID);

	return byte;
#else
	uint8_t rxBuff[2] = {0};
	uint8_t txBuff[2] = {W25X_CMD_ReadStatusReg,0xFF};

	SPIX_ReadWriteBytes(txBuff,rxBuff,sizeof(txBuff));
	return rxBuff[1];
#endif
}
void W25QXX_Write_SR(uint8_t sr) {
#ifdef USES_SINGLE_BYTE
	spiAcquireBus(&W25QXX_SPID);
	spiSelect(&W25QXX_SPID);

	SPIX_ReadWriteByte(W25X_CMD_WriteStatusReg);
	SPIX_ReadWriteByte(sr);

	spiUnselect(&W25QXX_SPID);
	spiReleaseBus(&W25QXX_SPID);
#else
	uint8_t rxBuff[2] = {0};
	uint8_t txBuff[2] = {W25X_CMD_ReadStatusReg,sr};

	SPIX_ReadWriteBytes(txBuff,rxBuff,sizeof(txBuff));
#endif
}

void W25QXX_Write_Enable(void) {
	spiAcquireBus(&W25QXX_SPID);
	spiSelect(&W25QXX_SPID);
#ifdef USES_SINGLE_BYTE
	SPIX_ReadWriteByte(W25X_CMD_WriteEnable);
#else
	SPIX_ReadWrite4Bytes(W25X_CMD_WriteEnable, 0,NULL);
#endif
	spiUnselect(&W25QXX_SPID);
	spiReleaseBus(&W25QXX_SPID);

}

int W25QXX_Wait_Busy(void) {
	int i = 0;

	while ((W25QXX_ReadSR() & 0x01) == 0x01){
		chThdSleepMicroseconds(10);
#if !defined(BOARD_BLACKBOARD_INDUSTRIAL2)
		if ( i++ > 3000 ){
			break;
		}
#endif
	}

	return i;
}

void W25QXX_Write_Disable(void) {
	spiAcquireBus(&W25QXX_SPID);
	spiSelect(&W25QXX_SPID);
#ifdef USES_SINGLE_BYTE
	SPIX_ReadWriteByte(W25X_CMD_WriteDisable);

#else
	SPIX_ReadWrite4Bytes(W25X_CMD_WriteDisable, 0,NULL);
#endif
	spiUnselect(&W25QXX_SPID);
	spiReleaseBus(&W25QXX_SPID);
}

uint16_t W25QXX_ReadID(void) {
	uint16_t IDnum = 0;
	uint8_t rxBuff[6] = {0};
	uint8_t txBuff[6] = {W25X_CMD_ManufactDeviceID,0x00,0x00,0x00,0xFF,0xFF};

	spiAcquireBus(&W25QXX_SPID);
	spiSelect(&W25QXX_SPID);
//	SPIX_ReadWriteByte(W25X_CMD_ManufactDeviceID);
//	SPIX_ReadWriteBytes(&txBuff[0],&rxBuff[0],1);
	SPIX_ReadWriteBytes(&txBuff[0],&rxBuff[0],6);
//	SPIX_ReadWriteBytes(&txBuff[2],&rxBuff[2],3);
//	SPIX_ReadWriteBytes(&txBuff[2],&rxBuff[2],2);
//	SPIX_ReadWriteBytes(&txBuff[4],&rxBuff[4],1);
//	SPIX_ReadWriteBytes(&txBuff[3],&rxBuff[3],3);

	IDnum |=rxBuff[4]<<8;
	IDnum |=rxBuff[5];
	spiUnselect(&W25QXX_SPID);
	spiReleaseBus(&W25QXX_SPID);
	IDnum = 0;

	spiAcquireBus(&W25QXX_SPID);
	spiSelect(&W25QXX_SPID);
	SPIX_ReadWriteByte(W25X_CMD_ManufactDeviceID);
	SPIX_ReadWriteByte(0x00);
	SPIX_ReadWriteByte(0x00);
	SPIX_ReadWriteByte(0x00);

	IDnum |= SPIX_ReadWriteByte(0xFF) << 8;
	IDnum |= SPIX_ReadWriteByte(0xFF);
	spiUnselect(&W25QXX_SPID);


	spiReleaseBus(&W25QXX_SPID);

	IDnum = 0;

	spiAcquireBus(&W25QXX_SPID);
	spiSelect(&W25QXX_SPID);
	for(int i = 0; i < 6; ++i){
		rxBuff[i] = 0;
	   spiExchange(&W25QXX_SPID, 1, &txBuff[i], &rxBuff[i]);
	}
	spiUnselect(&W25QXX_SPID);


	spiReleaseBus(&W25QXX_SPID);
	IDnum |=rxBuff[4]<<8;
	IDnum |=rxBuff[5];

	IDnum = 0;
	spiAcquireBus(&W25QXX_SPID);
	spiSelect(&W25QXX_SPID);
	SPIX_ReadWriteBytes(txBuff,rxBuff,sizeof(txBuff));
	spiUnselect(&W25QXX_SPID);
	spiReleaseBus(&W25QXX_SPID);
	  /*
	   *        0XEF13 ==> W25Q80
	            0XEF14 ==> W25Q16
	            0XEF15 ==> W25Q32
	            0XEF16 ==> W25Q64
	            0XEF17 ==> W25Q128
	   */
	IDnum |=rxBuff[4]<<8;
	IDnum |=rxBuff[5];
	uint8_t sr = W25QXX_ReadSR();

	return IDnum;
}

void W25QXX_Erase_Chip(void) {
	W25QXX_Write_Enable();
	W25QXX_Wait_Busy();
	spiAcquireBus(&W25QXX_SPID);
	spiSelect(&W25QXX_SPID);
#ifdef USES_SINGLE_BYTE
	SPIX_ReadWriteByte(W25X_CMD_ChipErase);
#else
	SPIX_ReadWrite4Bytes(W25X_CMD_ChipErase, 0,NULL);
#endif
	spiUnselect(&W25QXX_SPID);
	spiReleaseBus(&W25QXX_SPID);

	W25QXX_Wait_Busy();
}
void W25QXX_Erase_Sector(uint32_t First_Addr) {
	First_Addr *= 4096;
	W25QXX_Write_Enable();
	W25QXX_Wait_Busy();

	spiAcquireBus(&W25QXX_SPID);
	spiSelect(&W25QXX_SPID);
#ifdef USES_SINGLE_BYTE
	SPIX_ReadWrite4Bytes(W25X_CMD_SectorErase,First_Addr,NULL);
//	SPIX_ReadWriteByte(W25X_CMD_SectorErase);
//	SPIX_ReadWriteByte((uint8_t)((First_Addr) >> 16));
//	SPIX_ReadWriteByte((uint8_t)((First_Addr) >> 8));
//	SPIX_ReadWriteByte((uint8_t) First_Addr);
#else
	SPIX_ReadWrite4Bytes(W25X_CMD_SectorErase,First_Addr,NULL);
#endif
	spiUnselect(&W25QXX_SPID);
	spiReleaseBus(&W25QXX_SPID);

	W25QXX_Wait_Busy();
}

void W25QXX_PowerDown(void) {
	spiAcquireBus(&W25QXX_SPID);
	spiSelect(&W25QXX_SPID);
#ifdef USES_SINGLE_BYTE
	SPIX_ReadWriteByte(W25X_CMD_PowerDown);
#else
	SPIX_ReadWrite4Bytes(W25X_CMD_PowerDown,0,NULL);
#endif
	spiUnselect(&W25QXX_SPID);
	spiReleaseBus(&W25QXX_SPID);

	chThdSleepMicroseconds(3);
}

void W25QXX_WAKEUP(void) {
	spiAcquireBus(&W25QXX_SPID);
	spiSelect(&W25QXX_SPID);

#ifdef USES_SINGLE_BYTE
	SPIX_ReadWriteByte(W25X_CMD_ReleasePowerDown);
#else
	SPIX_ReadWrite4Bytes(W25X_CMD_ReleasePowerDown,0,NULL);
#endif
	spiUnselect(&W25QXX_SPID);
	spiReleaseBus(&W25QXX_SPID);

	chThdSleepMicroseconds(3);
}
#define TX_BUFF_SIZE   128
void W25QXX_Read(uint8_t* DataBuffer, uint32_t StartAddress, uint16_t ByteCount) {
	spiAcquireBus(&W25QXX_SPID);
	spiSelect(&W25QXX_SPID);

#ifdef USES_SINGLE_BYTE
	SPIX_ReadWrite4Bytes(W25X_CMD_ReadData,StartAddress,NULL);
//	SPIX_ReadWriteByte(W25X_CMD_ReadData);
//	SPIX_ReadWriteByte((uint8_t)((StartAddress) >> 16));
//	SPIX_ReadWriteByte((uint8_t)((StartAddress) >> 8));
//	SPIX_ReadWriteByte((uint8_t) StartAddress);
	while (ByteCount--) {
		*DataBuffer = SPIX_ReadWriteByte(0XFF);
		DataBuffer++;
	}
#else
	SPIX_ReadWrite4Bytes(W25X_CMD_ReadData,StartAddress,NULL);
	uint16_t ndx = 0;
	uint16_t bytesToRead = ByteCount;
	while (bytesToRead > 0 ) {
		uint8_t txBuff[TX_BUFF_SIZE]={[0 ... (TX_BUFF_SIZE-1)] = 0xFF};
		uint8_t batchSize = bytesToRead > TX_BUFF_SIZE?TX_BUFF_SIZE: bytesToRead;
		spiExchange(&W25QXX_SPID, batchSize, txBuff, DataBuffer+ndx);
//		SPIX_ReadWriteBytes(txBuff,DataBuffer+ndx,batchSize);
		bytesToRead -= batchSize;
		ndx +=batchSize;
	}
#endif
	spiUnselect(&W25QXX_SPID);
	spiReleaseBus(&W25QXX_SPID);

}
void W25QXX_Write_Page(uint8_t* DataBuffer, uint32_t StartAddress, uint16_t ByteCount) {
	W25QXX_Write_Enable();
	spiAcquireBus(&W25QXX_SPID);
	spiSelect(&W25QXX_SPID);
#ifdef USES_SINGLE_BYTE2
	SPIX_ReadWrite4Bytes(W25X_CMD_PageProgram, StartAddress,NULL);
//	SPIX_ReadWriteByte(W25X_CMD_PageProgram);
//	SPIX_ReadWriteByte((uint8_t)((StartAddress) >> 16));
//	SPIX_ReadWriteByte((uint8_t)((StartAddress) >> 8));
//	SPIX_ReadWriteByte((uint8_t) StartAddress);
	while (ByteCount--) {
		SPIX_ReadWriteByte(*DataBuffer);
		DataBuffer++;
	}
#else
	SPIX_ReadWrite4Bytes(W25X_CMD_PageProgram, StartAddress,NULL);
	uint16_t ndx = 0;
	uint16_t bytesToRead = ByteCount;

	while (bytesToRead > 0 ) {
		uint8_t rxBuff[TX_BUFF_SIZE]={0};
		uint8_t batchSize = bytesToRead > TX_BUFF_SIZE?TX_BUFF_SIZE: bytesToRead;
		spiExchange(&W25QXX_SPID, batchSize, DataBuffer+ndx, rxBuff);
//		SPIX_ReadWriteBytes(DataBuffer+ndx,rxBuff,batchSize);
		bytesToRead -= batchSize;
		ndx +=batchSize;
	}
#endif
	spiUnselect(&W25QXX_SPID);
	spiReleaseBus(&W25QXX_SPID);

	W25QXX_Wait_Busy();
}
void W25QXX_PageWrite(uint8_t* DataBuffer, uint32_t StartAddress,	uint16_t ByteCount) {
	uint8_t NumOfPage = 0, NumOfSingle = 0, Surplus_Addr = 0, Surplus_count = 0,
			midtemp = 0;

	Surplus_Addr = StartAddress % 256;
	Surplus_count = 256 - Surplus_Addr;
	NumOfPage = ByteCount / 256;
	NumOfSingle = ByteCount % 256;

	if (Surplus_Addr == 0) {
		if (NumOfPage == 0)  // ByteCount < 256
				{
			W25QXX_Write_Page(DataBuffer, StartAddress, ByteCount);
		} else                 // ByteCount > 256
		{
			while (NumOfPage--) {
				W25QXX_Write_Page(DataBuffer, StartAddress, 256);
				StartAddress += 256;
				DataBuffer += 256;
			}
			W25QXX_Write_Page(DataBuffer, StartAddress, NumOfSingle);
		}
	} else {
		if (NumOfPage == 0) //
				{
			if (NumOfSingle > Surplus_count) {
				midtemp = NumOfSingle - Surplus_count;
				W25QXX_Write_Page(DataBuffer, StartAddress, Surplus_count);
				StartAddress += Surplus_count;
				DataBuffer += Surplus_count;
				W25QXX_Write_Page(DataBuffer, StartAddress, midtemp);
			} else {
				W25QXX_Write_Page(DataBuffer, StartAddress, ByteCount);
			}
		} else //ByteCount > 256
		{
			ByteCount -= Surplus_count;
			NumOfPage = ByteCount / 256;
			NumOfSingle = ByteCount % 256;

			W25QXX_Write_Page(DataBuffer, StartAddress, Surplus_count);
			StartAddress += Surplus_count;
			DataBuffer += Surplus_count;
			while (NumOfPage--) {
				W25QXX_Write_Page(DataBuffer, StartAddress, 256);
				StartAddress += 256;
				DataBuffer += 256;
			}
			if (NumOfSingle != 0) {
				W25QXX_Write_Page(DataBuffer, StartAddress, NumOfSingle);
			}
		}
	}
}

static uint8_t TS_BUFFER[4096] = {0};
void W25QXX_SectorWrite(uint8_t* DataBuffer, uint32_t StartAddress,
		uint16_t ByteCount) {
	uint32_t secaddr;
	uint16_t secused;
	uint16_t Surplus_count;
	uint16_t i = 0;

	uint8_t * Cache_BUF;
	Cache_BUF = TS_BUFFER;

	secaddr = StartAddress / 4096;
	secused = StartAddress % 4096;
	Surplus_count = 4096 - secused;
	if (ByteCount <= Surplus_count)
		Surplus_count = ByteCount;
	while (1) {
		W25QXX_Read(Cache_BUF, secaddr * 4096, 4096);
		i = 0;
		while (Cache_BUF[secused + i] == 0XFF) {
			i++;
			if(i == Surplus_count)
			break;
		}
		if (i < Surplus_count) {
			W25QXX_Erase_Sector(secaddr);
			for (i = 0; i < Surplus_count; i++) {
				Cache_BUF[i + secused] = DataBuffer[i];
			}
			W25QXX_PageWrite(Cache_BUF, secaddr * 4096, 4096);

		} else
			W25QXX_PageWrite(DataBuffer, StartAddress, Surplus_count);
		if (ByteCount == Surplus_count)
			break;
		else {
			secaddr++;
			secused = 0;

			DataBuffer += Surplus_count;
			StartAddress += Surplus_count;
			ByteCount -= Surplus_count;
			if (ByteCount > 4096)
				Surplus_count = 4096;
			else
				Surplus_count = ByteCount;
		}
	};
}
#endif
