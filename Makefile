# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright (C) 2025 afuruta@m7.dion.ne.jp

include Makefile.inc

.PHONY: all clean subdirs tmp mtTest $(SUBDIRS)

all: subdirs

subdirs: $(SUBDIRS)

$(filter-out $(PRAND) $(MASHLF),$(SUBDIRS)):
	$(MAKE) -C $@

$(PRAND): $(MT19937AR) $(GETOPT)
	make -C $@

$(MASHLF): $(MT19937AR) $(GETOPT)
	make -C $@

mtTest: tmp $(MT19937AR)/*
	make -C $(MT19937AR) mtTest

tmp:
	mkdir -p tmp

clean: clean-subdirs
	rm -rf ./tmp

clean-subdirs:
	for d in $(SUBDIRS) ; do make -C $$d clean; done
