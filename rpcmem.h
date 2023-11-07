#ifndef RPCMEM_H
#define RPCMEM_H

// clang-format off

// variable size based on addr_bits
#include <stdio.h>
#define RPCADDR_BITS 16

#include "rpcasm.h"
#include <stdbool.h>
#include <stdint.h>

typedef RPCPTR_INTERNAL_TYPE rpcptr_t;

/*
 * RPC VIRTUAL MEMORY UNIT
 *
 * Internally, it does this by tracking a stack pointer, heap pointer, 
 * heap offset (used when compressing memory for transfer) and memory.  
 *
 * Using the RPC Assembly language these ops can be composed at a higher level
 **/
typedef struct {
  rpcptr_t SP;       // stack pointer
  rpcptr_t HP;       // heap pointer
  char *DATA;
} rpcmem_t;

rpcmem_t *rpcmem_new();
void rpcmem_free(rpcmem_t **mem_pp);

// returns total length of buffer
int rpcmem_tobuf(const char *fname, const rpcmem_t *MEM, void **outbuf);

// returns function name
char *rpcmem_frombuf(const void *inbuf, int len, rpcmem_t *MEM);

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

#include <stdio.h>

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
