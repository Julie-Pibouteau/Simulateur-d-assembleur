
CC=gcc
CFLAGS=-Wall
LDFLAGS=-lm
EXEC=simulateur


all: $(EXEC)

simulateur: main.o lire_instructions.o etiquette.o conversion.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

main.o: main.c lire_instructions.h etiquette.h conversion.h
	$(CC) -o $@ -c $< $(CFLAGS)

lire_instructions.o: lire_instructions.c lire_instructions.h etiquette.h
	$(CC) -o $@ -c $< $(CFLAGS)

etiquette.o: etiquette.c etiquette.h
	$(CC) -o $@ -c $< $(CFLAGS)

conversion.o: conversion.c conversion.h
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -f *.o core

mrproper: clean
	rm -f $(EXEC)

