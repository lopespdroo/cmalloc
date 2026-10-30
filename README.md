# cmalloc

My own from-scratch implementation of `malloc`, `free`, `calloc` and `realloc` in C, built as a learning project following Marween Burelle's tutorial _A Malloc Tutorial_.

## About the project

The goal of making this project was not to replace the libc allocator, but to understand how an allocator works under the hood: how memory is requested from the operating system, how blocks are organized, and what happens when someone resize them. Although the goal was not to make a powerful memory allocator, I won't say I will ever work on this project again and make some improvements.

TL;DR: this is a learning project.

## Implemented functions

| Functions | What it does |
| ----- | --------------|
| `malloc(size)` | Allocates `size` bytes, reusing free blocks when possible |
| `calloc(number, size)` | Allocates `number * size` zeroed bytes, with overflow checking |
| `realloc(ptr, size)` | Resizes a block, trying to expand in place before moving it |
| `free(ptr)` | Frees a block, merges it with free neighbors and returns the end of the heap to the system | 

## How it works

The allocator uses `sbrk`, which moves the end of the process heap, and `brk`, which sets it to a specific adress. When no free block is large enough, the heap grows. When the last is freed, the heap shrinks.

You can learn much more about the project in the article I wrote describing my journey and what I learned doing this. You can check it out here: [go to documentation](docs/devlog.md).

## Requirements
- Linux, because the code uses `sbrk` and `brk`.
- `gcc` to compile the code
- 64-bit architecture

## Building and running

I wrote a test-case you can use, but feel free to test with any other program you want. The allocator has no `main` function, so it must be compiled together with a test program. Check [here](test.c) to see the test program I wrote or build and compile the project with your own.

### Limitations

Due to how C program works, you cannot use any function (`printf` or `scanf`) that has its own memory management, because they use `malloc` as the default memory allocator. That's why I've used `say` and `write` in `test.c`.

1. Compile
```bash
gcc -Wall -Wextra -g main.c test.c -o program
```

2. Run
```bash
./program
```

3. Expected output
```bash
hello, cmalloc!
calloc has been reduced to zero
ended normally
```
