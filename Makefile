CXX := x86_64-w64-mingw32-g++
C32 := i686-w64-mingw32-g++
SRC_DIR := src
OBJ_DIR := obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
THR = -Lminhook32
SIX = -Lminhook
LDFLAGS := -lminhook -shared -s -static 
CXXFLAGS := -Iinclude -fpermissive 

all: kingsguard.dll

32bit: clean
	$(C32) src/*.cpp -o kingsguard32.dll -shared $(THR) $(LDFLAGS) $(CXXFLAGS) 

kingsguard.dll: $(OBJ_FILES)
	$(CXX) -o $@ $^ $(SIX) $(LDFLAGS)

kingsguard32.dll: $(OBJ_FILES)
	$(C32) -o $@ $^ $(THR) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS) 

clean:
	mkdir -p $(OBJ_DIR)
	rm -f $(OBJ_DIR)/*.o