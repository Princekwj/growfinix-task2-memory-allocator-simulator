#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "../include/allocator.h"

// Simple threaded test function to exercise thread-safety
void* thread_test(void* arg) {
    (void)arg;
    void* a = my_malloc(256);
    if (a) memset(a, 0xAA, 256);
    void* b = my_malloc(128);
    if (b) memset(b, 0xBB, 128);

    print_memory_map();

    my_free(a);
    my_free(b);
    return NULL;
}

int main(void) {
    init_heap();
    printf("=== Memory Allocator Simulator (Full) ===\n");

    // choose strategy
    set_alloc_strategy(STRATEGY_FIRST_FIT);
    printf("Using strategy: FIRST FIT\n");

    printf("\nInitial state:\n");
    print_memory_map();
    print_allocator_stats();

    // Basic allocations
    void* p1 = my_malloc(1000);
    printf("\nAfter allocating 1000 bytes (p1):\n");
    print_memory_map();
    print_allocator_stats();

    void* p2 = my_malloc(2000);
    printf("\nAfter allocating 2000 bytes (p2):\n");
    print_memory_map();
    print_allocator_stats();

    // Free p1 to create a free hole
    my_free(p1);
    printf("\nAfter freeing p1:\n");
    print_memory_map();
    print_allocator_stats();

    // Allocate small block to cause splitting
    void* p3 = my_malloc(128);
    printf("\nAfter allocating 128 bytes (p3):\n");
    print_memory_map();
    print_allocator_stats();

    // Realloc test
    p3 = my_realloc(p3, 512);
    printf("\nAfter realloc p3 -> 512 bytes:\n");
    print_memory_map();
    print_allocator_stats();

    // Multithreaded test
    printf("\nStarting threaded allocation test (2 threads)...\n");
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_test, NULL);
    pthread_create(&t2, NULL, thread_test, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("\nFinal state:\n");
    print_memory_map();
    print_allocator_stats();

    return 0;
}
