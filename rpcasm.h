#ifndef RPCASM_H
#define RPCASM_H

// Hiding the ugly...
//
// Recommended reading:
// http://jhnet.co.uk/articles/cpp_magic
//
#include "cpphacking.h"

/*
 * Implementation of the embedded RPC ASM language
 *
 * It is built using evil C preprocessor hacks
 *
 * To call, make sure an rpcmem_t* called MEM is in scope
 *
 * See rpcmem.h for language reference and tutorial
 */

#ifndef RPCADDR_BITS
#define RPCADDR_BITS 16
#endif

/* Name of the implicit memory unit */
#define MEM __mem

/* Names of the memory unit fields */
#define SP __sp     // stack pointer
#define HP __hp     // heap pointer
#define DATA __data // memory

/* Stuff dependent on bit address size */

/* make the type like printf("uint%d_t", addr_bits) */
#define RPCPTR_INTERNAL_TYPE CAT3(uint, RPCADDR_BITS, _t)

#if RPCADDR_BITS > 16
#define MEM_CAPACITY (1U << 20) // 1mb
#else
#define MEM_CAPACITY (1U << RPCADDR_BITS) // 64kb
#endif

// clang-format off

/* VIRTUAL HARDWARE REGISTERS */
#define RSP       MEM->SP
#define RHP       MEM->HP
#define RDATA     MEM->DATA

/* STACK ISA */
#define PUSH_U8(byte, ...) RDATA[RSP++] = (char)(byte)
#define POP_U8(...)        RDATA[--RSP]

/* HEAP ISA */
#define SBRK(n)                 (RHP = RHP - (n), RHP)
#define STORE(byte, rpc_addr)   RDATA[(rpc_addr)] = (byte)
#define LOAD(rpc_addr, cpp_var) (cpp_var) = RDATA[(rpc_addr)]

/* DEFINE A CUSTOM ROUTINE */ 

#define PUSHDEF(type, ...)  \
  extern inline void CAT(PUSH_,type)(__VA_ARGS__, rpcmem_t *MEM)
#define POPDEF(type, rtcty) \
  extern inline rtcty CAT(POP_,type)(rpcmem_t *MEM)

/* CALLING ROUTINES */

/* These eventually turn into PUSH__ONE or PUSH__ARRAY */
#define PUSH(type, ...) _PUSH_(type, NUM_ARGS(__VA_ARGS__), __VA_ARGS__)
#define POP(type, ...) _POP_(type, NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

/* SINGLE CASE */
#define PUSH__ONE(type, a) CAT(PUSH_, type)(a, MEM)
#define POP__ONE(type, a)          \
  IF_RETURN(a)                     \
    (return CAT(POP_, type) (MEM)) \
     (a = CAT(POP_, type) (MEM))

/* ARRAY CASE */
#define POP__ARRAY(type, a, ...)    \
  for (int i = SUM(__VA_ARGS__); i > 0; i--) { \
    POP__ONE(type, MULTIINDEX(a, i - 1, __VA_ARGS__));     \
  } 

#define PUSH__ARRAY(type, a, ...)                   \
  for (int i = 0; i < SUM(__VA_ARGS__); i++) {      \
    PUSH__ONE(type, MULTIINDEX(a, i, __VA_ARGS__)); \
  }

#define SUM(a, ...) ((a) EVAL(MAP(_MULTLAST, __VA_ARGS__)))
#define _MULTLAST(x) IF_ELSE(HAS_ARGS(x))(* (x))()

// for nd array `x` index with `x[0]...[0][i]` where all indexes < n are 0
#define MULTIINDEX(x, i, ...) x _INDEXERS_(__VA_ARGS__)[i]
#define ZEROINDEX(x) IF_ELSE(HAS_ARGS(x))([0])()
#define _INDEXERS_(_, ...) EVAL(MAP(ZEROINDEX, __VA_ARGS__))

/* HELPFUL STUFF */

/* reinterpret bits (not a cast) */
#define AS(ctype, var) (*(ctype*)&var)

// converts between PUSH__ARRAY and PUSH__ONE depending on argcount
#define _PUSH_(type, count, ...) CAT(COUNT_, count)(PUSH, type, __VA_ARGS__)
#define _POP_(type, count, ...) CAT(COUNT_, count)(POP, type, __VA_ARGS__)

// clang-format on
#endif
