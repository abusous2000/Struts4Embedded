/*
 * eByteLora.h
 *
 * Inspired by Kris Kasprzak's Arduino library located at
 * https://github.com/KrisKasprzak/EBYTE
 */
#include "Strust4EmbeddedConf.h"
#ifndef SOURCE_EBYTELORA_EBYTELORA_H_
#define SOURCE_EBYTELORA_EBYTELORA_H_

#if S4E_USE_EBYTE_LORA != 0

#define EBYTE_LORA_THD_STACK_SIZE		512//1024
#ifndef EBYTE_LORA_SAVE_PARAMS
#define EBYTE_LORA_SAVE_PARAMS			0
#endif
#define EBYTE_LORA_MODE      			PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_MID2
#define PIN_RECOVER 					20
#define PERMANENT 						0xC0
#define TEMPORARY 						0xC2
// modes NORMAL send and recieve for example
#define MODE_NORMAL 					0			// can send and recieve
#define MODE_WAKEUP 					1			// sends a preamble to waken receiver
#define MODE_POWERDOWN 					2			// can't transmit but receive works only in wake up mode
#define MODE_PROGRAM 					3			// for programming
#define PB_8N1 							0b00			// default
#define PB_8O1 							0b01
#define PB_8E1 							0b11

//UART data rates
// (can be different for transmitter and reveiver)
#define UDR_1200 						0b000		// 1200 baud
#define UDR_2400 						0b001		// 2400 baud
#define UDR_4800 						0b010		// 4800 baud
#define UDR_9600 						0b011		// 9600 baud default
#define UDR_19200 						0b100		// 19200 baud
#define UDR_38400 						0b101		// 34800 baud
#define UDR_57600 						0b110		// 57600 baud
#define UDR_115200 						0b111	// 115200 baud

// air data rates (certian types of modules)
// (must be the same for transmitter and reveiver)
#define ADR_300 						0b000		// 300 baud
#define ADR_1200 						0b001		// 1200 baud
#define ADR_2400 						0b010		// 2400 baud
#define ADR_4800 						0b011		// 4800 baud
#define ADR_9600 						0b100		// 9600 baud
#define ADR_19200 						0b101		// 19200 baud

// air data rates (other types of modules)
#define ADR_1K 							0b000		// 1k baud
#define ADR_2K 							0b001		// 2K baud
#define ADR_5K 							0b010		// 4K baud
#define ADR_8K 							0b011		// 8K baud
#define ADR_10K 						0b100		// 10K baud
#define ADR_15K 						0b101		// 15K baud
#define ADR_20K 						0b110		// 20K baud
#define ADR_25K 						0b111		// 25K baud

// various options
// (can be different for transmitter and reveiver)
#define OPT_FMDISABLE	 				0b0	//default
#define OPT_FMENABLE	 				0b1
#define OPT_IOOPENDRAIN		 			0b0
#define OPT_IOPUSHPULL	 				0b1
#define OPT_WAKEUP250	 				0b000
#define OPT_WAKEUP500	 				0b001
#define OPT_WAKEUP750	 				0b010
#define OPT_WAKEUP1000	 				0b011
#define OPT_WAKEUP1250	 				0b100
#define OPT_WAKEUP1500	 				0b101
#define OPT_WAKEUP1750	 				0b110
#define OPT_WAKEUP2000	 				0b111
#define OPT_FECDISABLE		  			0b0
#define OPT_FECENABLE	 				0b1
#define OPT_FMDISABLE	 				0b0	//default
#define OPT_FMENABLE	 				0b1
#define OPT_IOOPENDRAIN		 			0b0
#define OPT_IOPUSHPULL		  			0b1
#define OPT_WAKEUP250	  				0b000
#define OPT_WAKEUP500	  				0b001
#define OPT_WAKEUP750	  				0b010
#define OPT_WAKEUP1000	 				0b011
#define OPT_WAKEUP1250	 				0b100
#define OPT_WAKEUP1500	 				0b101
#define OPT_WAKEUP1750	 				0b110
#define OPT_WAKEUP2000	 				0b111
#define OPT_FECDISABLE		  			0b0
#define OPT_FECENABLE	 				0b1

