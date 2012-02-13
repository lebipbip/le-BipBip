# usually, the difference between debug and release falgs are all contained in the CFLAGS_OPT

#CFLAGS_OPT = -Wall -O3 -DNDEBUG -ftree-vectorize -msse3 -mssse3 -ffast-math
CFLAGS_OPT = -Wall -Wmissing-declarations -g -DDEBUG
