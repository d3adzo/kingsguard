CC=x86_64-w64-mingw32-g++
CP=i686-w64-mingw32-g++
OUTDIR=bin
CFLAGS=-shared -s -static -Iinclude -fpermissive
DFLAGS32=-Iminhook32 -Lminhook32 -lminhook
DFLAGS64=-Iminhook -Lminhook -lminhook

all: clean 64kingsguard 32kingsguard

32kingsguard:
	$(CP) $(wildcard src/*.cpp) -o $(OUTDIR)/kingsguard32.dll $(DFLAGS32) $(CFLAGS)

64kingsguard:
	$(CC) $(wildcard src/*.cpp) -o $(OUTDIR)/kingsguard.dll $(DFLAGS64) $(CFLAGS)

clean:
	mkdir -p $(OUTDIR)
	rm -f $(OUTDIR)/*
