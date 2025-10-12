# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright (C) 2025 afuruta@m7.dion.ne.jp

CC ?= gcc
CFLAGS ?= -Wall -O2

MT19937AR_FILES = $(wildcard  ../$(MT19937AR)/*.c ../$(MT19937AR)/*.h)
GETOPT_FILES = $(wildcard  ../$(GETOPT)/*.c ../$(GETOPT)/*.h)

all: prand

prand: prand.c ../$(MT19937AR)/mt19937ar.o ../$(GETOPT)/getopt-alt.o
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) -o $@ \
		../$(MT19937AR)/mt19937ar.o ../$(GETOPT)/getopt-alt.o \
		$<

../$(MT19937AR)/mt19937ar.o: $(MT19937AR_FILES)
	make -C ../$(MT19937AR)

../$(GETOPT)/getopt-alt.o: $(GETOPT_FILES)
	make -C ../$(GETOPT)

clean:
	rm prand
