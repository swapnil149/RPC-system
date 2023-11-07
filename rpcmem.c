#include "rpcmem.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpphacking.h"

#define BUFFERLOAD(buf)

#if RPCADDR_BITS > 16
#define FMTADDR "%08x"
#else
#define FMTADDR "%04x"
#endif

#define FMT_METADATA ("FN: %s HP:" FMTADDR " SP:" FMTADDR " ")

#if RPCADDR_BITS > 16
#define SIZEOF_FMT_MEMPTRS (sizeof(FMT_MEMPTRS) - (sizeof(FMTADDR) * 2) + 8 * 2)
#else
#define SIZEOF_FMT_MEMPTRS (sizeof(FMT_MEMPTRS) - (sizeof(FMTADDR) * 2) + 4 * 2)
#endif

rpcmem_t *rpcmem_new() {
  rpcmem_t *m = (rpcmem_t *)malloc(sizeof(*m));
  m->DATA = malloc(MEM_CAPACITY);
  m->SP = 0;
  m->HP = (rpcptr_t)MEM_CAPACITY;
  return m;
}

void rpcmem_free(rpcmem_t **mem_pp) {
  free((*mem_pp)->DATA);
  free(*mem_pp);
  *mem_pp = NULL;
}

// returns total length of buffer
int rpcmem_tobuf(const char *fname, const rpcmem_t *MEM, void **outbuf) {
  // int metadata_len = sprintf(, const char *, ...);
  return -1;
}

// returns function name
char *rpcmem_frombuf(const void *inbuf, int len, rpcmem_t *MEM) { return NULL; }
