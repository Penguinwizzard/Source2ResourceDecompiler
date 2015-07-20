CC=gcc
CFLAGS=-c -O2 -Wall -Wextra
LFLAGS=-O2 -Wall -Wextra

all: s2rd panoramacompiler
panoramacompiler: panoramacompiler.o
	$(CC) $(LFLAGS) panoramacompiler.o -o panoramacompiler
panoramacompiler.o: stupidvalve.h panoramacompiler.c
	$(CC) $(CFLAGS) panoramacompiler.c -o panoramacompiler.o
s2rd: s2rd.o fileinfo.o stupidvalve.o vcs.o
	$(CC) $(LFLAGS) s2rd.o fileinfo.o stupidvalve.o vcs.o -o s2rd
s2rd.o: s2rd.c s2rd.h
	$(CC) $(CFLAGS) s2rd.c -o s2rd.o
fileinfo.o: fileinfo.c fileinfo.h
	$(CC) $(CFLAGS) fileinfo.c -o fileinfo.o
stupidvalve.o: stupidvalve.c stupidvalve.h
	$(CC) $(CFLAGS) stupidvalve.c -o stupidvalve.o
vcs.o: vcs.c vcs.h
	$(CC) $(CFLAGS) vcs.c -o vcs.o
js: CC=emcc
js: s2rd.js
s2rd.js: s2rd.o fileinfo.o stupidvalve.o vcs.o
	$(CC) $(LFLAGS) s2rd.o fileinfo.o stupidvalve.o vcs.o -o s2rd.js --embed-file bin/
clean:
	rm -f *.o s2rd s2rd.js s2rd.js.mem
