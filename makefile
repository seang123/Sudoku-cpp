CC = g++ 

CFLAGS = -g -Wall -std=c++11


TARGET = main

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) -O3 -funroll-loops -Wall -Wextra -Wpedantic $(CFLAGS) -o $(TARGET) $(TARGET).cpp