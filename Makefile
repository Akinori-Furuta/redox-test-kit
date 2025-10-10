# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright (C) 2025 afuruta@m7.dion.ne.jp

CC ?= gcc
CFLAGS ?= -Wall -O2

all: ./tmp prand/prand

mtTest: ./tmp
	make -C mt19937ar mtTest

./tmp:
	mkdir -p ./tmp

prand/prand:
	make -C prand

clean:
	rm -rf ./tmp
	make -C prand clean
	make -C mt19937ar clean
