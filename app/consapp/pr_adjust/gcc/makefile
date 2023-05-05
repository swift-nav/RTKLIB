# makefile for pr_adjust
BINDIR  = /usr/local/bin
SRC     = ../../../../src

OPTS    = -DTRACE -DENAGLO -DENAQZS -DENAGAL -DENACMP -DENAIRN -DNFREQ=5
#OPTS    = -DTRACE -DENAGLO -DENAQZS -DENAGAL -DENACMP -DENAIRN -DNFREQ=5 -DIERS_MODEL
#OPTS    = -DENAGLO -DENAQZS -DENAGAL -DENACMP -DNFREQ=2

CC = gcc
CFLAGS = -Wall -O2 -g -Wno-unused-but-set-variable -I$(SRC) $(OPTS)
LDLIBS = -lm -lrt

all        : pr_adjust
pr_adjust  : pr_adjust.o rtkcmn.o rinex.o rtkpos.o postpos.o solution.o
pr_adjust  : lambda.o geoid.o sbas.o preceph.o pntpos.o ephemeris.o
pr_adjust  : ppp.o ppp_ar.o rtcm.o rtcm2.o rtcm3.o rtcm3e.o ionex.o tides.o

pr_adjust.o: ../pr_adjust.c
	$(CC) -c $(CFLAGS) ../pr_adjust.c
rtkcmn.o   : $(SRC)/rtkcmn.c
	$(CC) -c $(CFLAGS) $(SRC)/rtkcmn.c
rinex.o   : $(SRC)/rinex.c
	$(CC) -c $(CFLAGS) $(SRC)/rinex.c
rtkpos.o   : $(SRC)/rtkpos.c
	$(CC) -c $(CFLAGS) $(SRC)/rtkpos.c
postpos.o  : $(SRC)/postpos.c
	$(CC) -c $(CFLAGS) $(SRC)/postpos.c
solution.o : $(SRC)/solution.c
	$(CC) -c $(CFLAGS) $(SRC)/solution.c
lambda.o   : $(SRC)/lambda.c
	$(CC) -c $(CFLAGS) $(SRC)/lambda.c
geoid.o    : $(SRC)/geoid.c
	$(CC) -c $(CFLAGS) $(SRC)/geoid.c
sbas.o     : $(SRC)/sbas.c
	$(CC) -c $(CFLAGS) $(SRC)/sbas.c
preceph.o  : $(SRC)/preceph.c
	$(CC) -c $(CFLAGS) $(SRC)/preceph.c
pntpos.o   : $(SRC)/pntpos.c
	$(CC) -c $(CFLAGS) $(SRC)/pntpos.c
ephemeris.o: $(SRC)/ephemeris.c
	$(CC) -c $(CFLAGS) $(SRC)/ephemeris.c
options.o  : $(SRC)/options.c
	$(CC) -c $(CFLAGS) $(SRC)/options.c
ppp.o      : $(SRC)/ppp.c
	$(CC) -c $(CFLAGS) $(SRC)/ppp.c
ppp_ar.o   : $(SRC)/ppp_ar.c
	$(CC) -c $(CFLAGS) $(SRC)/ppp_ar.c
rtcm.o     : $(SRC)/rtcm.c
	$(CC) -c $(CFLAGS) $(SRC)/rtcm.c
rtcm2.o    : $(SRC)/rtcm2.c
	$(CC) -c $(CFLAGS) $(SRC)/rtcm2.c
rtcm3.o    : $(SRC)/rtcm3.c
	$(CC) -c $(CFLAGS) $(SRC)/rtcm3.c
rtcm3e.o   : $(SRC)/rtcm3e.c
	$(CC) -c $(CFLAGS) $(SRC)/rtcm3e.c
ionex.o    : $(SRC)/ionex.c
	$(CC) -c $(CFLAGS) $(SRC)/ionex.c
tides.o    : $(SRC)/tides.c
	$(CC) -c $(CFLAGS) $(SRC)/tides.c

rnx2rtkp.o : $(SRC)/rtklib.h
rtkcmn.o   : $(SRC)/rtklib.h
rinex.o    : $(SRC)/rtklib.h
rtkpos.o   : $(SRC)/rtklib.h
postpos.o  : $(SRC)/rtklib.h
solution.o : $(SRC)/rtklib.h
lambda.o   : $(SRC)/rtklib.h
geoid.o    : $(SRC)/rtklib.h
sbas.o     : $(SRC)/rtklib.h
preceph.o  : $(SRC)/rtklib.h
pntpos.o   : $(SRC)/rtklib.h
ephemeris.o: $(SRC)/rtklib.h
options.o  : $(SRC)/rtklib.h
ppp.o      : $(SRC)/rtklib.h
ppp_ar.o   : $(SRC)/rtklib.h
rtcm.o     : $(SRC)/rtklib.h
rtcm2.o    : $(SRC)/rtklib.h
rtcm3.o    : $(SRC)/rtklib.h
rtcm3e.o   : $(SRC)/rtklib.h
ionex.o    : $(SRC)/rtklib.h
tides.o    : $(SRC)/rtklib.h

clean :
	rm -f pr_adjust pr_adjust.exe *.o *.pos *.trace
