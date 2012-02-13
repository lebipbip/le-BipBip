CROSS=/usr/bin/msp430-
ifeq ($(CONFIG),release)
  CFLAGS_OPT = -Wall -O3 -DNDEBUG -ffast-math
endif