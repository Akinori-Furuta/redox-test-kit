# Common variables and rules

export CC ?= gcc
export CFLAGS ?= -O3 -Wall -Wunused -Wuninitialized

export PRAND = prand
export MT19937AR = mt19937ar
export GETOPT = getopt
export MASHLF = mashlf
export SUBDIRS = $(GETOPT) $(MT19937AR) $(PRAND) $(MASHLF)
