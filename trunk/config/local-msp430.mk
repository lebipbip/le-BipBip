CROSS=/usr/bin/msp430-
CFLAGS = -mmcu=msp430g2553
LDFLAGS = -mmcu=msp430g2553 -L/usr/msp430/lib/ -lm
ifeq ($(CONFIG),release)
  CFLAGS_OPT = -Wall -O3 -DNDEBUG -ffast-math
endif
