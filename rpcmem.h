#ifndef RPCMEM_H
#define RPCMEM_H

#include "debugmacros.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// clang-format off

// variable size based on addr_bits
#define RPCADDR_BITS 16

#define MAX_IDL_FNAME_LEN 80
#define MAX_PREFIX_LEN (MAX_IDL_FNAME_LEN + 40)

#include "rpcasm.h"

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
 **/
typedef struct {
  rpcptr_t SP;             // stack pointer
  rpcptr_t HP;             // heap pointer
  char DATA[MEM_CAPACITY]; // data segment 
} rpcmem_t;

// pre-allocate VM buffers
static char     global_transmit_buffer[MEM_CAPACITY + MAX_PREFIX_LEN] = {0};
static rpcmem_t global_rpc_memory_unit = {
  .SP = (rpcptr_t)MEM_CAPACITY - 1,
  .HP = 0,
  .DATA = {0},
};
static rpcmem_t *MEM = &global_rpc_memory_unit;

// clang-format on

// returns total length of buffer
static int rpcmem_tobuf(const char *fname, const rpcmem_t *MEM, char *outbuf) {
  // special format string to prevent buffer overflow attack
  // in the compressed buffer, stack starts at RHP
  int prefixlen = snprintf(outbuf, MAX_PREFIX_LEN,
                           "%" STRINGIFY(MAX_IDL_FNAME_LEN) "s %04x", fname, 0);
  assert(prefixlen < MAX_PREFIX_LEN);

  memcpy(outbuf + prefixlen, RDATA, RHP);
  memcpy(outbuf + prefixlen + RHP, RDATA + RSP, MEM_CAPACITY - RSP);

  int newprefixlen = snprintf(outbuf, MAX_PREFIX_LEN,
                              "%" STRINGIFY(MAX_IDL_FNAME_LEN) "s %04x", fname,
                              RHP + prefixlen);
  assert(prefixlen == newprefixlen);

  return prefixlen + RSP + (MEM_CAPACITY - RHP);
}

// returns function name
static void rpcmem_frombuf(const char *inbuf, int len, rpcmem_t *MEM,
                           char fname[MAX_IDL_FNAME_LEN]) {
  int stackptr, prefixlen;

  // special format string to prevent buffer overflow attack
  sscanf(inbuf, "%" STRINGIFY(MAX_IDL_FNAME_LEN) "s %04x%n", fname, &stackptr,
         &prefixlen);
  RSP = stackptr;
  memcpy(RDATA, inbuf + prefixlen, RSP);
  memcpy(RDATA + RHP, inbuf + prefixlen + RSP, MEM_CAPACITY - RHP);
}

// clang-format off

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
 * SBRK    | size          | rpcptr  | alloc on heap
 * STORE   | rpcptr, byte  |         | place on heap
 * LOAD    | rpcptr        | byte    | fetch from heap
 *
 * The basic PUSH and POP are defined on the rpc type U8. 
 * Syntatically, `PUSH(U8, <val>)` and `POP(U8, <var>)`. See function calls 
 * section for more info.
 *
 * Function Definitions
 * --------------------
 *   Users can define custom function as well.
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
 * PUSH(<type>, <c_exp>, ..., <c_exp>);
 *
 * To pop, provide a c variable to pop into, such that the c type of <c_id>
 * matches they type when the POPDEF was declared.
 *
 * POP(<type>, <c_id>);
 *
 * Or to immediately return the popped value,
 *
 * POP(<type>, return);
 *
 * To do it with arrays
 *
 * For example
 * int x[80][2][9];
 * int y[80][2][9];
 *
 * PUSH(INT, x, 80, 2, 9);
 * POP(INT, y, 80, 2, 9);
 *
 * assert(x[] == y[])
 * */

/*
 * Initial RPC Assembly Language Basis
 */

PUSHDEF(INT, int x) {
  for (int i = sizeof(x) - 1; i >= 0; i--) 
    PUSH(U8, (unsigned char)(x >> (8 * i)));
}

POPDEF(INT, int) {
  unsigned char c;
  int x = 0;
  for (int i = 0; i < sizeof(x); i++) {
    POP(U8, c);
    x |= c << (8 * i);
  }
  return x;
}

PUSHDEF(BOOL, bool b) { 
  PUSH(U8, (char)b);
}

POPDEF(BOOL, bool) {
  POP(U8, return);
}

PUSHDEF(FLOAT, float f) {
  PUSH(INT, AS(int, f));
}

POPDEF(FLOAT, float) {
  int i;
  POP(INT, i);
  return AS(float, i);
}

PUSHDEF(RPCPTR, rpcptr_t p) {
  for (int i = sizeof(p) - 1; i >= 0; i--)
    PUSH(U8, (unsigned char)(p >> (8 * i)));
}

POPDEF(RPCPTR, rpcptr_t) {
  unsigned char c;
  rpcptr_t p = 0;
  for (int i = 0; i < sizeof(p); i++) {
    POP(U8, c);
    p |= c << (8 * i);
  }
  return p;
}

// clang-format on

#endif
