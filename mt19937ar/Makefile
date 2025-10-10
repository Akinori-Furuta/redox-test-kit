# Make mt19937.o object file
#
#  Copyright 2012, 2017, 2025 Akinori Furuta<afuruta@m7.dion.ne.jp>.
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

# Make mt19937ar object binary:
# $ make
#
# Clean:
# $ make clean
#
# Test MT19937 (Mersenne Twister) lib:
# $ make mtTest
#

CFLAGS?=-O3 -Wall -Wunused -Wuninitialized

UseNative=$(shell uname -m | grep -q -e x86_64 -e 'i[3456]86' -e athlon ; echo $$? )
ifeq ($(UseNative), 0)
	CFLAGS+= -march=native -mtune=native
endif

MTTEST_TEMP=../tmp/mtTestOutUnderTest.txt

Test2SComp=$(shell echo -n '        ' | tr ' ' '\377' | od -t d4 | grep -e '[-]1' | awk '{print $$2}' )

ifeq ($(Test2SComp), -1)
	CFLAGS_CONFIG_2SCOMP?=-DCONFIG_2SCOMP
endif

CFLAGS+=$(CFLAGS_CONFIG_2SCOMP)

all: mt19937ar.o

mtTest: mtTest.o mt19937ar.o
	$(CC) $(CFLAGS) -o $@ $^
	./mtTest > $(MTTEST_TEMP)
	diff $(MTTEST_TEMP) mt19937ar.out && echo "Match to pre calculated output."

mt19937ar.o: mt19937ar.c mt19937ar.h
	$(CC) -c $(CFLAGS) -o $@ $<

mtTest.o: mtTest.c mt19937ar.h
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -rf mt19937ar.o mtTest $(MTTEST_TEMP)
