/*
 * PotReader.c
 *
 *  Created on: Aug 23, 2019
 *      Author: abusous2000
 */
#include "PotReader.h"

#include "Strust4EmbeddedConf.h"
#include "ActionEventsThd.h"

#if S4E_USE_POT == 1


#define ADC_GRP1_NUM_CHANNELS   1
#define ADC_GRP1_BUF_DEPTH      2

//By default these buffers are placed in .ram3 (DTCM-RAM ) by default, you can check the .map file
//Therefore there is no need to invalidate the cache to get around cache coherence issue
// __attribute__((section(".nocache.adcBuffer")))
//PLACE_IN_RAM_SECTION(.nocache.adcBuffer)
static adcsample_t adcBuffer[ADC_GRP1_NUM_CHANNELS * ADC_GRP1_BUF_DEPTH];// __attribute__((section(".ram3")));

/*
 * ADC streaming callback.
 */
adcsample_t potValue = 0;
static void adccallback(ADCDriver *adcp) {
  if (adcIsBufferComplete(adcp)) {
    uint32_t totalValue = 0;
     //uint8_t volume = 100 * samples1[0]/4096;
    for (uint8_t i = 0; i < ADC_GRP1_BUF_DEPTH; ++i)
      totalValue +=adcBuffer[i];
    potValue = (adcsample_t)(totalValue/ADC_GRP1_BUF_DEPTH);
  }
}
adcsample_t  getPotValue(void){
  return potValue;
}
/*
 * GPT4 configuration. This timer is used as trigger for the ADC.
 */

static const GPTConfig gptcfg = {
  .frequency =  ADC_TIM_FREQ, //At what clock freq. u need the TM to be clocked? 10KHz Which's used to calc. pre-scaler in the TIM reg. = STM32_TIMCLK1
                              //(STM32_TIMCLK1=STM32_PCLK1 * 2 see hal_lld.h 2067)=108MHz is clock config in CubeMX)/.frequency. see line 883 in hal_gpt_lld.c for details

  .callback  =  NULL,
  .cr2       =  TIM_CR2_MMS_1,  /* MMS = 010 = tiggers a TRGO on Update Event.  See page 751 in the ref. manual       */
  .dier      =  0U
};
/*
 * ADC errors callback, should never happen.
 */
static void adcerrorcallback(ADCDriver *adcp, adcerror_t err) {

  (void)adcp;
  (void)err;
}
/*
 * ADC conversion group.
 * Mode:   Continuous, 1 channel, HW triggered by GPT4-TRGO.
 * NOTES for ADC config... plz see Ref. Manual for diagram on page 439, note on the lower right hand corner-->TIM4_TGRO
 * 1-See in the Datasheet to figure out which pin is assigned to which ADC channel (see column "addition functions" page 70 in Table 10 pin and ball definition)
 * 2- assign the right channel macro in the the sequence i.e ADC_SQR3_SQx_N & ADC_SMPRx_SMP_ANy where x is 1,2,3 & y is channel number [1-16]
 * 3- assign the correct speed in cycles for each ADC
 * 4- configure the timer trigger if needed (rising edge and which timer ADC_CR2_EXTSEL_SRC(12) ), see diagram on p. 962 RF. Also see CubeMX. It will help a lot with the reading the clock
 *    4.1 .frequency param in TIM config (STM32_TIMCLK1 which (STM32_PCLK1 * 1)=54MHz is clock config in CubeMX) is used to divide the time's clock to get the prescaler.
 *         in this case it is 54, meaning the clock was reduced to 1 Mhz.
 *    4.2 calling gptStartContinuous expects the interval which defines every how many cycles the timer tiggers. In this example every 100 cycles or 10K tigers per second
 * 5- See Note at the end of this file for equations that configures the timer 4 prescaler
 * 6- Look at the MCU's XML file located CubeX $HOME/STM32CubeMX/db/mcu/STM32F769NGHx.xml to see which functionality is available for the port in question
 *
 * See this example for **excellent code** that uses 2 channels with analog watch dog:
 *   /testhal/STM32/multi/ADC/cfg/stm32h743_nucleo144/portab.c
 */

