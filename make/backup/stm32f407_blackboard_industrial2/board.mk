# List of all the board related files.
BOARDSRC = $(CHIBIOS)/os/hal/boards/$(BOARD_NAME)/board.c

# Required include directories
BOARDINC = $(CHIBIOS)/os/hal/boards/$(BOARD_NAME)

# Shared variables
ALLCSRC += $(BOARDSRC)
ALLINC  += $(BOARDINC)
