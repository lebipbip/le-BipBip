# -*- Makefile -*- variables global configuration

# !!!!!!!!!!!!!!!!!!!!!!
# DON'T MODIFY THIS FILE
# !!!!!!!!!!!!!!!!!!!!!!
# This is a sample showing Makefile variables that are used within the
# stereocam repository.
# Please use it as a template, and save lines concerning variables to modify in:
# local.mk for the default configuration ("make" command)
# local.mk followed by local-debug.mk for the debug configuration ("make CONFIG=debug" command)
# local.mk followed by local-release.mk for the release configuration ("make CONFIG=release" command)

AS	=	$(CROSS)as
AR	=	$(CROSS)ar
CC	=	$(CROSS)gcc
CXX	=	$(CROSS)g++
CPP	=	$(CROSS)gcc -E
# linker: please redefine it in your own Makefile after including this file 
# if you need to use gcc or ld instead of g++
LD	=	$(CROSS)gcc
NM	=	$(CROSS)nm
OBJCOPY	=	$(CROSS)objcopy
OBJDUMP	=	$(CROSS)objdump
RANLIB	=	$(CROSS)ranlib
READELF	=	$(CROSS)readelf
SIZE	=	$(CROSS)size
STRING	=	$(CROSS)string
STRIP	=	$(CROSS)strip

LEX	=	flex
YACC	=	bison
YFLAGS	=	-d

# CFLAGS_OPT is normally in the config$(CONFIG).mk file,
# but here's a reasonable default value
# add -ftree-vectorizer-verbose=3 to get information about vectorized loops
CFLAGS_OPT= -Wall  -Wmissing-declarations
#CFLAGS_OPT= -Wall -g -O2

CROSS_INCLUDES = /usr/msp430/include/
