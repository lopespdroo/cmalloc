#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#define align4() ((((x)-1)>>2)<<2)+4

typedef struct block_size* t_block;

struct block_size {
    size_t size;
    t_block next;
    int free;
};

t_block find_block(t_block* last, size_t size) {
    t_block b = base;

    while(b && !(b->free && b->size >= size)) {
        *last = b;
        b = b->next;
    }

    return (b);
}

#define BLOCK_SIZE sizeof(struct block_size)

t_block extend_heap(t_block last, size_t s) {
    t_block b;
    b = sbrk(0);

    if (sbrk(BLOCK_SIZE + s) == (void*)-1)
        return NULL;
    
    b->size = s;
    b->next = NULL;

    if (last)
        last->next = b;
    b->free = 0;

    return b;
}

