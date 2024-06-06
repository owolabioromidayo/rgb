#apt install freeglut3-dev

CC = g++
CFLAGS = -Wall -std=c++20
LDFLAGS = -lGL -lGLU -lglut -lglfw -lGLEW  -lassimp

TARGET := main
SRC_DIR := src
OBJ_DIR := obj
SOURCES := $(wildcard $(SRC_DIR)/*.cc)
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cc=$(OBJ_DIR)/%.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

clean:
	$(RM) -r $(OBJ_DIR) $(TARGET)