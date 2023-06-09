
#include "ssd1306.h"
#if S4E_USE_SSD1306_LCD != 0
#include <string.h>
#include "ssd1306_font.h"
#include "CommonHelper.h"


/* Steal from https://stm32f4-discovery.net/2015/05/library-61-ssd1306-oled-i2c-lcd-for-stm32f4xx/ */

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

static msg_t wrCmd(void *ip, uint8_t cmd) {
  const SSD1306Driver *drvp = (const SSD1306Driver *)ip;
  msg_t ret;
  uint8_t txbuf[] = { 0x00, cmd };

  i2cAcquireBus(drvp->config->i2cp);
  if (  drvp->config->i2cp->state != I2C_READY)
    i2cStart(drvp->config->i2cp, drvp->config->i2ccfg);

  ret = i2cMasterTransmitTimeout(drvp->config->i2cp, drvp->config->sad,
                                 txbuf, sizeof(txbuf), NULL, 0, TIME_INFINITE);

  i2cReleaseBus(drvp->config->i2cp);

  return ret;
}

static msg_t wrDat(void *ip, uint8_t *txbuf, uint16_t len) {
  const SSD1306Driver *drvp = (const SSD1306Driver *)ip;
  msg_t ret;

  i2cAcquireBus(drvp->config->i2cp);
  if (  drvp->config->i2cp->state != I2C_READY)
    i2cStart(drvp->config->i2cp, drvp->config->i2ccfg);

  ret = i2cMasterTransmitTimeout(drvp->config->i2cp, drvp->config->sad,
                                 txbuf, len, NULL, 0, TIME_INFINITE);

  i2cReleaseBus(drvp->config->i2cp);

  return ret;
}

static void updateScreen(void *ip) {
  SSD1306Driver *drvp = (SSD1306Driver *)ip;
  uint8_t idx;

  for (idx = 0; idx < 8; idx++) {
    wrCmd(drvp, 0xB0 + idx);
    wrCmd(drvp, 0x00);
    wrCmd(drvp, 0x10);

    // Write multi data
    wrDat(drvp, &drvp->fb[SSD1306_WIDTH_FIXED * idx], SSD1306_WIDTH_FIXED);
  }
}

static void toggleInvert(void *ip) {
  SSD1306Driver *drvp = (SSD1306Driver *)ip;
  uint16_t idx;

  // Toggle invert
  drvp->inv = !drvp->inv;

  for (idx = 0; idx < sizeof(drvp->fb); idx++) {
    if (idx % SSD1306_WIDTH_FIXED == 0) continue;
    drvp->fb[idx] = ~drvp->fb[idx];
  }
}


static void fillRowScreen(void *ip, ssd1306_color_t color, uint8_t idx) {
  SSD1306Driver *drvp = (SSD1306Driver *)ip;

  drvp->fb[SSD1306_WIDTH_FIXED * idx] = 0x40;
  memset(&drvp->fb[SSD1306_WIDTH_FIXED * idx + 1],color == SSD1306_COLOR_BLACK ? 0x00 : 0xff, SSD1306_WIDTH-1);
}
static void fillScreen(void *ip, ssd1306_color_t color) {
  SSD1306Driver *drvp = (SSD1306Driver *)ip;
  uint8_t idx;

  for (idx = 0; idx < 8; idx++)
    fillRowScreen(ip,color,idx);
}
static void drawPixel(void *ip, uint8_t x, uint8_t y, ssd1306_color_t color) {
  SSD1306Driver *drvp = (SSD1306Driver *)ip;
  if (x > SSD1306_WIDTH || y > SSD1306_HEIGHT) return;

  // Check if pixels are inverted
  if (drvp->inv) {
    color = (ssd1306_color_t)!color;
  }

  // Set color
  if (color == SSD1306_COLOR_WHITE) {
    drvp->fb[x + (y / 8) * SSD1306_WIDTH_FIXED + 1] |= 1 << (y % 8);
  } else {
    drvp->fb[x + (y / 8) * SSD1306_WIDTH_FIXED + 1] &= ~(1 << (y % 8));
  }
}

static void gotoXy(void *ip, uint8_t x, uint8_t y) {
  SSD1306Driver *drvp = (SSD1306Driver *)ip;

  drvp->x = x;
  drvp->y = y;
}

