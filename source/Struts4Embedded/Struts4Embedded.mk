ALLINC  += $(STRUTS4EMBEDDED)
ALLINC  += $(STRUTS4EMBEDDED)/Utils
ALLINC  += $(STRUTS4EMBEDDED)/MQTT
ALLINC  += $(STRUTS4EMBEDDED)/web
ALLINC  += $(STRUTS4EMBEDDED)/Threads
ALLINC  += $(STRUTS4EMBEDDED)/hal
ALLINC  += $(STRUTS4EMBEDDED)/SDCard

ALLCSRC += $(STRUTS4EMBEDDED)/Threads/WifiCommunicationThd.c \
           $(STRUTS4EMBEDDED)/Threads/BlinkerThd.c \
           $(STRUTS4EMBEDDED)/Threads/ActionEventsThd.c \
           $(STRUTS4EMBEDDED)/SDCard/SDCard.c \
           $(STRUTS4EMBEDDED)/Utils/JsonParser.c \
           $(STRUTS4EMBEDDED)/Utils/hashmap.c \
           $(STRUTS4EMBEDDED)/Utils/emlist.c \
           $(STRUTS4EMBEDDED)/Utils/FaultHandling.c \
           $(STRUTS4EMBEDDED)/Utils/CommonHelper.c \
           $(STRUTS4EMBEDDED)/Utils/RTCHelper.c \
           $(STRUTS4EMBEDDED)/hal/CPal.c \
           $(STRUTS4EMBEDDED)/web/web.c \
           $(STRUTS4EMBEDDED)/MQTT/MqttHelper.c