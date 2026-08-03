# cmalloc

The porpurse of this project is to understand the underlying concepts of a memory allocator and how one works under the hood. I'm writing this devlog to articulate what I'll learn by doing this project. The project will follow the article from Marwan Burelle, titled A Malloc Tutorial. My goal here is to learn this key concepts to apply them in the future in other projects, and also learn memory concepts like freeing, reallocating, anm allocating.

Here I have the definition of malloc and free in the words of Dennis Ritchie, author of The C Programming Language:
> Calls to `malloc` and `free` may occur in any order; malloc call upon the operating system to obtain more memory as necessary. [..] Rather than allocating from a compiled-in fixed-sized array, malloc will request space from the memory as needed. Since other activities in the program may also request space without calling this allocator, the space that `malloc` manages may not be contiguous. Thus its free storage is kept as a list of free blocks. Each block contains a size, a pointer to the next block, and the space itself. The blocks are kept in order of increasing storage adress, and the last block (highest address) points to the first.

Following the Burelle tutorial we can also estabilish some other key concepts:
* `malloc` should be tractable; `malloc` must terminate as soon as possible.
* The pointer returned by `malloc` points to an space where it can read or write without concerns.

In the article, Burelle explains we will use two main functions to do the memory allocation for us: `brk()` and `sbrk()`; where `brk()` will be responsible for setting the adress where the pointer should go in the memory heap, and `sbrk()` will allocate ou free (if the value is negative).

On the 2.3 section, Unmapped Region and No Man's Land, Burelle tells us about the limits of the allocation. What caught my attention was him saying that we don't really have a way to know where the next boundary of the page (or block) is, thus forcing us to go blindly. 

## Dummy allocator

The first implementation on the malloc we are building behaves just like a linear allocator: it justs takes the request memory, bumps and gives us the memory. The only way for us to free the memory is to free the whole block that was requested.

It fails because it doesn't gives us a way to reallocate the memory, and does not pick the best location for the memory to be allocated, it just goes blind and wastes a lot of memory.

## Adding address information to our memory allocator

As mentioned before, part of the fail of the dummy allocator is because it allocates the memory, returns to the past adress and terminates. To implement the concepts of malloc, free and realloc, we need to keep the information of where every allocated begins and ends, and follow in a linked-list-style to help us to know if we need to allocate more memory at the end or not.

## Notes

How can we know if the break is placed inside the page boundaries?
