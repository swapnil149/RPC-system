#include "rpcmem.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  rpcmem_t *MEM = (rpcmem_t *)malloc(sizeof(*MEM));
  RDATA = malloc(MEM_CAPACITY);
  RSP = 0;
  RHP = (rpcptr_t)MEM_CAPACITY;
  memset(RDATA, 0, MEM_CAPACITY);
  return MEM;
}

void rpcmem_free(rpcmem_t **mem_pp) {
  rpcmem_t *MEM = *mem_pp;
  free(RDATA);
  free(*mem_pp);
  *mem_pp = NULL;
}

#define MAX_PREFIX_LEN (MAX_IDL_FNAME_LEN + 40)

// returns total length of buffer
//
// Buffer looks like this
int rpcmem_tobuf(const char *fname, const rpcmem_t *MEM, void **outbuf_pp) {
  char prefix[MAX_PREFIX_LEN];
  int prefixlen = sprintf(prefix, "%s %d %d", fname, RSP, RHP) + 1;
  int heapsize = MEM_CAPACITY - RHP;
  int buflen = prefixlen + RSP + heapsize;
  void *outbuf = malloc(buflen);
  memcpy(outbuf, prefix, prefixlen);
  memcpy(outbuf + prefixlen, RDATA, RSP);
  memcpy(outbuf + prefixlen + RSP, RDATA + RHP, heapsize);
  *outbuf_pp = outbuf;
  return buflen;
}

// returns function name
void rpcmem_frombuf(const void *inbuf, rpcmem_t *MEM,
                    char fname[MAX_IDL_FNAME_LEN]) {
  int stackptr, heapptr, prefixlen;
  sscanf(inbuf, "%s %d %d%n", fname, &stackptr, &heapptr, &prefixlen);
  RSP = stackptr;
  RHP = heapptr;
  prefixlen++; // THIS IS IMPORTANT AND SUBTLE
  memcpy(RDATA, inbuf + prefixlen, RSP);
  memcpy(RDATA + RHP, inbuf + prefixlen + RSP, MEM_CAPACITY - RHP);
}
