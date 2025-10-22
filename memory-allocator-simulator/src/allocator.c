#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include "../include/allocator.h"

// Tunable heap size for simulation (64 KB)
#define HEAP_SIZE (64 * 1024)

// Alignment (8 bytes)
#define ALIGNMENT 8
#define ALIGN_UP(x) (((x) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

typedef struct Block {
    size_t size;           // payload size (not including header)
    bool free;             // true if block is free
    struct Block* next;    // next block in list
    struct Block* prev;    // previous block (useful for coalescing)
    unsigned long magic;   // magic number for basic corruption detection
} Block;

#define BLOCK_MAGIC 0xDEADBEEFCAFEBABEUL

static char heap[HEAP_SIZE];
static Block* head = NULL;
static pthread_mutex_t alloc_lock = PTHREAD_MUTEX_INITIALIZER;
static alloc_strategy_t current_strategy = STRATEGY_FIRST_FIT;

// Internal helpers
static inline char* block_to_payload(Block* b) {
    return (char*)b + sizeof(Block);
}

static inline Block* payload_to_block(void* ptr) {
    return (Block*)((char*)ptr - sizeof(Block));
}

void init_heap(void) {
    pthread_mutex_lock(&alloc_lock);
    if (head == NULL) {
        // Setup a single large free block
        head = (Block*)heap;
        head->size = HEAP_SIZE - sizeof(Block);
        head->free = true;
        head->next = NULL;
        head->prev = NULL;
        head->magic = BLOCK_MAGIC;
    }
    pthread_mutex_unlock(&alloc_lock);
}

void set_alloc_strategy(alloc_strategy_t strat) {
    pthread_mutex_lock(&alloc_lock);
    current_strategy = strat;
    pthread_mutex_unlock(&alloc_lock);
}

// Split block into allocated part and remainder free part (if big enough)
static void split_block(Block* block, size_t req_size) {
    // req_size is already aligned
    if (!block) return;
    if (block->size < req_size + sizeof(Block) + ALIGNMENT) {
        // not enough space to split meaningfully
        return;
    }

    char* new_block_addr = block_to_payload(block) + req_size;
    Block* new_block = (Block*)new_block_addr;
    new_block->size = block->size - req_size - sizeof(Block);
    new_block->free = true;
    new_block->magic = BLOCK_MAGIC;
    new_block->next = block->next;
    new_block->prev = block;

    if (block->next) block->next->prev = new_block;
    block->next = new_block;
    block->size = req_size;
}

// Coalesce block with next if both free
static void coalesce_with_next(Block* block) {
    if (!block || !block->next) return;
    Block* nxt = block->next;
    if (nxt->free) {
        // merge nxt into block
        block->size += sizeof(Block) + nxt->size;
        block->next = nxt->next;
        if (nxt->next) nxt->next->prev = block;
    }
}

// Find block using First Fit
static Block* find_first_fit(size_t size) {
    Block* cur = head;
    while (cur) {
        if (cur->free && cur->size >= size) return cur;
        cur = cur->next;
    }
    return NULL;
}

// Find block using Best Fit
static Block* find_best_fit(size_t size) {
    Block* cur = head;
    Block* best = NULL;
    while (cur) {
        if (cur->free && cur->size >= size) {
            if (best == NULL || cur->size < best->size) best = cur;
        }
        cur = cur->next;
    }
    return best;
}

// Try to extend by coalescing forward neighbors until enough
static Block* expand_by_coalescing(Block* block, size_t size) {
    Block* cur = block;
    // Keep coalescing with next free blocks while total size insufficient
    while (cur && cur->free && cur->size < size) {
        if (!cur->next || !cur->next->free) break;
        coalesce_with_next(cur);
    }
    if (cur->free && cur->size >= size) return cur;
    return NULL;
}

void* my_malloc(size_t size) {
    if (size == 0) return NULL;
    pthread_mutex_lock(&alloc_lock);

    if (!head) init_heap();

    size_t aligned = ALIGN_UP(size);
    Block* chosen = NULL;

    if (current_strategy == STRATEGY_FIRST_FIT) {
        chosen = find_first_fit(aligned);
    } else {
        chosen = find_best_fit(aligned);
    }

    // If chosen exists but too small due to fragmentation, try coalescing forward
    if (chosen && chosen->free && chosen->size < aligned) {
        chosen = expand_by_coalescing(chosen, aligned);
    }

    // If none found, attempt to scan and coalesce opportunistically
    if (!chosen) {
        Block* cur = head;
        while (cur) {
            if (cur->free) {
                // try to coalesce with neighbors to make room
                Block* merged = expand_by_coalescing(cur, aligned);
                if (merged && merged->size >= aligned) {
                    chosen = merged;
                    break;
                }
            }
            cur = cur->next;
        }
    }

    if (!chosen) {
        // No memory available
        pthread_mutex_unlock(&alloc_lock);
        return NULL;
    }

    // If block is significantly larger, split it
    split_block(chosen, aligned);
    chosen->free = false;

    // Return payload pointer
    void* payload = (void*)block_to_payload(chosen);
    pthread_mutex_unlock(&alloc_lock);
    return payload;
}

