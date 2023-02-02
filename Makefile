CC=x86_64-w64-mingw32-g++

all:
	$(CC) main.cpp -o main.dll -shared -static -L. -l:libminhook.a