static const ADCConversionGroup adcgrpcfg = {
  .circular=true,
  .num_channels=ADC_GRP1_NUM_CHANNELS,
  .end_cb=adccallback,
  .error_cb=adcerrorcallback,
  .cr1=0,
  .cr2=ADC_CR2_EXTEN_RISING | ADC_CR2_EXTSEL_SRC(TIM_TGRO_EVENT_EXTSEL),        /* CR2 --See RM pages 451 & 471 for details  */
  /***Figure out where ur channels belongs to which SMPR[1-2] and to which SQR[1-3],
   *see line 412 @hal_adc_lld.h header file for details
  */
  .smpr1=0,              					/* SMPR1 */
  .smpr2=ADC_SMPR2_SMP_AN6(ADC_SAMPLE_144), /* SMPR2 *///Define number of cycles for N6. See header file & RM on p. 473-4 for details
  .htr=0,                                   /* HTR  */ //no thresholds for analog watch dogs */
  .ltr=0,                                   /* LTR  */
  .sqr1=0,                                  /* SQR1 */
  .sqr2=0,                                  /* SQR2 */
  .sqr3=ADC_SQR3_SQ1_N(ADC_CHANNEL_IN)     /* SQR3 */ //See Table 10 on p. 70 in datasheet. Pay attention to column "Additional functionality"
                                                       //for pin PA6 (LINE_ARD_A0) top see which analog channel is assigned to pin
                                        	    	   //The extracted relevant PDFs & images are in repository
};


void initPotReader(void) {
  palSetLineMode(POT_GPIO_PIN, PAL_MODE_INPUT_ANALOG);
  gptStart(&ADC_POT_TIM, &gptcfg);

  /*
   * Fixed an errata on the STM32F7xx, the DAC clock is required for ADC triggering.
   */
  rccEnableDAC1(false);

  /*
   * Activates the ADC1 driver and the temperature sensor.
   */
  adcStart(&ADC_POT, NULL);
  /*
   * Starts an ADC continuous conversion triggered with a period of 1/4 second. 4 samples per seconds
   */
  adcStartConversion(&ADC_POT, &adcgrpcfg, adcBuffer, ADC_GRP1_BUF_DEPTH);
  /*Here is back trace for the clock
   * #define STM32_HSECLK                8000000U
	 #define STM32_PLLCLKIN              (STM32_HSECLK / STM32_PLLM_VALUE)= 25000000U / 25
	 #define STM32_PLLVCO                (STM32_PLLCLKIN * STM32_PLLN_VALUE)= (25000000U / 25) * 432

	 #define STM32_PLLM_VALUE            25
	 #define STM32_PLLN_VALUE            432
	 #define STM32_PLLP_VALUE            2
	 #define STM32_PLL_P_CLKOUT          (STM32_PLLVCO / STM32_PLLP_VALUE) = ((25000000U / 25) * 432) / 2
	 #define STM32_SYSCLK                STM32_PLL_P_CLKOUT = ((25000000U / 25) * 432) / 2
	 #define STM32_HCLK                  (STM32_SYSCLK / 1) = ((25000000U / 25) * 432) / 2
	 #define STM32_PCLK1                 (STM32_HCLK / 4)   = (((25000000U / 25) * 432) / 2) / 4
	 #define STM32_TIMCLK1               (STM32_PCLK1 * 2)  = ((((25000000 / 25) * 432) / 2) / 4) * 2
   * prescaler = Timer Clock / (Desired frequency - 1)--> where Timer Clock ( see MCU Block Diagram APB1 buss & CubeX Clock Diagram) = 54MHz= STM32_PCLK1 = (STM32_HCLK / 4) (see line1390 in hal_ldd.h)
   *                                                    STM32_PCLK2 --> see CubeX Clock diagram
   * 												&   STM32_PCLK2 = STM32_HCLK / 2 = 108MHz(see header file)
   * 												&   STM32_HCLK =STM32_SYSCLK/1 = 216MHz, Therefore
   * 												Prescaler = (gptp->clock / gptp->config->frequency) - 1)=(108Mhz/(10000 -1) =  108MHz * 10^6 / 10^4=10800
   *
   * How many update event per second (in Hz)= Timer Clock /((prescaler+1)*(period-1))=Desired frequency/(period-1)-->10KHz/2.5K=4Hz-->Equation as follows
   * =108MHz /(10.8KHz*2.5K)= 4 samples per second
   */
  gptStartContinuous(&ADC_POT_TIM, TIM_PERIOD_CYCLES);
}

static uint8_t  				lastPotValue = 0;
#define ABS(N) ((N<0)?(-N):(N))
static ActionEvent_Typedef 		*setVolumeActionEvent = NULL;
void checkOnPotVolumeChange(void){
  int8_t   potValue   = POT_VALUE_BTWN_0_TO_100;
  int8_t   delta      = potValue- lastPotValue;

  if ( setVolumeActionEvent == NULL )
	  setVolumeActionEvent = getActionEvent(SET_VOLUME_AE_NAME);
  //ignore changes within 1% margin of error
  if ( ABS(delta) > 1 ){
	  lastPotValue = potValue;
	  triggerActionEvent(setVolumeActionEvent->name,NULL,potValue,"pot");
  }
}

#endif

