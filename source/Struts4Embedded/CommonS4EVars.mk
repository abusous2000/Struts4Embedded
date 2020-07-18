ifeq ($(STRUTS4EMBEDDED),)
	STRUTS4EMBEDDED :=$(CHIBIOS)/demos/STM32/Struts4Embedded/source/Struts4Embedded
endif
CHIBIOS_CONTRIB  :=$(CHIBIOS)/ChibiOS-Contrib
GFXLIB   := $(CHIBIOS)/ext/ugfx-2.7
USBCFG   := $(STRUTS4EMBEDDED)/../../usbcfg
MQTTCLIENT := $(STRUTS4EMBEDDED)/../MQTTClient
CONTROLS = $(STRUTS4EMBEDDED)/../Controls
CONFDIR  := ./cfg/$(BOARD_NAME)
BUILDDIR := ./build/$(BOARD_NAME)
DEPDIR   := ./.dep/$(BOARD_NAME)

ALLINC  += $(GFXINC)