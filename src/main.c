#include <stdio.h>
#include <stddef.h>
#include <sys/types.h>
#include <unistd.h>

#define align8(x) (((((x)-1)>>3)<<3)+8)

void *base = NULL;

typedef struct block *t_block;
struct block {
    size_t size;
    t_block next;
    t_block prev;
    int free;
    void *ptr;
    char data[1];
};

#define BLOCK_SIZE offsetof(struct block, data)

t_block find_block(t_block *last, size_t size) {
    t_block b = base;

    while(b && !(b->free && b->size >= size)) {
        *last = b;
        b = b->next;
    }

    return b;
}

t_block extend_heap(t_block last, size_t s) {
    t_block b;
    b = sbrk(0);

    if (sbrk(BLOCK_SIZE + s) == (void*)-1) { return NULL; }
    
    b->size = s;
    b->next = NULL;
    b->prev = last;
    b->ptr = b->data;

    if (last) { last->next = b; }

    b->free = 0;

    return b;
}

void split_block(t_block b, size_t s) {
    t_block new;
    new       = (t_block)(b->data + s);
    new->size = b->size - s - BLOCK_SIZE;
    new->next = b->next;
    new->prev = b;
    new->free = 1;
    new->ptr = new->data;
    b->size = s;
    b->next = new;

    if (new->next) { new->next->prev = new; }
}

void *malloc(size_t size) {
    t_block b, last;
    size_t s;
    s = align8(size);

    if (base) {
        last = base;
        b = find_block(&last, s);

        if (b) {
            if ((b->size -s) >= 
                (BLOCK_SIZE + 8)) { split_block(b, s); }

            b->free = 0;

        } else {
            b = extend_heap(last, s);

            if (!b) { return NULL; }
        }
    } else { 
        b = extend_heap(NULL, s);
        
        if (!b) { return NULL; }
        
        base = b;
    }

    return(b->data);
}

void *calloc(size_t number, size_t size) {
    char *p;
    size_t total, i;

    if (size != 0 && number > (size_t)-1 / size) { return NULL; }

    total = number * size;
    p = malloc(total);

    if (p) {
        for (i = 0; i < total; i++) { p[i] = 0; }
    }

    return p;
}

t_block fusion(t_block b) {
    if (b->next && b->next->free) {
        b->size += BLOCK_SIZE + b->next->size;
        b->next = b->next->next;

        if (b->next) { b->next->prev = b; }
    }

    return b;
}

t_block get_block(void *p) {
    char *tmp;
    tmp = p;
    tmp -= BLOCK_SIZE;

    return (t_block)tmp;
}

int validate_adress(void *p) {
    if (base) {
        if (p > base && p < sbrk(0)) { return (p == (get_block(p))->ptr); }
    }

    return 0;
}

void free(void *pointer) {
    t_block b;
    
    if (validate_adress(pointer)) {
        b = get_block(pointer);
        b-> free = 1;

        if (b->prev && b->prev->free) { b = fusion(b->prev); } 

        if (b->next) { fusion(b); }
        else {
            if (b->prev)
                b->prev->next = NULL;
            else
                base = NULL;
            brk(b);
        }
    }
}

void copy_block(t_block src, t_block dest) {
    int* src_data, *dest_data;
    size_t i;
    src_data = src->ptr;
    dest_data = dest->ptr;


    for (i = 0; i * 4 < src->size && i * 4 < dest->size; i++) {
        dest_data[i] = src_data[i];
    }
}

void* realloc(void *pointer, size_t size) {
    size_t s;
    t_block block, new;
    void *new_pointer;

    if (!pointer) { return (malloc(size)); }

    if (validate_adress(pointer)) {
        s = align8(size);
        block = get_block(pointer);

        if (block->size >= s) {
            if (block->size - s >= (BLOCK_SIZE + 8)) 
                split_block(block, s);
        } else {
            if (block->next && block->next->free
                && (block->size + BLOCK_SIZE +
                block->next->size) >= s) {

                fusion(block);

                if (block->size - s >= (BLOCK_SIZE + 8))
                    split_block(block, s);
            } else {
                new_pointer = malloc(s);

                if (!new_pointer) { return NULL; }

                new = get_block(new_pointer);
                copy_block(block, new);
                free(pointer);

                return new_pointer;
            }
        }
        return pointer;
    }
    return NULL;
}
