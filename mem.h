#ifndef MEM_H
#define MEM_H

#include "common/debugmacros.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Initial capacity of the memory block, and if not defined elsewhere,
// it is set to a default value of 2^16 (65536)
#ifndef INITIAL_CAPACITY
#define INITIAL_CAPACITY (1 << 16)
#endif

// Definition of the pointer type used for RPC communication
typedef uint32_t __rpcptr_t;

struct __rpcmem_t {
    char *data;          // Pointer to the data block
    __rpcptr_t hp;       // Heap pointer
    __rpcptr_t sp;       // Stack pointer
    __rpcptr_t capacity; // Capacity of the memory block
};

// Function to create a new RPC memory block.
static __rpcmem_t *rpcmem_new() {
    __rpcmem_t *m = (__rpcmem_t *)malloc(sizeof(*m));
    assert(m);
    m->hp = 0;
    m->sp = m->capacity = INITIAL_CAPACITY;
    m->data = (char *)malloc(m->capacity);
    assert(m->data);
    return m;
}

// Function to free the memory associated with an RPC memory block.
static void rpcmem_free(__rpcmem_t **m) {
    free((*m)->data);
    free(*m);
    *m = NULL; // Set the pointer to NULL to avoid dangling pointers.
}

// Function to expand the capacity of an RPC memory block.
static void rpcmem_expand(__rpcmem_t *m) {
    int stacksize = m->capacity - m->sp;
    m->capacity *= 2;
    m->data = (char *)realloc(m->data, m->capacity);
    assert(m->data);
    // Move the stack to the end of data.
    memmove(m->data + m->capacity - stacksize, m->data + m->sp, stacksize);
    m->sp = m->capacity - stacksize; // Update the stack pointer.
}

#endif // MEM_H
