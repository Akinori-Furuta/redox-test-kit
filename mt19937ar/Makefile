# SPDX-License-Identifier: BSD-2-Clause
# Make mt19937.o object file
#
#  Copyright 2012, 2017, 2025 Akinori Furuta<afuruta@m7.dion.ne.jp>.
#  All rights reserved.
#

# Make mt19937ar object binary:
# $ make
#
# Clean:
# $ make clean
#
# Test MT19937 (Mersenne Twister) lib:
# $ make mtTest
#

include ../Makefile.inc

which_awk:=$(shell which awk 2>/dev/null)

UseNative=$(shell uname -m | grep -q -e x86_64 -e 'i[3456]86' -e athlon ; echo $$? )
ifeq ($(UseNative), 0)
	CFLAGS+= -march=native -mtune=native
endif

MTTEST_TEMP=../tmp/mtTestOutUnderTest.txt

ifneq ($(which_awk),)
	Test2SComp=$(shell echo -n '        ' | tr ' ' '\377' | od -t d4 | grep -e '[-]1' | awk '{print $$2}' )
endif

ifeq ($(Test2SComp), -1)
	CFLAGS_CONFIG_2SCOMP?=-DCONFIG_2SCOMP
endif

CFLAGS+=$(CFLAGS_CONFIG_2SCOMP)

all: mt19937ar.o

mtTest: mtTest.o mt19937ar.o
	$(CC) $(CFLAGS) -o $@ $^
	./mtTest > $(MTTEST_TEMP)
	( diff $(MTTEST_TEMP) mt19937ar.out && echo "PASS: Match to pre calculated output." ) || echo "FAIL: Incorrect output."

mt19937ar.o: mt19937ar.c mt19937ar.h
	$(CC) -c $(CFLAGS) -o $@ $<

mtTest.o: mtTest.c mt19937ar.h
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -rf mt19937ar.o mtTest $(MTTEST_TEMP)
