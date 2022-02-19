# ----------------------------
# Makefile Options
# ----------------------------

NAME = WORDLE
ICON = icon.png
DESCRIPTION = "Wordle for TI-84 Plus CE"
COMPRESSED = NO # YES or NO
ARCHIVED = NO

CFLAGS = -Wall -Wextra -Oz #-DDEBUG
CXXFLAGS = -Wall -Wextra -Oz #-DDEBUG

# ----------------------------

include $(shell cedev-config --makefile)