static char PUTC(void *ip, char ch, const ssd1306_font_t *font, ssd1306_color_t color) {
  SSD1306Driver *drvp = (SSD1306Driver *)ip;
  uint32_t i, b, j;

  // Check available space in OLED
  if (drvp->x + font->fw >= SSD1306_WIDTH ||
      drvp->y + font->fh >= SSD1306_HEIGHT) {
    return 0;
  }

  // Go through font
  for (i = 0; i < font->fh; i++) {
    b = font->dt[(ch - 32) * font->fh + i];
    for (j = 0; j < font->fw; j++) {
      if ((b << j) & 0x8000) {
        drawPixel(drvp, drvp->x + j, drvp->y + i, color);
      } else {
        drawPixel(drvp, drvp->x + j, drvp->y + i,(ssd1306_color_t)! color);
      }
    }
  }

  //dirty hack for 7x10 font
  /*	for (i = 0; i < font->fh; i++) {
  		b = font->dt[(ch - 32) * font->fh + i];
  		for (j = 1; j < (uint32_t)(font->fw + 1); j++) {
  			if ((b << j) & 0x8000) {
  				drawPixel(drvp, drvp->x + j, drvp->y + i + 1, color);
  			} else {
  				drawPixel(drvp, drvp->x + j, drvp->y + i + 1,(ssd1306_color_t)! color);
  			}
  		}
  	}
  */

  // Increase pointer
  drvp->x += font->fw;

  // Return character written
  return ch;
}

static char PUTS(void *ip, char *str, const ssd1306_font_t *font, ssd1306_color_t color) {
  // Write characters
  while (*str) {
    // Write character by character
    if (PUTC(ip, *str, font, color) != *str) {
      // Return error
      return *str;
    }

    // Increase string pointer
    str++;
  }

  // Everything OK, zero should be returned
  return *str;
}

static void setDisplay(void *ip, uint8_t on) {
  wrCmd(ip, 0x8D);
  wrCmd(ip, on ? 0x14 : 0x10);
  wrCmd(ip, 0xAE);
}

static const struct SSD1306VMT vmt_ssd1306 = {
  updateScreen, toggleInvert, fillRowScreen,fillScreen, drawPixel,
  gotoXy, PUTC, PUTS, setDisplay
};

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

void ssd1306ObjectInit(SSD1306Driver *devp) {
  devp->vmt = &vmt_ssd1306;
  devp->config = NULL;

  devp->state = SSD1306_STOP;
}

void ssd1306Start(SSD1306Driver *devp, const SSD1306Config *config) {
  const uint8_t cmds[] = {
    0xAE,	// display off
    0x20,	// Set memory address
    0x10,	// 0x00: horizontal addressing mode, 0x01: vertical addressing mode
    // 0x10: Page addressing mode(RESET), 0x11: invalid
    0xB0,	// Set page start address for page addressing mode: 0 ~ 7
    0xC8,	// Set COM output scan direction
    0x00,	// Set low column address
    0x10,	// Set height column address
    0x40,	// Set start line address
    0x81,	// Set contrast control register
    0xFF,
    0xA1,	// Set segment re-map 0 to 127
    0xA6,	// Set normal display
    0xA8,	// Set multiplex ratio(1 to 64)
    0x3F,
    0xA4,	// 0xa4: ouput follows RAM content, 0xa5: ouput ignores RAM content
    0xD3,	// Set display offset
    0x00,	// Not offset
    0xD5,	// Set display clock divide ratio/oscillator frequency
    0xF0,	// Set divide ration
    0xD9,	// Set pre-charge period
    0x22,
    0xDA,	// Set COM pins hardware configuration
    0x12,
    0xDB,	// Set VCOMH
    0x20,	// 0x20: 0.77*Vcc
    0x8D,	// Set DC-DC enable
    0x14,
    0xAF,	// turn on SSD1306panel
  };
  uint8_t idx;

  chDbgCheck((devp != NULL) && (config != NULL));

  chDbgAssert((devp->state == SSD1306_STOP) || (devp->state == SSD1306_READY),
              "ssd1306Start(), invalid state");

  devp->config = config;

  // A little delay
  chThdSleepMilliseconds(100);

  // OLED initialize
  for (idx = 0; idx < sizeof(cmds) / sizeof(cmds[0]); idx++) {
    wrCmd(devp, cmds[idx]);
  }

  // Clear screen
  fillScreen(devp, SSD1306_COLOR_WHITE);

  // Update screen
  updateScreen(devp);

  // Set default value
  devp->x = 0;
  devp->y = 0;

  devp->state = SSD1306_READY;
}

void ssd1306Stop(SSD1306Driver *devp) {
  chDbgAssert((devp->state == SSD1306_STOP) || (devp->state == SSD1306_READY),
              "ssd1306Stop(), invalid state");

  if (devp->state == SSD1306_READY) {
    // Turn off display
    setDisplay(devp, 0);
  }

  devp->state = SSD1306_STOP;
}

