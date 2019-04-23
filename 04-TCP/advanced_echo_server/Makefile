# definizione variabili
CFLAGS=-Wall -O -g -std=c99
CC=gcc
LDFLAGS=-pthread -lrt

# elenco eseguibili
EXECS=client server

all: $(EXECS)

# regola generale per i file oggetto
%: %.c 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<  