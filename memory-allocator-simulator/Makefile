# -------------------------------
# Makefile for Memory Allocator Simulator
# -------------------------------

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Iinclude -pthread

# Source files
SRCS = $(wildcard src/*.c)

# Output executable
TARGET = allocator_simulator

# Default target
all: $(TARGET)

# Build executable
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

# Clean build files
clean:
	rm -f $(TARGET) *.o

# Run the program
run: $(TARGET)
	./$(TARGET)
