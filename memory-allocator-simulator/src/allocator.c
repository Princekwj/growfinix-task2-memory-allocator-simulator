#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "../include/allocator.h"

#define HEAP_SIZE 1024  // 1 KB heap for simulation

typedef struct Block {
    size_t size;          // size of block
    bool free;            // free or allocated
    struct Block* next;   // next block
} Block;

static char heap[HEAP_SIZE];   // simulated heap
static Block* free_list = NULL;

// Initialize heap
void init_heap() {
    free_list = (Block*)heap;
    free_list->size = HEAP_SIZE - sizeof(Block);
    free_list->free = true;
    free_list->next = NULL;
}

// Split block
static void split_block(Block* block, size_t size) {
    if (block->size >= size + sizeof(Block) + 1) {
        Block* new_block = (Block*)((char*)block + sizeof(Block) + size);
        new_block->size = block->size - size - sizeof(Block);
        new_block->free = true;
        new_block->next = block->next;

        block->size = size;
        block->next = new_block;
    }
}

// Custom malloc
void* my_malloc(size_t size) {
    Block* current = free_list;
    while (current != NULL) {
        if (current->free && current->size >= size) {
            split_block(current, size);
            current->free = false;
            return (char*)current + sizeof(Block);
        }
        current = current->next;
    }
    return NULL; // no memory
}

// Custom free
void my_free(void* ptr) {
    if (ptr == NULL) return;

    Block* block = (Block*)((char*)ptr - sizeof(Block));
    block->free = true;

    // TODO: coalescing (later part of 60%)
}

// Print memory map
void print_memory_map() {
    Block* current = free_list;
    printf("Memory Map:\n");
    while (current != NULL) {
        printf("[Block: %zu bytes | %s]\n",
               current->size,
               current->free ? "FREE" : "USED");
        current = current->next;
    }
    printf("\n");
}
