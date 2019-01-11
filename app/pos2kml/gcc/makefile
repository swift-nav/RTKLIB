# makefile for pos2kml
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

CC=gcc

OPTIONS =
ifeq ($(UNAME),Windows)
OPTIONS += -DWIN32
endif

CFLAGS = -Wall -O2 -ansi -std=c99 -pedantic -I$(SRC) $(OPTIONS) -DTRACE

LDLIBS= -lm -lpthread
ifeq ($(UNAME),Linux)
LDLIBS+= -lrt
endif
ifeq ($(UNAME),Windows)
LDLIBS+= -lwinmm
endif

pos2kml    : pos2kml.o convkml.o convgpx.o solution.o geoid.o rtkcmn.o preceph.o

pos2kml.o  : ../pos2kml.c
	$(CC) -c $(CFLAGS) ../pos2kml.c
convkml.o  : $(SRC)/convkml.c
	$(CC) -c $(CFLAGS) $(SRC)/convkml.c
convgpx.o  : $(SRC)/convgpx.c
	$(CC) -c $(CFLAGS) $(SRC)/convgpx.c
solution.o : $(SRC)/solution.c
	$(CC) -c $(CFLAGS) $(SRC)/solution.c
geoid.o    : $(SRC)/geoid.c
	$(CC) -c $(CFLAGS) $(SRC)/geoid.c
rtkcmn.o   : $(SRC)/rtkcmn.c
	$(CC) -c $(CFLAGS) $(SRC)/rtkcmn.c
preceph.o  : $(SRC)/preceph.c
	$(CC) -c $(CFLAGS) $(SRC)/preceph.c

pos2kml.o  : $(SRC)/rtklib.h makefile
convkml.o  : $(SRC)/rtklib.h makefile
convgpx.o  : $(SRC)/rtklib.h makefile
solution.o : $(SRC)/rtklib.h makefile
geoid.o    : $(SRC)/rtklib.h makefile
rtkcmn.o   : $(SRC)/rtklib.h makefile
preceph.o  : $(SRC)/rtklib.h makefile

install:
	cp pos2kml $(BINDIR)

clean:
	rm -f pos2kml pos2kml.exe *.o
