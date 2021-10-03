1-add this to your make fil1
include $(CHIBIOS)/os/various/fatfs_bindings/fatfs.mk
2-make macro HAL_USE_SDC to TRUE in you halconf.h
3-add to mcuconf.h
#define STM32_SDC_USE_SDMMC1                1