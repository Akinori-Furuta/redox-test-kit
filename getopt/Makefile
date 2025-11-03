# SPDX-License-Identifier: BSD-2-Clause
# Make getopt-alt.o object file
#
#  Copyright 2025 Akinori Furuta<afuruta@m7.dion.ne.jp>.
#  All rights reserved.

# Make getopt-alt.o object binary:
# $ make
#
# Clean:
# $ make clean
#
# Make executable to test getopt-alt lib:
# $ make test
#

include ../Makefile.inc

.PHONY: all test

all: getopt-alt.o

test: getopt-alt-test

getopt-alt-test: getopt-alt-test.o getopt-alt.o
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) -o $@ $^

getopt-alt.o: getopt-alt.c getopt-alt.h
	$(CC) -c $(CFLAGS) $(CFLAGS_EXTRA) -o $@ $<

getopt-alt-test.o: getopt-alt-test.c getopt-alt.c getopt-alt.h
	$(CC) -c $(CFLAGS) $(CFLAGS_EXTRA) -o $@ $<

clean:
	rm -rf getopt-alt.o getopt-alt-test.o getopt-alt-test
