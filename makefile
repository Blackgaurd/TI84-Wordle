# ----------------------------
# Makefile Options
# ----------------------------

NAME = WORDLE
ICON = icon.png
DESCRIPTION = "Wordle for TI-84 Plus CE"
COMPRESSED = YES
COMPRESSED_MODE = zx7 # zx0 is so slow
ARCHIVED = NO

CFLAGS = -Wall -Wextra -Oz #-DDEBUG
CXXFLAGS = -Wall -Wextra -Oz #-DDEBUG

# ----------------------------

include $(shell cedev-config --makefile)
