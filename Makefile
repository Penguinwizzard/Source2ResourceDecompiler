CC=gcc
CFLAGS=-c -O2 -Wall -Werror -Wextra
LFLAGS=-O2 -Wall -Werror -Wextra

all: s2rd
s2rd: s2rd.o fileinfo.o stupidvalve.o
	$(CC) $(LFLAGS) s2rd.o fileinfo.o stupidvalve.o -o s2rd
s2rd.o: s2rd.c s2rd.h
	$(CC) $(CFLAGS) s2rd.c -o s2rd.o
fileinfo.o: fileinfo.c fileinfo.h
	$(CC) $(CFLAGS) fileinfo.c -o fileinfo.o
stupidvalve.o: stupidvalve.c stupidvalve.h
	$(CC) $(CFLAGS) stupidvalve.c -o stupidvalve.o
clean:
	rm -f *.o s2rd
