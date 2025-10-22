CC = gcc
CFLAGS = -Wall -Wextra -pthread -Iinclude -g
SRCS = src/allocator.c src/main.c
TARGET = allocator

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
