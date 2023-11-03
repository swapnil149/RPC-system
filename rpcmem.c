#include "rpcmem.h"
#include <stdlib.h>

rpcmem_t *rpcmem_new() {
  rpcmem_t *m = (rpcmem_t *)malloc(sizeof(*m));
  m->DATA = malloc(MEM_CAPACITY);
  m->SP = 0;
  m->HP = (rpcptr_t)MEM_CAPACITY;
  m->HPOFFSET = 0;
  return m;
}

void rpcmem_free(rpcmem_t **mem_pp) {
  free((*mem_pp)->DATA);
  free(*mem_pp);
  *mem_pp = NULL;
}

int rpcmem_tobuf(const rpcmem_t *mem, const void *outbuf) { return 0; }
rpcmem_t *rpcmem_frombuf(void *inbuf, int len) { return rpcmem_new(); }
