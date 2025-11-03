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

all: $(GETOPT_OBJS)

test: getopt-alt-test

getopt-alt-test: getopt-alt-test.o $(GETOPT_OBJS)
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) -o $@ $^

$(GETOPT_OBJS): $(GETOPT_DEPS)
	$(CC) -c $(CFLAGS) $(CFLAGS_EXTRA) -o $@ $<

getopt-alt-test.o: getopt-alt-test.c $(GETOPT_DEPS)
	$(CC) -c $(CFLAGS) $(CFLAGS_EXTRA) -o $@ $<

clean:
	rm -rf $(GETOPT_OBJS) getopt-alt-test.o getopt-alt-test