static const I2CConfig i2ccfg = {
#if defined(STM32F769xx) || defined(STM32F746xx) || defined(STM32F767xx)
//  .timingr=STM32_TIMINGR_PRESC(15U) | STM32_TIMINGR_SCLDEL(4) |
//           STM32_TIMINGR_SDADEL(2U) | STM32_TIMINGR_SCLH(15U) | STM32_TIMINGR_SCLL(21U),
  .timingr=0x00501E63,//0x6000030D,
  .cr1=0,
  .cr2=0
#else
  .op_mode=OPMODE_I2C,
  .clock_speed=100000,
  .duty_cycle=STD_DUTY_CYCLE
#endif
};

static const SSD1306Config ssd1306cfg = {
  .i2cp=&I2CD_LCD,
  .i2ccfg=&i2ccfg,
  .sad=SSD1306_SAD_0X78
};

//PLACE_IN_RAM_SECTION(.nocache.SSD1306D1)
static SSD1306Driver SSD1306D1;

void ssd130InitAndConfig(char *appName){


  //look at the schematic, these pins are already externally pulled up, see p. 41 in the user manual
  palSetLineMode(SSD1306_I2C_SCL_LINE,   SSD1306_I2C_LINE_MODE );//PB8-->I2C1_SCL
  palSetLineMode(SSD1306_I2C_SDA_LINE,   SSD1306_I2C_LINE_MODE);//PB9-->I2C1_SDA

  //To control LCD, see Arduino driver for the Accessory Shield
  palSetLineMode(SSD1306_SA0_LINE,   SSD1306_LINE_MODE );//PJ4-->SA0
  palSetLineMode(SSD1306_RESET_LINE, SSD1306_LINE_MODE );//Reset
  i2cStop(&I2CD_LCD);
  i2cStart(&I2CD_LCD, &i2ccfg);
  //Pay attention to the way these pins are set and cleared; without them the LCD wouldn't work
  palClearLine(SSD1306_SA0_LINE);//SA0
  palSetLine(SSD1306_RESET_LINE);
  chThdSleepMilliseconds(2);
  palClearLine(SSD1306_RESET_LINE);
  chThdSleepMilliseconds(10);
  palSetLine(SSD1306_RESET_LINE);


  ssd1306ObjectInit(&SSD1306D1);
  ssd1306Start(&SSD1306D1, &ssd1306cfg);

  ssd1306FillScreen(&SSD1306D1, 0x00);
  SSD1306D1.y = 0;
  SSD1306D1.x = 0;
  ssd1306Puts(&SSD1306D1, appName, &ssd1306_font_7x10, SSD1306_COLOR_WHITE);
  ssd1306UpdateScreen(&SSD1306D1);

}
void LCD_Display_Update(void){
  ssd1306UpdateScreen(&SSD1306D1);
}
void LCD_Display_String(char *buffer, uint8_t y, bool updateScreen){
//  ssd1306FillRowScreen(&SSD1306D1,SSD1306_COLOR_BLACK,3);
//  ssd1306FillRowScreen(&SSD1306D1,SSD1306_COLOR_BLACK,4);
  if ( buffer != NULL ){
	SSD1306D1.y = y;
	SSD1306D1.x = 0;
	ssd1306Puts(&SSD1306D1, buffer, &ssd1306_font_7x10, SSD1306_COLOR_WHITE);
  }
  if ( updateScreen )
	  ssd1306UpdateScreen(&SSD1306D1);
}
void fillScreenRow(ssd1306_color_t color, uint8_t row){
	ssd1306FillRowScreen(&SSD1306D1,color,row);
}
void LCD_Display_3Strings(char *buffer1, char *buffer2,char *buffer3,uint8_t y1, uint8_t y2, uint8_t y3){
  ssd1306FillRowScreen(&SSD1306D1,SSD1306_COLOR_BLACK,3);
  ssd1306FillRowScreen(&SSD1306D1,SSD1306_COLOR_BLACK,4);
  if ( buffer1 != NULL ){
	SSD1306D1.y = y1;
	SSD1306D1.x = 0;
	ssd1306Puts(&SSD1306D1, buffer1, &ssd1306_font_7x10, SSD1306_COLOR_WHITE);
  }
  if ( buffer2 != NULL ){
	SSD1306D1.y = y2;
	SSD1306D1.x = 0;

	ssd1306Puts(&SSD1306D1, buffer2, &ssd1306_font_7x10, SSD1306_COLOR_WHITE);
  }
  if ( buffer3 != NULL ){
    SSD1306D1.y = y3;
    SSD1306D1.x = 0;
  	ssd1306Puts(&SSD1306D1, buffer3, &ssd1306_font_7x10, SSD1306_COLOR_WHITE);
  }
  ssd1306UpdateScreen(&SSD1306D1);
}
#endif
