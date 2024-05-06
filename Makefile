#apt install freeglut3-dev

CC = g++
CFLAGS = -Wall -std=c++20
LDFLAGS = -lGL -lGLU -lglut

TARGET = main
SOURCES = src/main.cc

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
