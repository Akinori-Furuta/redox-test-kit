# Common variables and rules

export CC ?= gcc
export CFLAGS ?= -O3 -Wall -Wunused -Wuninitialized

export PRAND = prand

# Pseudo Random Generator library defines
export MT19937AR = mt19937ar
export MT19937AR_DEPS = mt19937ar.c mt19937ar.h
export MT19937AR_OBJS = mt19937ar.o

# Alternate getopt library defines
export GETOPT = getopt
export GETOPT_DEPS = getopt-alt.c getopt-alt.h
export GETOPT_OBJS = getopt-alt.o

export MASHLF = mashlf
export SUBDIRS = $(GETOPT) $(MT19937AR) $(PRAND) $(MASHLF)
