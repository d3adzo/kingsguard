CC=x86_64-w64-mingw32-g++
OUTDIR=bin
CFLAGS=-s -static
DFLAGS=-shared -Iminhook -Lminhook -lminhook

all: clean knight archer kingsguard

kingsguard:
	$(CC) kingsguard.cpp -o $(OUTDIR)/kingsguard.dll $(FLAGS) $(SYM)

knight:
	$(CC) knight.cpp -o $(OUTDIR)/knight.exe $(SYM)

archer:
	$(CC) archer.cpp -o $(OUDIR)/archer.exe $(SYM)

clean:
	mkdir -p $(OUTDIR)
	rm -f $(OUTDIR)/*
