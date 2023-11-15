#ifndef RPCMEM_H
#define RPCMEM_H

// #define NOLIB

#ifndef NOLIB
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#include "debugmacros.h"
#include "naughtycpp.h"

// clang-format off

// variable size based on addr_bits
#define RPCADDR_BITS 16

#include "rpcasm.h"

#define MAX_IDL_FNAME_LEN 80
#define MAX_PREFIX_LEN (MAX_IDL_FNAME_LEN + 40)

typedef RPCPTR_TYPE rpcptr_t;

/*
 * RPC VIRTUAL MEMORY UNIT
 *
 * Internally, it does this by tracking a stack pointer, heap pointer, 
 * heap offset (used when compressing memory for transfer) and memory.  
 *
 * Using the RPC Assembly language these ops can be composed at a higher level
 *
 * MEM_CAPACITY is the size of the data segment. It is defined by "rpcasm.h".
 *
 * Stack starts at MEM_CAPACITY and grows down
 * Heap starts at 0 and grows up
 *
 * Like assembly code with global registers, operations on memunit typically
 * use global registers in syntax
 *
 * RSP is Stack pointer register  == MEM->SP
 * RHP is Heap pointer register   == MEM->HP
 * RDATA is Data segment register == MEM->DATA
 *
 * It becomes more natural when writing PUSHDEF and POPDEF functions,
 * explained below.
 **/
typedef struct {
  rpcptr_t SP; // stack pointer (grows down)
  rpcptr_t HP; // heap pointer  (grows up)
  char *DATA;  // data segment 
} rpcmem_t;

static rpcmem_t *rpcmem_new() {
  rpcmem_t *MEM = (rpcmem_t*) malloc(sizeof(*MEM));
  RSP = MEM_CAPACITY - 1;
  RHP = 0;
  RDATA = (char*)malloc(MEM_CAPACITY);
  memset(RDATA, 0, MEM_CAPACITY);
  return MEM;
}

static void rpcmem_free(rpcmem_t **mem_pp) {
  rpcmem_t *MEM = *mem_pp;
  free(RDATA);
  free(MEM);
  *mem_pp = NULL;
}

// TODO: decide if we want it and implement
static void rpcmem_expand(rpcmem_t *MEM) {
  (void)MEM;
}

#define PREFIX_FMT "%s %08x"

// returns total length of buffer
static int rpcmem_tobuf(const char *fname, const rpcmem_t *MEM, char **outbuf) {
  // calculate prefix length (contains fname and RSP value)
  int prefixlen = snprintf(NULL, 0, PREFIX_FMT, fname, RHP);
  assert(prefixlen < MAX_PREFIX_LEN && prefixlen);
  *outbuf = (char *)malloc(prefixlen + RHP + (MEM_CAPACITY - RSP));
  snprintf(*outbuf, MAX_PREFIX_LEN, PREFIX_FMT, fname, RHP);
  memcpy(*outbuf + prefixlen, RDATA, RHP);
  memcpy(*outbuf + prefixlen + RHP, RDATA + RSP, MEM_CAPACITY - RSP);
  return prefixlen + RHP + (MEM_CAPACITY - RSP);
}

// returns function name
static void rpcmem_frombuf(const char *inbuf, int len, rpcmem_t *MEM,
                           char fname[MAX_IDL_FNAME_LEN]) {
  int stackptr, prefixlen;
  sscanf(inbuf, PREFIX_FMT "%n", fname, &stackptr, &prefixlen);
  RSP = stackptr;
  RHP = 0;
  memcpy(RDATA, inbuf + prefixlen, len);
}


#undef PREFIX_FMT

/*
 * RPC Assembly Embedded Language, implemented with C preprocessor
 * ---------------------------------------------------------------
 * Implicitly requires an in-scope `rpcmem_t *mem` to call from normal c
 *
 * RPC Stack Language reference:
 *
 * Operations
 * ----------
 * 5 primitive operations, these are like the ISA of the RPC Virtual Mem Unit
 *
 * OP      | INPUT         | RETURNS | PURPOSE
 * ----------------------------------------------
 * PUSHU8  | byte          |         | add to stack
 * POPU8   |               | byte    | pop off stack
 * SBRK    | size          | rpcptr  | alloc `size` bytes on heap
 * STORE   | rpcptr, byte  |         | place byte on heap
 * LOAD    | rpcptr        | byte    | fetch byte from heap
 *
 * The basic PUSH and POP are defined on the rpc type U8. 
 * Syntatically, `PUSH(U8, <val>)` and `POP(U8, <var>)`. See function calls 
 * section for more info.
 *
 * Function Definitions
 * --------------------
 *   Users can define custom functions as well.
 *
 *   Functions can either pop off the memory unit or push onto
 *   the memory unit. This is distinguished by a POP or PUSH call.
 *   Somewhat confusingly, the PUSH and POP calls may interact
 *   with both the stack and the heap.
 *
 * To define a push function
 * PUSHDEF(<type>, <c_type> <id>, ..., <c_type> <id>) {
 *   <body>
 * }
 *
 * To define a pop function, which never has arguments, but specifies a 
 * return type.
 * POPDEF(<type>, <c_type>) {
 *   <body>
 * }
 *
 * Function Calls
 * --------------
 *
 * To call use, make sure the c type of the <c_exp> matches the parameters
 * defined before.
 * PUSH(<type>, <c_exp>);
 *
 * e.g.
 * int c = 8;
 * PUSH(INT, c); // OR 
 * PUSH(INT, 8);
 *
 * To pop, provide a c variable to pop into. The c type of <c_id>
 * matches the type given when the POPDEF was declared.
 *
 * POP(<type>, <c_id>);
 *
 * e.g.
 * int c;
 * POP(INT, c);
 *
 * Or to immediately return the popped value,
 *
 * POP(<type>, return);
 *
 * e.g.
 * POP(INT, return);
 *
 * To push and pop contiguous arrays add the dimensions after the values
 *
 * For example (in C-like pseudocode):
 * int x[80][2][9] = {0};
 * int y[80][2][9] = {-1};
 *
 * PUSH(INT, x, 80, 2, 9); // real RPC asm
 * POP(INT, y, 80, 2, 9);
 *
 * assert(x[] == y[])
 * */

/*
 * Initial RPC Assembly Language Basis
 */

PUSHDEF(BOOL, bool b) {
  PUSH(U8, (char)b);
}

POPDEF(BOOL, bool) {
  POP(U8, return);
}

PUSHDEF(RPCPTR, rpcptr_t p) {
  unsigned char cs[] = {
    (unsigned char)p,
    (unsigned char)(p >> 8),
  };
  PUSH(U8, cs, 2);
}

POPDEF(RPCPTR, rpcptr_t) {
  unsigned char cs[2];
  POP(U8, cs, 2);
  return cs[0] | cs[1] << 8;
}

PUSHDEF(INT, int x) {
  unsigned char cs[] = {
    (unsigned char)x,
    (unsigned char)(x >> 8),
    (unsigned char)(x >> 16),
    (unsigned char)(x >> 24),
  };
  PUSH(U8, cs, 4);
}

POPDEF(INT, int) {
  unsigned char cs[4];
  POP(U8, cs, 4);
  return cs[0] | cs[1] << 8 | cs[2] << 16 | cs[3] << 24;
}

PUSHDEF(FLOAT, float f) {
  PUSH(INT, AS(unsigned, f));
}

POPDEF(FLOAT, float) {
  unsigned i;
  POP(INT, i);
  return AS(float, i);
}

// clang-format on

#endif
