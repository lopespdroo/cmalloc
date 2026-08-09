#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct block_size* t_block;

struct block_size {
    size_t size;
    t_block next;
    int free;
};

