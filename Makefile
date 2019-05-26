CC=gcc
CFLAGS=-c -O2 -Wall -Wextra -pedantic -Wno-implicit-fallthrough -std=gnu99
LFLAGS=-O2 -Wall -Wextra -pedantic -lm

all: s2rd panoramacompiler panorepack
panoramacompiler: panoramacompiler.o crc32.o stupidvalve.o
	$(CC) $(LFLAGS) panoramacompiler.o crc32.o stupidvalve.o -o panoramacompiler
panoramacompiler.o: stupidvalve.h panoramacompiler.c
	$(CC) $(CFLAGS) panoramacompiler.c -o panoramacompiler.o
panorepack: panorepack.o crc32.o stupidvalve.o fileinfo.o vcs.o dmx.o dmx/dmx.o dmx/dmxb.o
	$(CC) $(LFLAGS) panorepack.o crc32.o stupidvalve.o fileinfo.o vcs.o dmx.o dmx/dmx.o dmx/dmxb.o -o panorepack
panorepack.o: panorepack.c
	$(CC) $(CFLAGS) panorepack.c -o panorepack.o
s2rd: s2rd.o fileinfo.o stupidvalve.o vcs.o decompilers/SVF1.o dmx.o dmx/dmx.o dmx/dmxb.o
	$(CC) $(LFLAGS) s2rd.o fileinfo.o stupidvalve.o vcs.o dmx.o dmx/dmx.o dmx/dmxb.o decompilers/SVF1.o -o s2rd
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
dmx.o: dmx.c dmx.h dmx/dmx.h
	$(CC) $(CFLAGS) dmx.c -o dmx.o
dmx/dmx.o: dmx/dmx.c dmx/dmx.h dmx/dmxb.h dmx/dmxbraw.h
	$(CC) $(CFLAGS) dmx/dmx.c -o dmx/dmx.o
dmx/dmxb.o: dmx/dmx.h dmx/dmxb.h dmx/dmxb.c dmx/dmxbraw.c
	$(CC) $(CFLAGS) dmx/dmxb.c -o dmx/dmxb.o
js: CC=emcc
js: s2rd.js
s2rd.js: s2rd.o fileinfo.o stupidvalve.o vcs.o
	$(CC) $(LFLAGS) s2rd.o fileinfo.o stupidvalve.o vcs.o -o s2rd.js --embed-file bin/
clean:
	rm -f *.o decompilers/*.o dmx/*.o s2rd s2rd.js s2rd.js.mem panoramacompiler panorepack
