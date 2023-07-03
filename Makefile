CXX := x86_64-w64-mingw32-g++
SRC_DIR := src
OBJ_DIR := obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -Lminhook -lminhook -shared -s -static 
CXXFLAGS := -Iinclude -fpermissive 

kingsguard.dll: $(OBJ_FILES)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS) 

clean:
	rm -f $(OBJ_DIR)/*.o