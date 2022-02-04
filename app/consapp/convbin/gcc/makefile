#
# makefile for sbp2rinex
#

SHELL := /bin/bash#
ifeq ($(OS),Windows_NT)
    UNAME := Windows
else
ifneq (,$(findstring WINDOWS,$(PATH)))
    UNAME := Windows
else
    UNAME := $(shell uname -s)
endif
endif

BINDIR = /usr/local/bin

SRC    = ../../../../src

INCLUDE= -I$(SRC)
OPTIONS= -DTRACE -DENAGLO -DENAQZS -DENAGAL -DENACMP -DENAIRN -DNFREQ=5 -DNEXOBS=3

CC=gcc
CFLAGS=-O2 -ansi -std=c99 -Wall -Wno-unused-function -Wno-unused-but-set-variable $(INCLUDE) $(OPTIONS)
DATDIR=../../../../test/data/rcvraw
SEP="/"

ifeq ($(UNAME),Windows)
OPTIONS += -DWIN32
DATDIR=..\\..\\..\\..\\test\\data\\rcvraw
SEP="\\"
endif

LDLIBS= -lm
ifeq ($(UNAME),Linux)
LDLIBS+=
endif
ifeq ($(UNAME),Windows)
LDLIBS+= -lwinmm
endif

all  : sbp2rinex

sbp2rinex  : convbin.o rtkcmn.o rinex.o sbas.o preceph.o rcvraw.o convrnx.o
sbp2rinex  : rtcm.o rtcm2.o rtcm3.o rtcm3e.o pntpos.o ephemeris.o ionex.o
sbp2rinex  : novatel.o swiftnav.o ublox.o skytraq.o javad.o nvs.o
sbp2rinex  : binex.o rt17.o septentrio.o

sbp2rinex  :
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

convbin.o  : ../convbin.c
	$(CC) -c $(CFLAGS) ../convbin.c
rtkcmn.o   : $(SRC)/rtkcmn.c
	$(CC) -c $(CFLAGS) $(SRC)/rtkcmn.c
rinex.o    : $(SRC)/rinex.c
	$(CC) -c $(CFLAGS) $(SRC)/rinex.c
sbas.o     : $(SRC)/sbas.c
	$(CC) -c $(CFLAGS) $(SRC)/sbas.c
preceph.o  : $(SRC)/preceph.c
	$(CC) -c $(CFLAGS) $(SRC)/preceph.c
rcvraw.o   : $(SRC)/rcvraw.c
	$(CC) -c $(CFLAGS) $(SRC)/rcvraw.c
convrnx.o  : $(SRC)/convrnx.c
	$(CC) -c $(CFLAGS) $(SRC)/convrnx.c
rtcm.o     : $(SRC)/rtcm.c
	$(CC) -c $(CFLAGS) $(SRC)/rtcm.c
rtcm2.o    : $(SRC)/rtcm2.c
	$(CC) -c $(CFLAGS) $(SRC)/rtcm2.c
rtcm3.o    : $(SRC)/rtcm3.c
	$(CC) -c $(CFLAGS) $(SRC)/rtcm3.c
rtcm3e.o   : $(SRC)/rtcm3e.c
	$(CC) -c $(CFLAGS) $(SRC)/rtcm3e.c
pntpos.o   : $(SRC)/pntpos.c
	$(CC) -c $(CFLAGS) $(SRC)/pntpos.c
ionex.o    : $(SRC)/ionex.c
	$(CC) -c $(CFLAGS) $(SRC)/ionex.c
ephemeris.o: $(SRC)/ephemeris.c
	$(CC) -c $(CFLAGS) $(SRC)/ephemeris.c
novatel.o  : $(SRC)/rcv/novatel.c
	$(CC) -c $(CFLAGS) $(SRC)/rcv/novatel.c
swiftnav.o : $(SRC)/rcv/swiftnav.c
	$(CC) -c $(CFLAGS) $(SRC)/rcv/swiftnav.c
ublox.o    : $(SRC)/rcv/ublox.c
	$(CC) -c $(CFLAGS) $(SRC)/rcv/ublox.c
skytraq.o  : $(SRC)/rcv/skytraq.c
	$(CC) -c $(CFLAGS) $(SRC)/rcv/skytraq.c
javad.o    : $(SRC)/rcv/javad.c
	$(CC) -c $(CFLAGS) $(SRC)/rcv/javad.c
nvs.o      : $(SRC)/rcv/nvs.c
	$(CC) -c $(CFLAGS) $(SRC)/rcv/nvs.c
binex.o    : $(SRC)/rcv/binex.c
	$(CC) -c $(CFLAGS) $(SRC)/rcv/binex.c
rt17.o     : $(SRC)/rcv/rt17.c
	$(CC) -c $(CFLAGS) $(SRC)/rcv/rt17.c
