# makefile for str2str
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
SRC    = ../../../src

# for beagleboard
#CTARGET= -mfpu=neon -mfloat-abi=softfp -ffast-math
CC=gcc
CTARGET=

OPTIONS = -DENAGLO -DENAGAL -DENAQZS -DENACMP -DENAIRN -DTRACE -DNFREQ=7 -DNEXOBS=3 -DSVR_REUSEADDR
ifeq ($(UNAME),Windows)
OPTIONS += -DWIN32
endif

CFLAGS = -Wall -O2 -ansi -std=c99 -Wno-unused-but-set-variable -I$(SRC) $(OPTIONS) $(CTARGET)

LDLIBS= -lm -lpthread
ifeq ($(UNAME),Linux)
LDLIBS+= -lrt
endif
ifeq ($(UNAME),Windows)
LDLIBS+= -lwinmm -lws2_32
endif


all : str2str
str2str : str2str.o stream.o rtkcmn.o solution.o sbas.o geoid.o
str2str : rcvraw.o novatel.o ublox.o swiftnav.o skytraq.o gw10.o javad.o
str2str : nvs.o binex.o rt17.o rtcm.o rtcm2.o rtcm3.o rtcm3e.o preceph.o streamsvr.o
str2str : septentrio.o cmr.o tersus.o

str2str  :
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

str2str.o  : ../str2str.c
	$(CC) -c $(CFLAGS) ../str2str.c
stream.o   : $(SRC)/stream.c
	$(CC) -c $(CFLAGS) $(SRC)/stream.c
streamsvr.o: $(SRC)/streamsvr.c
	$(CC) -c $(CFLAGS) $(SRC)/streamsvr.c
rtkcmn.o   : $(SRC)/rtkcmn.c
	$(CC) -c $(CFLAGS) $(SRC)/rtkcmn.c
solution.o : $(SRC)/solution.c
	$(CC) -c $(CFLAGS) $(SRC)/solution.c
sbas.o     : $(SRC)/sbas.c
	$(CC) -c $(CFLAGS) $(SRC)/sbas.c
geoid.o    : $(SRC)/geoid.c
	$(CC) -c $(CFLAGS) $(SRC)/geoid.c
rcvraw.o   : $(SRC)/rcvraw.c
	$(CC) -c $(CFLAGS) $(SRC)/rcvraw.c
novatel.o  : $(SRC)/rcv/novatel.c
	$(CC) -c $(CFLAGS) $(SRC)/rcv/novatel.c
ublox.o    : $(SRC)/rcv/ublox.c
	$(CC) -c $(CFLAGS) $(SRC)/rcv/ublox.c
swiftnav.o : $(SRC)/rcv/swiftnav.c
	$(CC) -c $(CFLAGS) $(SRC)/rcv/swiftnav.c
skytraq.o  : $(SRC)/rcv/skytraq.c
	$(CC) -c $(CFLAGS) $(SRC)/rcv/skytraq.c
gw10.o     : $(SRC)/rcv/gw10.c
	$(CC) -c $(CFLAGS) $(SRC)/rcv/gw10.c
javad.o    : $(SRC)/rcv/javad.c
	$(CC) -c $(CFLAGS) $(SRC)/rcv/javad.c
nvs.o      : $(SRC)/rcv/nvs.c
	$(CC) -c $(CFLAGS) $(SRC)/rcv/nvs.c
binex.o    : $(SRC)/rcv/binex.c
	$(CC) -c $(CFLAGS) $(SRC)/rcv/binex.c
rt17.o     : $(SRC)/rcv/rt17.c
	$(CC) -c $(CFLAGS) $(SRC)/rcv/rt17.c
rtcm.o     : $(SRC)/rtcm.c
	$(CC) -c $(CFLAGS) $(SRC)/rtcm.c
rtcm2.o    : $(SRC)/rtcm2.c
	$(CC) -c $(CFLAGS) $(SRC)/rtcm2.c
rtcm3.o    : $(SRC)/rtcm3.c
	$(CC) -c $(CFLAGS) $(SRC)/rtcm3.c
rtcm3e.o   : $(SRC)/rtcm3e.c
	$(CC) -c $(CFLAGS) $(SRC)/rtcm3e.c
preceph.o  : $(SRC)/preceph.c
	$(CC) -c $(CFLAGS) $(SRC)/preceph.c
septentrio.o: $(SRC)/rcv/septentrio.c
	$(CC) -c $(CFLAGS) $(SRC)/rcv/septentrio.c
cmr.o      : $(SRC)/rcv/cmr.c
	$(CC) -c $(CFLAGS) $(SRC)/rcv/cmr.c
tersus.o   : $(SRC)/rcv/tersus.c
	$(CC) -c $(CFLAGS) $(SRC)/rcv/tersus.c

str2str.o  : $(SRC)/rtklib.h makefile
stream.o   : $(SRC)/rtklib.h makefile
streamsvr.o: $(SRC)/rtklib.h makefile
rtkcmn.o   : $(SRC)/rtklib.h makefile
solution.o : $(SRC)/rtklib.h makefile
sbas.o     : $(SRC)/rtklib.h makefile
geoid.o    : $(SRC)/rtklib.h makefile
rcvraw.o   : $(SRC)/rtklib.h makefile
novatel.o  : $(SRC)/rtklib.h makefile
ublox.o    : $(SRC)/rtklib.h makefile
swiftnav.o : $(SRC)/rtklib.h makefile
skytraq.o  : $(SRC)/rtklib.h makefile
gw10.o     : $(SRC)/rtklib.h makefile
javad.o    : $(SRC)/rtklib.h makefile
nvs.o      : $(SRC)/rtklib.h makefile
binex.o    : $(SRC)/rtklib.h makefile
rt17.o     : $(SRC)/rtklib.h makefile
rtcm.o     : $(SRC)/rtklib.h makefile
rtcm2.o    : $(SRC)/rtklib.h makefile
rtcm3.o    : $(SRC)/rtklib.h makefile
rtcm3e.o   : $(SRC)/rtklib.h makefile
preceph.o  : $(SRC)/rtklib.h makefile
septentrio.o: $(SRC)/rtklib.h makefile
cmr.o      : $(SRC)/rtklib.h makefile
tersus.o   : $(SRC)/rtklib.h makefile

install:
	cp str2str $(BINDIR)

#DEVICE=ttyACM0
DEVICE=ttyS1:115200:8:n:1:off
#DEVICE=ttyUSB0:115200:8:n:1:off

test1:
	./str2str -in serial://$(DEVICE) -out file://test1.out -t 2

test2:
	./str2str -in serial://$(DEVICE) -out tcpsvr://:2102 -c ../../../data/oem4_raw_10hz.cmd -t 5

test3:
	./str2str -in serial://$(DEVICE) -out ntrips://:sesam@localhost:8000/BUCU0 -c ../../../data/oem4_raw_10hz.cmd -t 5

test4:
	./str2str -in ../oem6_20121107.gps#nov -out oem6_20121107.rtcm3#rtcm3 -sta 123 -msg "1077(10),1087(10),1097(10),1107(10),1117(10),1019,1020"

test5:
	./str2str -in ../oem6_20121107.gps#nov -out oem6_20121107_gal.rtcm3#rtcm3 -msg "1097(10)"

test6:
	./str2str -in serial://ttyS54:115200#nov -out tcpsvr://:2103#rtcm3 -c ../../../data/oem4_raw_10hz.cmd

clean:
	rm -f str2str str2str.exe *.o *.out *.trace

