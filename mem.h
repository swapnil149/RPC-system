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
    __rpcptr_t hp;        // Heap pointer
    __rpcptr_t sp;        // Stack pointer
    __rpcptr_t capacity;  // Capacity of the memory block
};

// Function to create a new RPC memory block.
static __rpcmem_t *rpcmem_new() {
    __rpcmem_t *m = (__rpcmem_t *)malloc(sizeof(*m));  // Allocate memory for the structure.
    assert(m);  // Ensure that the allocation was successful. TODO: Do we need assert statements?
    m->hp = 0;
    m->sp = m->capacity = INITIAL_CAPACITY;
    m->data = (char *)malloc(m->capacity);  // Allocate memory for the data block.
    assert(m->data);  // Ensure that the allocation was successful. TODO: Do we need assert statements?
    return m;
}

// Function to free the memory associated with an RPC memory block.
static void rpcmem_free(__rpcmem_t **m) {
    free((*m)->data);  // Free the data block.
    free(*m);          // Free the structure.
    *m = NULL;         // Set the pointer to NULL to avoid dangling pointers.
}

// Function to expand the capacity of an RPC memory block.
static void rpcmem_expand(__rpcmem_t *m) {
    int stacksize = m->capacity - m->sp;  // Calculate the size of the stack.
    m->capacity *= 2;  // Double the capacity.
    m->data = (char *)realloc(m->data, m->capacity);  // Reallocate memory for the expanded data block.
    memmove(m->data + m->capacity - stacksize, m->data + m->sp, stacksize);  // Move the stack to the end.
    m->sp = m->capacity - stacksize;  // Update the stack pointer.
    
    // Debug messages providing information about the expansion. TODO, I think we don't need them
    DEBUG("EXPANDED: %d", m->capacity);
    DEBUG("new sp: %d", m->sp);
    DEBUG("new hp: %d", m->hp);
}

#endif // MEM_H
