#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

// Initialize heap
void init_heap();

// Allocate memory
void* my_malloc(size_t size);

// Free memory
void my_free(void* ptr);

// Print memory map
void print_memory_map();

#endif // ALLOCATOR_H
