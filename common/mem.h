#ifndef MEM_H
#define MEM_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY (1 << 16)

//                 "<fname> <stackptr>"
#define PREFIX_FMT "%s %08x"

#define MAX_FNAME_LEN 80
#define MAX_PREFIX_LEN (MAX_FNAME_LEN + 20) // rough estimate

typedef uint32_t rpcptr_t;

struct rpcmem_t {
  unsigned char *data;
  rpcptr_t hp;
  rpcptr_t sp;
  rpcptr_t capacity; // internal use only
};

static rpcmem_t *rpcmem_new() {
  rpcmem_t *m = (rpcmem_t *)malloc(sizeof(*m));
  m->hp = 0;
  m->sp = m->capacity = INITIAL_CAPACITY;
  m->data = (unsigned char *)malloc(m->capacity);
  return m;
}

static void rpcmem_free(rpcmem_t **m) {
  free((*m)->data);
  free(*m);
  *m = NULL;
}

// clang-format off

static void rpcmem_expand(rpcmem_t *m) { 
  int stacksize = m->capacity - m->sp;
  m->capacity *= 2;
  m->data = (unsigned char*)realloc(m->data, m->capacity);
  memcpy(m->data + m->capacity - stacksize, m->data + m->sp, stacksize);
}

static int rpcmem_tobuf(const char *fname, const rpcmem_t *m, char **outbuf) {
  int prefixlen = snprintf(NULL, 0, PREFIX_FMT, fname, m->hp);
  assert(prefixlen < MAX_PREFIX_LEN && prefixlen);
  *outbuf = (char *)malloc(prefixlen + m->hp + (m->capacity - m->sp));
  snprintf(*outbuf, MAX_PREFIX_LEN, PREFIX_FMT, fname, m->hp);
  memcpy(*outbuf + prefixlen,         m->data,         m->hp);
  memcpy(*outbuf + prefixlen + m->hp, m->data + m->sp, m->capacity - m->sp);
  return prefixlen + m->hp + (m->capacity - m->sp);
}

// clang-format on

static void rpcmem_frombuf(const char *inbuf, int len,
                           char fname[MAX_PREFIX_LEN], rpcmem_t *m) {
  int stackptr, prefixlen;
  sscanf(inbuf, PREFIX_FMT "%n", fname, &stackptr, &prefixlen);
  m->sp = stackptr;
  m->hp = 0;
  memcpy(m->data, inbuf + prefixlen, len);
}

#undef PREFIX_FMT
#undef MAX_PREFIX_LEN

#endif
