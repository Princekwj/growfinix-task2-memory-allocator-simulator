🧠 Memory Allocator Simulator (C)

A custom implementation of malloc(), free(), and realloc() built in pure C — simulating how dynamic memory allocation works internally in operating systems.






📘 Overview

This project simulates a low-level heap memory allocator — mimicking the behavior of the standard C library’s malloc() and free().
It demonstrates key system-level concepts like:

Heap management

Block metadata

Fragmentation control

Coalescing of free blocks

Allocation strategies (First Fit / Best Fit)

Thread-safe memory operations using mutex locks

🚀 Features

✅ Simulated 64 KB heap
✅ Custom my_malloc, my_free, and my_realloc
✅ Block splitting and coalescing
✅ First Fit and Best Fit allocation strategies
✅ Thread-safe using pthread_mutex_t
✅ Alignment to 8 bytes
✅ Double-free detection & memory corruption checks
✅ Memory map & allocator statistics display

🗂️ Project Structure
memory-allocator-simulator/
│── include/
│   └── allocator.h          # Function declarations & types
│── src/
│   ├── allocator.c          # Core allocator logic (100%)
│   └── main.c               # Demonstration / test driver
│── Makefile                 # Build automation
│── README.md                # Project documentation

🧩 Build & Run
🔧 Requirements

GCC or Clang

POSIX threads (pthread)

Linux / macOS terminal (or WSL on Windows)

💻 Commands
# Clone repository
git clone https://github.com/YOUR-USERNAME/memory-allocator-simulator.git
cd memory-allocator-simulator

# Build
make

# Run
./allocator

🧠 Example Output
=== Memory Allocator Simulator (Full) ===
Using strategy: FIRST FIT

Initial state:
---------- Memory Map ----------
Block  0 | addr=0x55b2a5e00000 | size= 65504 | FREE
--------------------------------
Allocator Stats:
Total heap: 65536 bytes
Used     : 0 bytes in 0 block(s)
Free     : 65504 bytes in 1 block(s)

After allocating 1000 bytes (p1):
Block  0 | addr=0x55b2a5e00000 | size= 1000 | USED
Block  1 | addr=0x55b2a5e00430 | size= 64400 | FREE
--------------------------------

⚙️ Allocation Strategies
Strategy	Description
First Fit	Allocates the first free block large enough to hold the request.
Best Fit	Scans all free blocks and allocates the smallest possible one that fits.

You can change strategy in code:

set_alloc_strategy(STRATEGY_BEST_FIT);

📊 Functions Implemented
Function	Purpose
init_heap()	Initializes the simulated heap.
my_malloc(size)	Allocates memory block.
my_free(ptr)	Frees memory, merges adjacent free blocks.
my_realloc(ptr, new_size)	Resizes an existing block.
print_memory_map()	Displays current heap block layout.
print_allocator_stats()	Shows memory usage and fragmentation info.
🧵 Thread Safety

All allocator operations are wrapped in a global mutex for safe concurrent usage in multi-threaded programs.

🧰 Tech Stack

Languages: C
Libraries: pthread.h, stdio.h, stdlib.h
Concepts: Pointers, Memory Management, Linked Lists, Mutex Locks, Algorithms

🧑‍💻 Author

[Your Name Here]
C & C++ Development Intern @ Growfinix
🔗 LinkedIn
 | GitHub

🪪 License

This project is licensed under the MIT License — free to use and modify with attribution.

🌟 Inspiration

Developed during my C & C++ Development Internship at Growfinix, focusing on system-level programming and memory management.