#include <stdio.h>
#include "../include/allocator.h"

int main() {
    init_heap();

    printf("Initial Heap:\n");
    print_memory_map();

    void* p1 = my_malloc(100);
    printf("After allocating 100 bytes:\n");
    print_memory_map();

    void* p2 = my_malloc(200);
    printf("After allocating 200 bytes:\n");
    print_memory_map();

    my_free(p1);
    printf("After freeing first block:\n");
    print_memory_map();

    return 0;
}
