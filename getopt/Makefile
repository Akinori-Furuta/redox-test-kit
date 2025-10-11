# Make getopt-alt.o object file
#
#  Copyright 2025 Akinori Furuta<afuruta@m7.dion.ne.jp>.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#
#  1. Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
#
#  2. Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
#  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
#  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Make getopt-alt.o object binary:
# $ make
#
# Clean:
# $ make clean
#
# Test getopt-alt lib:
# $ make test
#

CC?=gcc
CFLAGS?=-O3 -Wall -Wunused -Wuninitialized

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
