CC=gcc
CFLAGS=-c -O2 -Wall -Wextra
LFLAGS=-O2 -Wall -Wextra

all: s2rd panoramacompiler
panoramacompiler: panoramacompiler.o crc32.o
	$(CC) $(LFLAGS) panoramacompiler.o crc32.o -o panoramacompiler
panoramacompiler.o: stupidvalve.h panoramacompiler.c
	$(CC) $(CFLAGS) panoramacompiler.c -o panoramacompiler.o
s2rd: s2rd.o fileinfo.o stupidvalve.o vcs.o decompilers/SVF1.o
	$(CC) $(LFLAGS) s2rd.o fileinfo.o stupidvalve.o vcs.o decompilers/SVF1.o -o s2rd
s2rd.o: s2rd.c s2rd.h
	$(CC) $(CFLAGS) s2rd.c -o s2rd.o
fileinfo.o: fileinfo.c fileinfo.h
	$(CC) $(CFLAGS) fileinfo.c -o fileinfo.o
stupidvalve.o: stupidvalve.c stupidvalve.h
	$(CC) $(CFLAGS) stupidvalve.c -o stupidvalve.o
vcs.o: vcs.c vcs.h
	$(CC) $(CFLAGS) vcs.c -o vcs.o
crc32.o: crc32.c crc32.h
	$(CC) $(CFLAGS) crc32.c -o crc32.o
decompilers/SVF1.o: decompilers/SVF1.c decompilers/SVF1.h
	$(CC) $(CFLAGS) decompilers/SVF1.c -o decompilers/SVF1.o
js: CC=emcc
js: s2rd.js
s2rd.js: s2rd.o fileinfo.o stupidvalve.o vcs.o
	$(CC) $(LFLAGS) s2rd.o fileinfo.o stupidvalve.o vcs.o -o s2rd.js --embed-file bin/
clean:
	rm -f *.o decompilers/*.o s2rd s2rd.js s2rd.js.mem