#ifdef EBYTE_LORA_1000MW
#define OPT_TP30	 					0b00		// 30 db
#define OPT_TP27	 					0b01		// 27 db
#define OPT_TP24	 					0b10		// 24 db
#define OPT_TP21	 					0b11		// 21 db
#elif defined(EBYTE_LORA_500MW)
// constants or 500 mW units
#define OPT_TP27	 					0b00		// 27 db
#define OPT_TP24	 					0b01		// 24 db
#define OPT_TP21	 					0b10		// 21 db
#define OPT_TP18	 					0b11		// 17 db
#define OPT_TP17	 					0b11		// 17 db
#else
// constants or 100 mW units
#define OPT_TP20	 					0b00		// 20 db
#define OPT_TP17	 					0b01		// 17 db
#define OPT_TP14	 					0b10		// 14 db
#define OPT_TP11	 					0b11		// 10 db
#define OPT_TP10 						0b11		// 10 db
#endif

#define NONE_BUTTON   		0b0
#define TOGGLE_MUTE_BUTTON 	0b1
#define TOGGLE_PAUSE_BUTTON 0b10
#define NEXT_TRACK_BUTTON   0b100
#define PREV_TRACK_BUTTON   0b1000
#define VOLUME_UP_BUTTON    0b10000
#define VOLUME_DOWN_BUTTON  0b100000


#ifndef EBYTE_LORA_DEFAULT_IO_TIMEOUT
#define EBYTE_LORA_DEFAULT_IO_TIMEOUT 	20
#endif

#ifndef EBYTE_LORA_DEFAULT_REC_TIMEOUT
#define EBYTE_LORA_DEFAULT_REC_TIMEOUT 	50
#endif

#ifndef EBYTE_LORA_SLEEP_MS_BTWN_MSGS
#define EBYTE_LORA_SLEEP_MS_BTWN_MSGS 	100
#endif

#ifndef EBYTE_LORA_HOST_ID
#define EBYTE_LORA_HOST_ID              1
#endif

#ifndef EBYTE_LORA_SEND_PERIODIC_MSG
#define EBYTE_LORA_SEND_PERIODIC_MSG    1
#endif

#ifndef DEFAULT_ADDRESS_HIGH
#define DEFAULT_ADDRESS_HIGH			1
#endif

#ifndef DEFAULT_ADDRESS_LOW
#define DEFAULT_ADDRESS_LOW				0
#endif

#ifndef DEFAULT_TO_ADDRESS_HIGH
#define DEFAULT_TO_ADDRESS_HIGH			1
#endif

#ifndef DEFAULT_TO_ADDRESS_LOW
#define DEFAULT_TO_ADDRESS_LOW  		0
#endif

#ifndef DEFAULT_TO_CHANNEL
#define DEFAULT_TO_CHANNEL				5
#endif

#ifndef DEFAULT_CHANNEL
#define DEFAULT_CHANNEL					5
#endif

#ifndef DEFAULT_OPTION_FIXED_TRANS
#define DEFAULT_OPTION_FIXED_TRANS		0
#endif

#define DEFAULT_AIR_DATA_RATE			ADR_2400//ADR_300//ADR_2400
#define DEFAULT_UART_DATA_RATE			UDR_9600
#define DEFAULT_PARITY_BIT				PB_8N1
#define DEFAULT_OPTION_PULLUP			1
#define DEFAULT_OPTION_WAKEUP			OPT_WAKEUP250
#define DEFAULT_OPTION_POWER			OPT_TP20
#define DEFAULT_OPTION_FEC				1
#define DEFAULT_UART_TIMEOUT            5

#define MSG_RECEIVED_MSG_TYPE	    	1
#define POT_JOYSTICK_MSG_TYPE	    	2

