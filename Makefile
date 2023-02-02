CC=x86_64-w64-mingw32-g++

all:
	$(CC) main.cpp -o main.dll -static -shared -Iminhook -Lminhook -lminhook -s


clean:
	rm main.dll