septentrio.o: $(SRC)/rcv/septentrio.c
	$(CC) -c $(CFLAGS) $(SRC)/rcv/septentrio.c

convbin.o  : $(SRC)/rtklib.h makefile
rtkcmn.o   : $(SRC)/rtklib.h makefile
rinex.o    : $(SRC)/rtklib.h makefile
sbas.o     : $(SRC)/rtklib.h makefile
preceph.o  : $(SRC)/rtklib.h makefile
rcvraw.o   : $(SRC)/rtklib.h makefile
convrnx.o  : $(SRC)/rtklib.h makefile
rtcm.o     : $(SRC)/rtklib.h makefile
rtcm2.o    : $(SRC)/rtklib.h makefile
rtcm3.o    : $(SRC)/rtklib.h makefile
rtcm3e.o   : $(SRC)/rtklib.h makefile
pntpos.o   : $(SRC)/rtklib.h makefile
ephemeris.o: $(SRC)/rtklib.h makefile
ionex.o    : $(SRC)/rtklib.h makefile
novatel.o  : $(SRC)/rtklib.h makefile
swiftnav.o : $(SRC)/rtklib.h makefile
ublox.o    : $(SRC)/rtklib.h makefile
skytraq.o  : $(SRC)/rtklib.h makefile
gw10.o     : $(SRC)/rtklib.h makefile
javad.o    : $(SRC)/rtklib.h makefile
nvs.o      : $(SRC)/rtklib.h makefile
binex.o    : $(SRC)/rtklib.h makefile
rt17.o     : $(SRC)/rtklib.h makefile
septentrio.o: $(SRC)/rtklib.h makefile
cmr.o      : $(SRC)/rtklib.h makefile
tersus.o   : $(SRC)/rtklib.h makefile

install:
	cp sbp2rinex $(BINDIR)

clean:
	rm -f sbp2rinex sbp2rinex.exe *.o *.obs *.nav *.gnav *.hnav *.qnav *.sbs *.stackdump

test : test1 test2 test3 test4 test5 test6 testclean

test1: # binary sbp
	@echo -e "\n\n****** Test1: sbp binary ********\n"
	./sbp2rinex $(DATDIR)$(SEP)2017-05-12-v1.1.26.sbp -o 2017-05-12-v1.1.26-bin.obs
test2: # sbp json with spaces
	@echo -e "\n\n****** Test2: sbp json ********\n"
	./sbp2rinex $(DATDIR)$(SEP)2017-05-12-v1.1.26.sbp.json -o 2017-05-12-v1.1.26-json.obs -n 2017-05-12-v1.1.26-json.nav
test3: test1 test2 # ensure outputs from test1 and test2 are bit exact minus first two lines
	@echo -e "\n\n****** Test3: compare json and binary output ********\n"
	tail -n +5 ./2017-05-12-v1.1.26-json.obs > 2017-05-12-v1.1.26-json.trunc.obs
	tail -n +5 ./2017-05-12-v1.1.26-bin.obs > 2017-05-12-v1.1.26-bin.trunc.obs
	diff -q 2017-05-12-v1.1.26-json.trunc.obs 2017-05-12-v1.1.26-bin.trunc.obs
test4:
	@echo -e "\n\n***** Test: json without spaces********\n"
	./sbp2rinex $(DATDIR)$(SEP)2017-05-12-v1.1.26-nospaces.sbp.json -o 2017-05-12-v1.1.26-json-nospaces.obs -n 2017-05-12-v1.1.26-json.nav
test5: test1 test4  # make sure JSON with no spaces are bit exact minus first two lines
	tail -n +5 ./2017-05-12-v1.1.26-json-nospaces.obs > 2017-05-12-v1.1.26-json-nospaces.trunc.obs
	diff 2017-05-12-v1.1.26-json-nospaces.trunc.obs 2017-05-12-v1.1.26-bin.trunc.obs
test6: # BDS + GAL + GLO + GPS
	@echo -e "\n\n****** Test6: BDS + GAL + GLO + GPS ********\n"
	./sbp2rinex $(DATDIR)$(SEP)2018-06-15-v1.4.0-develop-2018061321-3-g734bd4b9.sbp.json -o 2018-06-15-json.obs -n 2018-06-15-json.nav
	tail -n +4 ./2018-06-15-json.obs > 2018-06-15-json.trunc.obs
	diff --strip-trailing-cr ./2018-06-15-json.trunc.obs $(shell pwd)$(SEP)$(DATDIR)$(SEP)2018-06-15-json.obs
	tail -n +4 ./2018-06-15-json.nav > 2018-06-15-json.trunc.nav
	diff --strip-trailing-cr ./2018-06-15-json.trunc.nav $(shell pwd)$(SEP)$(DATDIR)$(SEP)2018-06-15-json.nav
testclean:
	rm -f *.o *.obs *.nav *.gnav *.hnav *.qnav *.sbs
