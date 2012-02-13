# This is a -*- Makefile -*- for the upc_main executable
# Warning: this file contains tabs which cannot be converted to spaces

default:
	@echo 'This binary is meant to be cross-compiled.'
	@echo 'please execute "make" from one of the subdirectories corresponding to your target (e.g. Debug_mp430)'


include $(SRCDIR)/config/default.mk
-include $(SRCDIR)/config/local.mk
-include $(SRCDIR)/config/local-$(CONFIG).mk
-include $(SRCDIR)/config/local-$(TARGET).mk


VERSION_PATH = $(CONFIG)_$(TARGET)


#CC = $(CROSS)gcc
#CXX = $(CROSS)g++
#LD = $(CROSS)g++
#AR = $(CROSS)ar
#RANLIB = $(CROSS)ranlib
#CFLAGS_OPT = -Wall -O2 -g

# Create dependencies
MAKEDEPEND = gcc -M $(CPPFLAGS) -o $*.d $<

CXXFLAGS=$(CFLAGS_OPT)
CFLAGS=$(CFLAGS_OPT)

CPPFLAGS =  -I$(SRCDIR) $(CPPFLAGS_ADD) -I$(CROSS_INCLUDES)

BINARY = $(SRCDIR)/$(VERSION_PATH)/lebipbip

VPATH = $(SRCDIR)

all: $(BINARY)

bin_SOURCES_C = \
	I2C.c \
	TI_USCI_I2C_master.c \
	bmp085.c \
	booster.c \
	button.c \
	buzzer.c \
	filter.c \
	lebipbip.c \
	ms5607.c \
	oscillator.c \
	power.c \
	sink.c \
	timer.c \
	uart.c \
	vario.c 


bin_OBJS = $(bin_SOURCES_C:.c=.o)


$(BINARY): $(bin_OBJS)
	$(LD) -o $@ $^ $(LDFLAGS) $(LDADD)

.SUFFIXES: .c .o 

## gcc-only version:
%.o : %.c
	$(COMPILE.c) -MD -o $@ $<
	@cp $*.d $*.P; \
	    sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	        -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	    rm -f $*.d

clean:
	-rm -f $(BINARY) *.o  *~

-include $(SRCS_C:.c=.P)
