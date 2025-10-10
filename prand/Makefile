# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright (C) 2025 afuruta@m7.dion.ne.jp

CC ?= gcc
CFLAGS ?= -Wall -O2

all: prand

prand: prand.c ../mt19937ar/mt19937ar.o
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) -o $@ ../mt19937ar/mt19937ar.o $<

../mt19937ar/mt19937ar.o: ../mt19937ar/mt19937ar.c ../mt19937ar/mt19937ar.h
	make -C ../mt19937ar

clean:
	rm prand
