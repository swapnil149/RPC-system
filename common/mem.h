#ifndef MEM_H
#define MEM_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY (1 << 16)

typedef uint32_t __rpcptr_t;

struct __rpcmem_t {
  unsigned char *data;
  __rpcptr_t hp;       // heap ptr
  __rpcptr_t sp;       // stack ptr
  __rpcptr_t capacity; // internal use only
};

static __rpcmem_t *rpcmem_new() {
  __rpcmem_t *m = (__rpcmem_t *)malloc(sizeof(*m));
  m->hp = 0;
  m->sp = m->capacity = INITIAL_CAPACITY;
  m->data = (unsigned char *)malloc(m->capacity);
  return m;
}

static void rpcmem_free(__rpcmem_t **m) {
  free((*m)->data);
  free(*m);
  *m = NULL;
}

static void rpcmem_expand(__rpcmem_t *m) {
  int stacksize = m->capacity - m->sp;
  m->capacity *= 2;
  m->data = (unsigned char *)realloc(m->data, m->capacity);
  memmove(m->data + m->capacity - stacksize, m->data + m->sp, stacksize);
}

#endif
