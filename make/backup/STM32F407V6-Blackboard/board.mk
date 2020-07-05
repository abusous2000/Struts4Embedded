# List of all the board related files.
BOARDSRC = $(CHIBIOS)/os/hal/boards/STM32F407V6-Blackboard/board.c

# Required include directories
BOARDINC = $(CHIBIOS)/os/hal/boards/STM32F407V6-Blackboard

# Shared variables
ALLCSRC += $(BOARDSRC)
ALLINC  += $(BOARDINC)
