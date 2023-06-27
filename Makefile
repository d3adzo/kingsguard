CC=x86_64-w64-mingw32-g++
OUTDIR=bin
CFLAGS=-s -static
DFLAGS=-shared -Iminhook -Lminhook -lminhook

all: clean kingsguard

kingsguard:
	$(CC) start.cpp -o $(OUTDIR)/kingsguard.dll $(DFLAGS) $(CFLAGS)

clean:
	mkdir -p $(OUTDIR)
	rm -f $(OUTDIR)/*