void my_free(void* ptr) {
    if (ptr == NULL) return;

    pthread_mutex_lock(&alloc_lock);
    // Basic pointer validity check: must be inside simulated heap range
    if ((char*)ptr < heap + sizeof(Block) || (char*)ptr >= heap + HEAP_SIZE) {
        fprintf(stderr, "my_free: pointer out of heap range\n");
        pthread_mutex_unlock(&alloc_lock);
        return;
    }

    Block* block = payload_to_block(ptr);
    if (block->magic != BLOCK_MAGIC) {
        fprintf(stderr, "my_free: corruption or invalid pointer detected (magic mismatch)\n");
        pthread_mutex_unlock(&alloc_lock);
        return;
    }

    if (block->free) {
        fprintf(stderr, "my_free: double free detected\n");
        pthread_mutex_unlock(&alloc_lock);
        return;
    }

    block->free = true;

    // Coalesce with next if free
    if (block->next && block->next->free) {
        coalesce_with_next(block);
    }

    // Coalesce with previous if free
    if (block->prev && block->prev->free) {
        coalesce_with_next(block->prev);
    }

    pthread_mutex_unlock(&alloc_lock);
}

void* my_realloc(void* ptr, size_t new_size) {
    if (ptr == NULL) return my_malloc(new_size);
    if (new_size == 0) {
        my_free(ptr);
        return NULL;
    }

    pthread_mutex_lock(&alloc_lock);
    Block* block = payload_to_block(ptr);
    if (block->magic != BLOCK_MAGIC) {
        pthread_mutex_unlock(&alloc_lock);
        return NULL;
    }

    size_t aligned = ALIGN_UP(new_size);

    if (block->size >= aligned) {
        // currently enough; optionally split
        split_block(block, aligned);
        pthread_mutex_unlock(&alloc_lock);
        return ptr;
    }

    // Check if next block is free and can be merged to satisfy size
    if (block->next && block->next->free &&
        block->size + sizeof(Block) + block->next->size >= aligned) {
        coalesce_with_next(block);
        split_block(block, aligned);
        block->free = false;
        pthread_mutex_unlock(&alloc_lock);
        return ptr;
    }

    // Otherwise allocate new block, copy data, free old
    pthread_mutex_unlock(&alloc_lock);
    void* newptr = my_malloc(aligned);
    if (!newptr) return NULL;

    // copy smaller of old and new sizes
    size_t copy_sz = block->size < aligned ? block->size : aligned;
    memcpy(newptr, ptr, copy_sz);
    my_free(ptr);
    return newptr;
}

void print_memory_map(void) {
    pthread_mutex_lock(&alloc_lock);
    if (!head) {
        printf("Heap not initialized.\n");
        pthread_mutex_unlock(&alloc_lock);
        return;
    }
    printf("---------- Memory Map ----------\n");
    Block* cur = head;
    size_t idx = 0;
    while (cur) {
        printf("Block %2zu | addr=%p | size=%6zu | %s\n",
               idx,
               (void*)cur,
               cur->size,
               cur->free ? "FREE" : "USED");
        cur = cur->next;
        ++idx;
    }
    printf("--------------------------------\n");
    pthread_mutex_unlock(&alloc_lock);
}

void print_allocator_stats(void) {
    pthread_mutex_lock(&alloc_lock);
    size_t total_free = 0, total_used = 0;
    size_t free_blocks = 0, used_blocks = 0;

    Block* cur = head;
    while (cur) {
        if (cur->free) {
            total_free += cur->size;
            free_blocks++;
        } else {
            total_used += cur->size;
            used_blocks++;
        }
        cur = cur->next;
    }

    printf("Allocator Stats:\n");
    printf("Total heap: %zu bytes\n", (size_t)HEAP_SIZE);
    printf("Used     : %zu bytes in %zu block(s)\n", total_used, used_blocks);
    printf("Free     : %zu bytes in %zu block(s)\n", total_free, free_blocks);
    printf("External fragmentation estimate: %zu bytes\n", total_free);
    pthread_mutex_unlock(&alloc_lock);
}
