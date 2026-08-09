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

We start the dummy allocator discovering where the top of the heap is with:
```c
p = sbrk(0)
```

And then, after that, we call the function to increment to the size we passed; if it fails, we return `NULL`.

## Adding address information to our memory allocator

As mentioned before, part of the fail of the dummy allocator is because it allocates the memory, returns to the past adress and terminates. To implement the concepts of malloc, free and realloc, we need to keep the information of where every allocated memory begins and ends, and follow in a linked-list-style to help us to know if we need to allocate more memory at the end or not.

To follow this structure in a linked-list way, we need to adress one of the topics we adressed earlier: "`malloc` should be tractable". To jump between the memory page, while tracking where we should go, we need to know the adress of each allocatec memory, the places where it is free and the size of it; to do all that we meta-datas!

The example given in the article perfect illustrates how we should conceptualize meta-data, but here's a brief summary: the meta-data is stored in the memory block together with the user-data, and its purpose is only to give us information of where we should go, the size of where we currently are and where is the freed memory.

### Linked lists in C and typedef's

To implement this structure where a point a points to point b, we indeed need to use the linked list as our data structure to hold up the user-data and the meta-data. There's few types of linked lists, but the one we'll implement is the singly linked list, where it will end with the last node pointing no `NULL`.

Example:
```
+---------+-----+       +---------+-----+
|meta-data|value| ----> |meta-data|value| ----> NULL
+---------+-----+       +---------+-----+
```
As every linked list, our node will contain the data, the pointer to the next node and an integer to tell us if the chunk we are seeing it is free or not.

To help us we also make use of typedef. In The C Programming Language we have:
> C provides a facily called `typedef` for creating new data type names.[...] It must be emphasized that a `typedef` declaration does not create a new type in any sense; it merely adds a new name for some existing type. [...] variables declared this way have exactly the same properties whose declaration are spelled explicitly. In effect `typedef` is like `#define`, except that since it is interpreted by the compiler, it can cope with textual substitutions that are beyond the capabilties of the preprocessor.

But if we stop to notice, the declaration of `typedef` in the article shows us that inside the struct of containing the nodes, we reference the node itself. So, why do we need that and why does it need to be declared with a pointer?

The answer to this question is embedded with memory concepts itself. If we declared our struct like
```c
struct block_size {
    size_t size;
    block_size;
    int free;
}
```
the compiler would throw us a warning saying that the declaration does not declare nothing. And this happens because the declaration of the struct ends in the `}`, and when we create an instance of its own structure inside of it, the compiler does not know how much memory it needs to allocate to create the struct. We solve this by implementing a pointer to the struct, forcing the compiler to work with the size of the pointer, that is known in compile-time.

Doing so will correctly reference the own struct inside of it, making the nodes point to each other with no problems. If you are questioning how does the struct allocates memory without a memory allocator, the articles explains this very well.

## Notes

How can we know if the break is placed inside the page boundaries?
