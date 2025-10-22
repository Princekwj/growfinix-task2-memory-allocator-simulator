#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

<<<<<<< HEAD
// Initialize heap
void init_heap();

// Allocate memory
void* my_malloc(size_t size);

// Free memory
void my_free(void* ptr);

// Print memory map
void print_memory_map();
=======
// Allocation strategy enum
typedef enum {
    STRATEGY_FIRST_FIT,
    STRATEGY_BEST_FIT
} alloc_strategy_t;

// Initialize heap (must be called before any allocation)
void init_heap(void);

// Set allocation strategy (default: FIRST_FIT)
void set_alloc_strategy(alloc_strategy_t strat);

// Allocate memory (thread-safe)
void* my_malloc(size_t size);

// Free memory (thread-safe)
void my_free(void* ptr);

// Reallocate memory (thread-safe)
void* my_realloc(void* ptr, size_t new_size);

// Print memory map for debugging
void print_memory_map(void);

// Print allocator statistics
void print_allocator_stats(void);
>>>>>>> 6aa803f (Updated full version: coalescing, best-fit, thread safety, and improved README)

#endif // ALLOCATOR_H
