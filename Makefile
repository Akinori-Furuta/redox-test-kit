# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright (C) 2025 afuruta@m7.dion.ne.jp

CC ?= gcc
CFLAGS ?= -Wall -O2

export MT19937AR = mt19937ar

SUBDIRS = prand $(MT19937AR)

.PHONY: all clean subdirs tmp mtTest $(SUBDIRS)

all: subdirs

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

mtTest: tmp $(MT19937AR)/*
	make -C mt19937ar mtTest

tmp:
	mkdir -p tmp

prand: mt19937ar

clean:
	rm -rf ./tmp
	make -C prand clean
	make -C mt19937ar clean
