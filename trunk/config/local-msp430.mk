CROSS=/usr/bin/msp430-
CROSS_INCLUDES = /usr/msp430/include/
CFLAGS = -mmcu=msp430g2553 -c
LDFLAGS = -mmcu=msp430g2553 -L/usr/msp430/lib/ -lm

