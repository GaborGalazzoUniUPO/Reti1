# definizione variabili
CFLAGS=-Wall -O -g -std=c99
CC=gcc
LDFLAGS=-pthread -lrt

# elenco eseguibili
EXECS=server

all: $(EXECS)

# regola generale per i file oggetto
%: %.c 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<  