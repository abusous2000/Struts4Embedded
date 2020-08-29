ALLINC  += $(CONTROLS)
ALLINC  += $(CONTROLS)/LCD
ALLINC  += $(CONTROLS)/RGB
ALLINC  += $(CONTROLS)/PWM
ALLINC  += $(CONTROLS)/ButtonsLEDs
ALLINC  += $(CONTROLS)/PPMFrameDecoder
ALLINC  += $(CONTROLS)/PotReader

ALLCSRC += $(CONTROLS)/LCD/ssd1306_7x10_font.c \
           $(CONTROLS)/LCD/ssd1306.c \
           $(CONTROLS)/PWM/PWM.c \
           $(CONTROLS)/RGB/P9813_RGB_Driver.c \
           $(CONTROLS)/PPMFrameDecoder/PPMFrameDecoder.c \
           $(CONTROLS)/ButtonsLEDs/ButtonLEDs.c \
           $(CONTROLS)/PotReader/PotReader.c