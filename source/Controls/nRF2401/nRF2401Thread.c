/*
    PLAY Embedded demos - Copyright (C) 2014...2019 Rocco Marco Guglielmi

    This file is part of PLAY Embedded demos.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/


#include "Strust4EmbeddedConf.h"
#include "userconf.h"
#if USERLIB_USE_RF != 0
#include "ch.h"
#include "hal.h"
#include "dbgtrace.h"
#include "rf.h"
#include "string.h"


#define  FRAME_LEN                            5

static SPIConfig std_spi_cfg = { .circular = FALSE,
		                        .end_cb = NULL,
							    .ssport =PAL_PORT(NRF24L01_SPI_CS),
								.sspad = PAL_PAD(NRF24L01_SPI_CS),
								.cr1=SPI_CR1_BR_1 | SPI_CR1_BR_0,
								.cr2 = 0 };
static RFConfig nrf24l01_cfg = {
  NRF24L01_LINE_CE,
  NRF24L01_LINE_IRQ,
  &NRF24L01_SPID,
  &std_spi_cfg,
  NRF24L01_ARC_15_times,     /* auto_retr_count */
  NRF24L01_ARD_4000us,       /* auto_retr_delay */
  NRF24L01_AW_5_bytes,       /* address_width */
  2,                       /* channel_freq 2.4 + 0.02 GHz */
  NRF24L01_ADR_2Mbps,       /* data_rate */
  NRF24L01_PWR_0dBm,         /* out_pwr */
  NRF24L01_LNA_disabled,     /* lna */
  NRF24L01_DPL_enabled ,     /* en_dpl */
  NRF24L01_ACK_PAY_disabled, /* en_ack_pay */
  NRF24L01_DYN_ACK_disabled  /* en_dyn_ack */
};

/*===========================================================================*/
/* Generic code.                                                             */
/*===========================================================================*/



static THD_WORKING_AREA(wanRF2401Thread, NRF24L01_THD_STACK_SIZE );
static THD_FUNCTION(nRF2401Thread, arg) {(void)arg;
  /*
   * SPID1 I/O pins setup.(It bypasses board.h configurations)
   */
  palSetLineMode(NRF24L01_SPI_SCK,  NRF24L01_SPI_MODE);
  palSetLineMode(NRF24L01_SPI_MISO, NRF24L01_SPI_MODE);
  palSetLineMode(NRF24L01_SPI_MOSI, NRF24L01_SPI_MODE);
  palSetLineMode(NRF24L01_SPI_CS,   NRF24L01_SPI_CS_MODE);
  /*
   * CE and IRQ pins setup.
   */
  palSetLineMode(NRF24L01_LINE_CE,  NRF24L01_SPI_CS_MODE);
  palSetLineMode(NRF24L01_LINE_IRQ, NRF24L01_SPI_IRQ_MODE);

  /* RF Driver Object constructor. */
  rfInit();

  /* Starting RF driver. */
  rfStart(&RFD1, &nrf24l01_cfg);
  int msgId = 0;
  char msg[RF_MAX_STRLEN + 1] = "Hello Salah";
  uint32_t strl;
  while (TRUE) {
#if TRANSMITTER == TRUE
    char c;
    rf_msg_t msgRC = RF_OK;
    uint8_t counter = 0;
    chsnprintf(msg,sizeof(msg),"Msg:%d",++msgId);
    strl = strlen(msg);
    if(strl){
      msgRC = rfTransmitString(&RFD1, msg, "RXadd", TIME_MS2I(75));
      if(msgRC == RF_OK){
    	  dbgprintf("Message sent:%s\n\r",msg);
      }
      else if(msgRC == RF_ERROR){
        dbgprintf("Message not sent (MAX_RT)\n\r");
      }
      else{
        dbgprintf("Message not sent (TIMEOUT)\n\r");
      }
    }
    chThdSleepMilliseconds(550);
#else
    msg[0] = '\0';
    rfReceiveString(&RFD1, msg, "RXadd", TIME_MS2I(200));
    strl = strlen(msg);
    if(strl){
      dbgprintf(msg);
      dbgprintf("\n\r");
    }
#endif
    }
    rfStop(&RFD1);
 }

void initNRF2401(void){
    chThdCreateStatic(wanRF2401Thread,  sizeof(wanRF2401Thread),  NORMALPRIO+2,     nRF2401Thread, NULL);
}
#endif
