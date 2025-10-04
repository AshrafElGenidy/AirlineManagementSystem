CXX = g++

# Find all subdirectories in include
INC_DIRS = $(wildcard include/*/)
INC_FLAGS = -Iinclude $(addprefix -I,$(INC_DIRS))

CXXFLAGS = -std=c++17 -Wall $(INC_FLAGS)

# Rest of your makefile...
SRC = $(wildcard src/*.cpp src/**/*.cpp)
OBJ = $(patsubst src/%.cpp,obj/%.o,$(SRC))
TARGET = AirlineManagementSystem

build: $(TARGET)
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

obj/%.o: src/%.cpp
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@if exist obj (rd /s /q obj)
	@if exist $(TARGET).exe (del /q $(TARGET).exe)

run: $(TARGET)
	$(TARGET).exe