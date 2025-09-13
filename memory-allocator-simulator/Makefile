CC = gcc
CFLAGS = -Wall -Iinclude

all: allocator

allocator: src/main.c src/allocator.c
	$(CC) $(CFLAGS) src/main.c src/allocator.c -o allocator

clean:
	rm -f allocator
