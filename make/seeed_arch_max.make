##############################################################################
# Build global options
# NOTE: Can be overridden externally.
#

# Compiler options here.
ifeq ($(USE_OPT),)
  USE_OPT = -O0 -ggdb -fomit-frame-pointer -falign-functions=16 -Wno-unused-variable -Wunused-function -DFPM_ARM --specs=nosys.specs -lm
endif

# C specific options here (added to USE_OPT).
ifeq ($(USE_COPT),)
  USE_COPT = -DWOLFSSL_USER_SETTINGS -DLWIP_DEBUG
endif

# C++ specific options here (added to USE_OPT).
ifeq ($(USE_CPPOPT),)
  USE_CPPOPT = -fno-rtti
endif

# Enable this if you want the linker to remove unused code and data.
ifeq ($(USE_LINK_GC),)
  USE_LINK_GC = yes
endif

# Linker extra options here.
ifeq ($(USE_LDOPT),)
  USE_LDOPT = 
endif

# Enable this if you want link time optimizations (LTO).
ifeq ($(USE_LTO),)
  USE_LTO = yes
endif

# Enable this if you want to see the full log while compiling.
ifeq ($(USE_VERBOSE_COMPILE),)
  USE_VERBOSE_COMPILE = no
endif

# If enabled, this option makes the build process faster by not compiling
# modules not used in the current configuration.
ifeq ($(USE_SMART_BUILD),)
  USE_SMART_BUILD = yes
endif

#
# Build global options
##############################################################################

##############################################################################
# Architecture or project specific options
#

# Stack size to be allocated to the Cortex-M process stack. This stack is
# the stack used by the main() thread.
ifeq ($(USE_PROCESS_STACKSIZE),)
  USE_PROCESS_STACKSIZE = 0x1000
endif

# Stack size to the allocated to the Cortex-M main/exceptions stack. This
# stack is used for processing interrupts and exceptions.
ifeq ($(USE_EXCEPTIONS_STACKSIZE),)
  USE_EXCEPTIONS_STACKSIZE = 0x400
endif

# Enables the use of FPU (no, softfp, hard).
ifeq ($(USE_FPU),)
  USE_FPU = no
endif

# FPU-related options.
ifeq ($(USE_FPU_OPT),)
  USE_FPU_OPT = -mfloat-abi=$(USE_FPU) -mfpu=fpv4-sp-d16
endif

#
# Architecture or project specific options
##############################################################################

##############################################################################
# Project, target, sources and paths
#

# Define project name here
PROJECT = ch

# Target settings.
MCU  = cortex-m4

# Imported source files and paths.
CHIBIOS  := ../../..
BOARD_NAME := seeed_arch_max
STRUTS4EMBEDDED :=$(CHIBIOS)/demos/STM32/Struts4Embedded/source/Struts4Embedded
include $(STRUTS4EMBEDDED)/CommonS4EVars.mk

INCLUDE_SEGGER_JLINK := "no"
INCLUDE_SEGGER_JLINK_VALUE :=0
USE_MAC := "yes"
USE_AE_SHELL := "yes"
USE_AE_SHELL_VALUE := 0
USE_FATFS := "yes"


# Licensing files.
include $(CHIBIOS)/os/license/license.mk
# Startup files.
include $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk/startup_stm32f4xx.mk
# HAL-OSAL files (optional).
include $(CHIBIOS)/os/hal/hal.mk
include $(CHIBIOS)/os/hal/ports/STM32/STM32F4xx/platform.mk
include $(CHIBIOS)/os/hal/boards/SEEED_ARCH_MAX/board.mk
include $(CHIBIOS)/os/hal/osal/rt-nil/osal.mk
# RTOS files (optional).
include $(CHIBIOS)/os/rt/rt.mk
include $(CHIBIOS)/os/common/ports/ARMv7-M/compilers/GCC/mk/port.mk
# Auto-build files in ./source recursively.
include $(CHIBIOS)/tools/mk/autobuild.mk
# Other files (optional).
ifeq ($(USE_AE_SHELL),"yes")
include $(CHIBIOS)/test/lib/test.mk
include $(CHIBIOS)/test/rt/rt_test.mk
include $(CHIBIOS)/test/oslib/oslib_test.mk
include $(CHIBIOS)/os/various/shell/shell.mk
USE_AE_SHELL_VALUE := 1
endif
include $(CHIBIOS)/os/hal/lib/streams/streams.mk
include $(CHIBIOS_CONTRIB)/os/common/ports/ARMCMx/compilers/GCC/utils/fault_handlers_v7m.mk
ifeq ($(USE_FATFS),"yes")
include $(CHIBIOS)/os/various/fatfs_bindings/fatfs.mk
endif
#STARTUPLD = /os/common/startup/ARMCMx/compilers/GCC/ld
ifeq ($(USE_MAC),"yes")
include $(CHIBIOS)/os/various/lwip_bindings/lwip.mk
endif
ifeq ($(INCLUDE_SEGGER_JLINK),"yes")
include $(CHIBIOS_CONTRIB)/os/various/segger_bindings/segger_rtt.mk
include $(CHIBIOS_CONTRIB)/os/various/segger_bindings/segger_systemview.mk
INCLUDE_SEGGER_JLINK_VALUE := 1
endif
LDSCRIPT= $(STARTUPLD)/STM32F407xE.ld

# C sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CSRC = $(ALLCSRC) \
       $(CHIBIOS)/os/various/syscalls.c \
       $(CHIBIOS)/os/various/evtimer.c \
       main.c

# C++ sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CPPSRC = $(ALLCPPSRC)

# List ASM source files here.
ASMSRC = $(ALLASMSRC)

# List ASM with preprocessor source files here.
ASMXSRC = $(ALLXASMSRC)

# Inclusion directories.
INCDIR = $(CONFDIR) $(ALLINC) $(TESTINC) ./cfg

# Define C warning options here.
CWARN = -Wall -Wextra -Wundef -Wstrict-prototypes

# Define C++ warning options here.
CPPWARN = -Wall -Wextra -Wundef

#
# Project, target, sources and paths
##############################################################################

##############################################################################
# Start of user section
#

# List all user C define here, like -D_DEBUG=1
UDEFS = -DSHELL_CMD_TEST_ENABLED=0  -DWICED_LWIP_DEBUG222 \
        -DDEBUG_TRACE_PRINT=1 -DCHPRINTF_USE_FLOAT=1 -DPORT_ENABLE_GUARD_PAGES=1 \
        -DINCLUDE_SEGGER_JLINK=$(INCLUDE_SEGGER_JLINK_VALUE) -Dboot_t=bool -DSERIAL_BUFFERS_SIZE=512 \
        -DUSE_AE_SHELL=$(USE_AE_SHELL_VALUE)
#UDEFS =-DSHELL_CMD_TEST_ENABLED=0 -DBOARD_PHY_ID=MII_DP83848I_ID

# Define ASM defines here
UADEFS =

# List all user directories here
UINCDIR =

# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS =

#
# End of user section
##############################################################################

##############################################################################
# Common rules
#

RULESPATH = $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk
include $(RULESPATH)/arm-none-eabi.mk
include $(RULESPATH)/rules.mk

#
# Common rules
##############################################################################

##############################################################################
# Custom rules
#

#
# Custom rules
##############################################################################
