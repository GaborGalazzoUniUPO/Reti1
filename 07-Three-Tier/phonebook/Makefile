# definizione variabili
CFLAGS=-Wall -O -g -std=c99
CC=gcc
LDFLAGS=-pthread -lrt

# elenco eseguibili
EXECS=server

all: $(EXECS)

# regole per gli eseguibili
server: server.o  protocol.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# regola generale per i file oggetto
%.o: %.c protocol.h
	$(CC) $(CFLAGS) -o $@ -c $<

# protocol.o
protocol.o: protocol.c  protocol.h
	$(CC) $(CFLAGS) -o protocol.o -c protocol.c



clean:
	rm *.o $(EXECS)