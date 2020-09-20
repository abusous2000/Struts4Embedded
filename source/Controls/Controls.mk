ALLINC  += $(CONTROLS)
ALLINC  += $(CONTROLS)/LCD
ALLINC  += $(CONTROLS)/RGB
ALLINC  += $(CONTROLS)/PWM
ALLINC  += $(CONTROLS)/ButtonsLEDs
ALLINC  += $(CONTROLS)/PPMFrameDecoder
ALLINC  += $(CONTROLS)/PotReader
ALLINC  += $(CONTROLS)/eByteLora
ALLINC  += $(CONTROLS)/IRReceiver

ALLCSRC += $(CONTROLS)/LCD/ssd1306_7x10_font.c \
           $(CONTROLS)/LCD/ssd1306.c \
           $(CONTROLS)/PWM/PWM.c \
           $(CONTROLS)/RGB/P9813_RGB_Driver.c \
           $(CONTROLS)/PPMFrameDecoder/PPMFrameDecoder.c \
           $(CONTROLS)/eByteLora/EByteLora.c \
           $(CONTROLS)/IRReceiver/IRReceiver.c \
           $(CONTROLS)/ButtonsLEDs/ButtonLEDs.c \
           $(CONTROLS)/PotReader/PotReader.c