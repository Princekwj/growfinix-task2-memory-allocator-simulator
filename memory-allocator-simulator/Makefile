CC = gcc
<<<<<<< HEAD
CFLAGS = -Wall -Iinclude

all: allocator

allocator: src/main.c src/allocator.c
	$(CC) $(CFLAGS) src/main.c src/allocator.c -o allocator

clean:
	rm -f allocator
=======
CFLAGS = -Wall -Wextra -pthread -Iinclude -g
SRCS = src/allocator.c src/main.c
TARGET = allocator

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
>>>>>>> 6aa803f (Updated full version: coalescing, best-fit, thread safety, and improved README)
