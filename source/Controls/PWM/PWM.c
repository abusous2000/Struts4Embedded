/*
 * PWM.c
 *
 *  Created on: Jan 1, 2020
 *      Author: abusous2000
 */
#include "Strust4EmbeddedConf.h"
#if S4E_USE_PWM == 1
#include "PWM.h"


static void pwmcb(PWMDriver *pwmp) {
  (void)pwmp;
}

static void pwmc1cb(PWMDriver *pwmp) {
  (void)pwmp;
}
static void pwmc2cb(PWMDriver *pwmp) {
  (void)pwmp;
}
static PWMConfig pwmcfg = {
  TIM8_FREQ,                                    /* 10kHz PWM clock frequency.   */
  TIM8_FREQ,                                    /* Initial PWM period 1s.       */
  pwmcb,
  {
   {PWM_OUTPUT_ACTIVE_HIGH, pwmc1cb},
   {PWM_OUTPUT_ACTIVE_HIGH, pwmc2cb},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL}
  },
  0,
  0
};

void setPWMDutyCycle(uint32_t dc1, uint32_t dc2){
  pwmEnableChannel(&PWM_DRIVER, 0, PWM_PERCENTAGE_TO_WIDTH(&PWM_DRIVER, dc1));
//  pwmEnableChannelNotification(&PWM_DRIVER, 0);
  pwmEnableChannel(&PWM_DRIVER, 1, PWM_PERCENTAGE_TO_WIDTH(&PWM_DRIVER, dc2));
//  pwmEnableChannelNotification(&PWM_DRIVER, 0);
}
void reInitPWM(uint32_t  frequency, pwmcnt_t    period , double dc1, double dc2){
  /*
   * -look at the Arduino header PIN diagram, usually they state which PWM driver & channel to use
   * -find the AF number from AF table
   * -set duty cycle
   * -set up channel notification if required
   * Visit this link for details: https://gamazeps.github.io/posts/stm_pwm_driver.html
   */
  palSetLineMode(LINE_ARD_D1, PAL_MODE_ALTERNATE(3));//PC6
  palSetLineMode(LINE_ARD_D0, PAL_MODE_ALTERNATE(3));//PC7

  pwmcfg.period    = period;
  if ( PWM_DRIVER.state != PWM_STOP ){
	  if ( pwmcfg.frequency == frequency)
		  pwmChangePeriod(&PWM_DRIVER,period);
	  else
	      pwmStop(&PWM_DRIVER);
  }
  else
	  pwmcfg.frequency = frequency;
  if ( PWM_DRIVER.state == PWM_STOP )
	  pwmStart(&PWM_DRIVER, &pwmcfg);//PWM8/1
// pwmEnablePeriodicNotification(&PWMD8);
  setPWMDutyCycle(pwmcfg.frequency*dc1,pwmcfg.frequency*dc2);
}

void initPWM(void){
  reInitPWM(TIM8_FREQ, TIM8_FREQ,.5f, 0.2f);
}

#endif

