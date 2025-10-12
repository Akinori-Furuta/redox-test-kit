# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright (C) 2025 afuruta@m7.dion.ne.jp

CC ?= gcc
CFLAGS ?= -Wall -O2

export MT19937AR = mt19937ar
export GETOPT = getopt

SUBDIRS = prand $(GETOPT) $(MT19937AR)

.PHONY: all clean subdirs tmp mtTest $(SUBDIRS)

all: subdirs

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

mtTest: tmp $(MT19937AR)/*
	make -C $(MT19937AR) mtTest

tmp:
	mkdir -p tmp

prand: $(MT19937AR) $(GETOPT)

clean:
	rm -rf ./tmp
	make -C prand clean
	make -C $(MT19937AR) clean
	make -C $(GETOPT) clean
