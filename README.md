# Memory Allocator Simulator (C)

This project simulates a **custom memory allocator** in C, similar to `malloc()` and `free()`.

## Features (40% Completed)
- Simulated heap using a static array.
- Block metadata for allocation.
- Basic `malloc` and `free`.
- Block splitting when allocating.
- Memory map visualization.

## To Be Added (Remaining 60%)
- Coalescing adjacent free blocks.
- Allocation strategies (First Fit, Best Fit).
- Fragmentation handling.
- Thread safety with mutex locks.

## Build & Run
```bash
make
./allocator