typedef struct{
	uint8_t _Params[6];

	// indicidual variables for each of the 6 bytes
	// _Params could be used as the main variable storage, but since some bytes
	// are a collection of several options, let's just make storage consistent
	// also Param[1] is different data depending on the _Save variable
	uint8_t _Save;
	uint8_t _AddressHigh;
	uint8_t _AddressLow;
	uint8_t _Speed;
	uint8_t _Channel;
	uint8_t _Options;


	// individual variables for all the options
	uint8_t _ParityBit;
	uint8_t _UARTDataRate;
	uint8_t _AirDataRate;
	uint8_t _OptionTrans;
	uint8_t _OptionPullup;
	uint8_t _OptionWakeup;
	uint8_t _OptionFEC;
	uint8_t _OptionPower;
	uint16_t _Address;
	uint8_t _Model;
	uint8_t _Version;
	uint8_t _Features;
	uint8_t _buf;

}EByteParams_TypeDef;
#ifndef EBYTE_LORA_MAX_PAYLOAD_SIZE
#define EBYTE_LORA_MAX_PAYLOAD_SIZE 	20
#endif

#if DEFAULT_OPTION_FIXED_TRANS != 0
#define OPTION_TRANS_FIELDS 			uint8_t         toAddressHigh; \
										uint8_t         toAddressLow; \
										uint8_t 		toChannel;
#define FIXED_TRANS_PAD			        3
#else
#define OPTION_TRANS_FIELDS
#define FIXED_TRANS_PAD			        0
#endif
//Make sure that this structure stays representing everything in bytes/chars/uint8_t, else
//it won't pack/align well, and the client won't be able to easily unpack it
typedef struct {
	OPTION_TRANS_FIELDS
	uint8_t 	fromAddressHigh;
	uint8_t 	fromAddressLow;
	uint8_t 	fromChannel;
    uint8_t     msgTypeID;
    uint8_t     hostID;
    uint16_t  	frameID;
    uint8_t  	payloadSize;
	uint8_t  	payload[EBYTE_LORA_MAX_PAYLOAD_SIZE];
 }EByteLoRaFrame_TypeDef;
 typedef struct {
	uint16_t  	    frameID;
 }EByteLoRaMsgReceived_TypeDef;
typedef struct {
	#ifdef EBYTE_LORA_MSG_DATA
	EBYTE_LORA_MSG_DATA
	#else
	uint8_t					   volume;
    uint8_t                    buttons;
	#endif
}MyMessage_TypeDef;

enum PROGRAM_COMMAND
{
  WRITE_CFG_PWR_DWN_SAVE  	= 0xC0,
  READ_CONFIGURATION 		= 0xC1,
  WRITE_CFG_PWR_DWN_LOSE 	= 0xC2,
  READ_MODULE_VERSION   	= 0xC3,
  WRITE_RESET_MODULE     	= 0xC4
};
#ifdef __cplusplus
 extern "C" {
#endif

void eByteReset(void);
void eBytePopulateFrame(EByteLoRaFrame_TypeDef *pEByteLoRaFrame,void *pPayload, uint8_t payloadSize, uint8_t msgTypeID, EByteLoRaFrame_TypeDef	*pFromMsgPayload);
void eByteSendMsg(EByteLoRaFrame_TypeDef *pEByteLoRaFrame);
size_t eByteReceiveMsg(EByteLoRaFrame_TypeDef *pEByteLoRaFrame, MyMessage_TypeDef		*pPayload);
void eBytePrintParameters(void);
bool eByteInit(void);
void eByteOnMsgReply(EByteLoRaFrame_TypeDef   *pEByteLoRaFrame);
void eByteProcessReplyMsgs(void);
void eByteProcessReceivedMsg( EByteLoRaFrame_TypeDef	*pEByteLoRaFrame, MyMessage_TypeDef *pMyPayload);
void initEByteLoraThread(void);
void eByteSendAckMsg(EByteLoRaFrame_TypeDef	*pEByteLoRaFrame);
#ifdef __cplusplus
 }
#endif
#endif /* S4E_USE_EBYTE_LORA */
#endif /* SOURCE_EBYTELORA_EBYTELORA_H_ */
