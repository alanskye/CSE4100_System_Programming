CC = gcc
CFLAGS=-O2 -Wall
LDLIBS = -lpthread

all: multiclient stockclient stockserver

multiclient: multiclient.c csapp.c csapp.h stockdb.c stockdb.h bst.c bst.h
stockclient: stockclient.c csapp.c csapp.h stockdb.c stockdb.h bst.c bst.h
stockserver: stockserver.c echo.c csapp.c csapp.h stockdb.c stockdb.h bst.c bst.h

clean:
	rm -rf *~ multiclient stockclient stockserver *.o
