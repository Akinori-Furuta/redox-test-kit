# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright (C) 2025 afuruta@m7.dion.ne.jp

include ../Makefile.inc

MT19937AR_FILES = $(wildcard  ../$(MT19937AR)/*.c ../$(MT19937AR)/*.h)
MT19937AR_OBJECT_FILES = $(foreach f, $(MT19937AR_OBJS), ../$(MT19937AR)/$(f))

GETOPT_FILES = $(wildcard  ../$(GETOPT)/*.c ../$(GETOPT)/*.h)
GETOPT_OBJECT_FILES = $(foreach f, $(GETOPT_OBJS), ../$(GETOPT)/$(f))

all: prand

prand: prand.c $(MT19937AR_OBJECT_FILES) $(GETOPT_OBJECT_FILES)
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) -o $@ \
		$(MT19937AR_OBJECT_FILES) $(GETOPT_OBJECT_FILES) \
		$<

$(MT19937AR_OBJECT_FILES): $(MT19937AR_FILES)
	make -C ../$(MT19937AR)

$(GETOPT_OBJECT_FILES): $(GETOPT_FILES)
	make -C ../$(GETOPT)

clean:
	rm prand
