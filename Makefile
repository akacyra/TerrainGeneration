CC :=g++
SOURCE_FILES := $(wildcard src/*.cpp) $(wildcard src/*/*.cpp)
SRC_OBJ := $(SOURCE_FILES:%.cpp=%.o)
OBJ_FILES := $(SRC_OBJ:src/%=obj/%)
LD_FLAGS := -lm
CC_FLAGS := -Wall -MMD -std=c++11 -Iinclude -g
TARGET := terrain

all: entry 

entry: directories $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) -o $@ $^ $(LD_FLAGS)

obj/%.o: src/%.cpp
	$(CC) $(CC_FLAGS) -c -o $@ $<

.PHONY: directories clean
directories:
	mkdir -p obj/ obj/Noise

clean:
	rm -f $(TARGET) $(OBJ_FILES) $(OBJ_FILES:.o=.d)

# Automatic dependency graph generation with -MMD
-include $(OBJ_FILES:.o=.d)
