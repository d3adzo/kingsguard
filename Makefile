CC=x86_64-w64-mingw32-g++
OUTDIR=bin
SYM=-s
FLAGS=-static -shared -Iminhook -Lminhook -lminhook

all: clean knight guard

guard:
	$(CC) guard.cpp -o $(OUTDIR)/kingsguard.dll $(FLAGS) $(SYM)

knight:
	$(CC) knight.cpp -o $(OUTDIR)/knight.exe $(SYM)

clean:
	mkdir -p $(OUTDIR)
	rm -f $(OUTDIR)/*
