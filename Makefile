# sudo apt install fonts-liberation

CC = g++
CFLAGS = -Wall -std=c++20
LDFLAGS = -lGL -lGLU -lglut -lglfw -lGLEW -lassimp

# Use pkg-config to get FreeType flags
FREETYPE_CFLAGS = $(shell pkg-config --cflags freetype2)
FREETYPE_LIBS = $(shell pkg-config --libs freetype2)

TARGET := main
SRC_DIR := src
OBJ_DIR := obj
SOURCES := $(wildcard $(SRC_DIR)/*.cc)
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cc=$(OBJ_DIR)/%.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(FREETYPE_LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ $(FREETYPE_CFLAGS)

$(OBJ_DIR):
	mkdir -p $@

clean:
	$(RM) -r $(OBJ_DIR) $(TARGET)#apt install freeglut3-dev

# CC = g++
# CFLAGS = -Wall -std=c++20
# LDFLAGS = -lGL -lGLU -lglut -lglfw -lGLEW  -lassimp -lfreetype

# TARGET := main
# SRC_DIR := src
# OBJ_DIR := obj
# SOURCES := $(wildcard $(SRC_DIR)/*.cc)
# OBJECTS := $(SOURCES:$(SRC_DIR)/%.cc=$(OBJ_DIR)/%.o)

# .PHONY: all clean

# all: $(TARGET)

# $(TARGET): $(OBJECTS)
# 	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc | $(OBJ_DIR)
# 	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

# $(OBJ_DIR):
# 	mkdir -p $@

# clean:
# 	$(RM) -r $(OBJ_DIR) $(TARGET)