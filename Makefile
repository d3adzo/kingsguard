32P := i686-w64-mingw32
64P := x86_64-w64-mingw32
SRC_DIR := src
OBJ_DIR := obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
THR = -Lminhook32
SIX = -Lminhook
MH := -lminhook
LDFLAGS := -shared -s -static 
CXXFLAGS := -Iinclude -fpermissive 

all: resource.o kgsgrd.dll kgsgrd32.dll

resource.o:
	$(64P)-windres resource.rc -o $(OBJ_DIR)/$@

resource32.o:
	$(32P)-windres resource.rc -o minhook32/$@

kgsgrd32.dll: resource32.o
	$(32P)-g++ $(SRC_FILES) minhook32/resource32.o -o $@ -shared $(THR) $(MH) $(LDFLAGS) $(CXXFLAGS) 

kgsgrd.dll: $(OBJ_FILES)
	$(64P)-g++ -o $@ $(wildcard $(OBJ_DIR)/*.o) $(SIX) $(MH) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(64P)-g++ -c -o $@ $< $(CXXFLAGS) 

clean:
	mkdir -p $(OBJ_DIR)
	rm -f *.dll