#include "P9813_RGB_Driver.h"

#if S4E_USE_RGB != 0

#define _CLK_PULSE_DELAY    2
#define _CL_RED             0
#define _CL_GREEN           1
#define _CL_BLUE            2

static CPalTypedef IOData={        .line=LINE_IO_DATA,             .mode=RGB_LED_GPIO_MODE};
static CPalTypedef IOClk ={        .line=LINE_IO_CLOCK,            .mode=RGB_LED_GPIO_MODE};
static CPalTypedef *pIOData = NULL, *pIOClk = NULL;
//io_data(D5)
static uint8_t numOfLEDs;
static bool    driverInit = false;
static uint8_t ledState[20];

void initP9813RGBDriver(uint8_t num_leds){
	if ( !driverInit){
		numOfLEDs = num_leds;
		pIOData = initCPalInstance(&IOData);pIOData->init(pIOData);pIOData->clear(pIOData);
		pIOClk  = initCPalInstance(&IOClk);pIOClk->init(pIOClk);pIOClk->clear(pIOClk);
		driverInit = true;
	}

	return;
}
static void pulseIt(void){
    pIOClk->clear(pIOClk);
    chThdSleepMilliseconds(_CLK_PULSE_DELAY);
    pIOClk->set(pIOClk);
    chThdSleepMilliseconds(_CLK_PULSE_DELAY);
	return;
}
static void sendByte(uint8_t b){
    // Send one bit at a time, starting with the MSB
    for (uint8_t i=0; i<8; i++){
        // If MSB is 1, write one and clock it, else write 0 and clock
        if ((b & 0x80) != 0)
			pIOData->set(pIOData);
        else
        	pIOData->clear(pIOData);
        pulseIt();
        // Advance to the next bit to send
        b <<= 1;
    }

	return;
}
static void sendColor(uint8_t red, uint8_t green, uint8_t blue){
    // Start by sending a uint8_t with the format "1 1 /B7 /B6 /G7 /G6 /R7 /R6"
    uint8_t prefix = 0xC0;
    if ((blue  & 0x80) == 0)    prefix|= 0x20;
    if ((blue  & 0x40) == 0)    prefix|= 0x10;
    if ((green & 0x80) == 0)    prefix|= 0x08;
    if ((green & 0x40) == 0)    prefix|= 0x04;
    if ((red   & 0x80) == 0)    prefix|= 0x02;
    if ((red   & 0x40) == 0)    prefix|= 0x01;
    sendByte(prefix);

    // Now must send the 3 colors
    sendByte(blue);
    sendByte(green);
    sendByte(red);

	return;
}
//setColorRGB(led, (uint8_t)(255.0f*r), (uint8_t)(255.0f*g), (uint8_t)(255.0f*b));
void setColorRGB(uint8_t led, uint8_t red, uint8_t green, uint8_t blue){
	if ( !driverInit)
		initP9813RGBDriver(TOTAL_NUM_OF_LEDS);
    // Send data frame prefix (32x "0")
	for (uint8_t i=0; i<4; i++)
		sendByte(0x00);

    // Send color data for each one of the leds
    for (uint8_t i=0; i<numOfLEDs; i++){
        if (i == led){
            ledState[i*3 + _CL_RED]   = red;
            ledState[i*3 + _CL_GREEN] = green;
            ledState[i*3 + _CL_BLUE]  = blue;
        }
        sendColor(ledState[i*3 + _CL_RED],
                  ledState[i*3 + _CL_GREEN],
                  ledState[i*3 + _CL_BLUE]);
    }
    // Terminate data frame (32x "0")
	for (uint8_t i=0; i<4; i++)
		sendByte(0x00);

	return;
}
#endif
