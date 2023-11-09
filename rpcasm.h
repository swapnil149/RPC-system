#ifndef RPCASM_H
#define RPCASM_H

/*
 * Implementation of the embedded RPC ASM language
 *
 * It is built using some relatively niche C preprocessor hacks
 *
 * To use embedded RPC ASM, make sure an `rpcmem_t*` called `MEM`
 * is in scope
 *
 * See rpcmem.h for language reference and tutorial
 */

/* These are the "exported" language features (a.k.a. macros).
 * All others are TOTALLY OFF LIMITS!
 *
 * MEM          : name of the memory unit
 * MEM_CAPACITY : capacity of the address space
 * RPCPTR_TYPE  : equivalent c type for the rpcptr
 *
 * RSP          : stack pointer register
 * RHP          : heap pointer register
 * RDATA        : address space
 *
 * PUSH(...)    : add data into mem unit, updates RSP
 * POP(...)     : remove data from mem unit, updates RSP
 *
 * SBRK(n)      : low-level alloc heap space on mem unit
 * STORE(...)   : arbitrary memory store
 * LOAD(...)    : arbitrary memory load
 *
 * PUSHDEF(...) : create a new procedure to add data type
 * POPDEF(...)  : create a new procedure to remove data type
 */

/* See naughtycpp.h for more nitty gritty preprocessor hacking */
#include "naughtycpp.h"
/* From it, we use the following magic macros
 * `CAT(a, b)`                       ==> `ab`
 *
 * `FOO(x)`                          ==> `hello x! ` // as an example
 *
 * `EVAL(MAP(FOO, x1))`              ==> `hello x1! `
 * `EVAL(MAP(FOO, x1, x2))`          ==> `hello x1! hello x2! `
 * `EVAL(MAP(FOO, x1, x2, x3, ...))` ==> `hello x1! hello x2! hello x3! ...`
 *
 * `NOT(0)`                          ==> `1`
 * `NOT(_)`                          ==> `0`
 *
 * `IF_RETURN(return)(tr)(fl)`       ==> `tr`
 * `IF_RETURN(_)(tr)(fl)`            ==> `fl`
 *
 * `IF_ELSE(0)(tr)(fl)`              ==> `fl`
 * `IF_ELSE(_)(tr)(fl)`              ==> `tr`
 *
 * `HAS_ARGS()`                      ==> `0`
 * `HAS_ARGS(_)`                     ==> `1`
 * `HAS_ARGS(_, ...)`                ==> `1`
 *
 * `NUM_ARGS(a)`                     ==> `1`
 * `NUM_ARGS(a, b, c, d, e, f, g)`   ==> `7`
 * `NUM_ARGS(x1, .., x63)`           ==> `63`
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

/* Info that is dependent on bit address size */
#if RPCADDR_BITS > 16
#define MEM_CAPACITY (1U << 20) // 1mb
#else
#define MEM_CAPACITY (1U << RPCADDR_BITS) // 64kb
#endif

#if RPCADDR_BITS == 64
#define RPCPTR_TYPE unsigned long
#elif RPCADDR_BITS == 32
#define RPCPTR_TYPE unsigned int
#elif RPCADDR_BITS == 16
#define RPCPTR_TYPE unsigned short
#endif

// clang-format off

/* Virtual Hardware Registers */
#define RSP       MEM->SP
#define RHP       MEM->HP
#define RDATA     MEM->DATA

/* Stack ISA */
#define PUSH_U8(byte, ...) RDATA[RSP--] = (char)(byte)
#define POP_U8(...)        RDATA[++RSP]

/* Heap ISA */
#define SBRK(n)                 (RHP = RHP + (n), RHP - (n))
#define STORE(byte, rpc_addr)   RDATA[(rpc_addr)] = (byte)
#define LOAD(rpc_addr, cpp_var) (cpp_var) = RDATA[(rpc_addr)]

// clang-format on

/*
 * Utils
 */

/* reinterpret bits (not a cast) */
#define AS(ctype, var) (*(ctype *)&var)
/*     `AS(float, myint)` ==> `(*(float *)&myint)`
 */

/*
 * Define A Custom Routine (defers to a C function internally)
 */

#define PUSHDEF(type, ...)                                                     \
  static inline void CAT(PUSH_, type)(__VA_ARGS__, rpcmem_t * MEM)
/*     `PUSHDEF(ALT_INT, int a) {
 *        ...
 *     }` ==>
 *     `static inline void PUSH_ALT_INT(int a, rpcmem_t *MEM) {
 *        ...
 *     }`
 */

#define POPDEF(type, returntype)                                               \
  static inline returntype CAT(POP_, type)(rpcmem_t * MEM)
/*     `POPDEF(ALT_INT, int) {
 *        int i;
 *        ...
 *        return i;
 *     }` ==>
 *     `static inline int POP_ALT_INT(rpcmem_t *MEM) {
 *        int i;
 *        ...
 *        return i;
 *     }`
 */

/* Calling Routines */

/* These eventually turn into PUSH__ONE or PUSH__ARRAY */
#define PUSH(type, val, ...) _PUSH(type, val, __VA_ARGS__)
/*     `PUSH(INT, x)`       ==> `PUSH_INT(x, MEM)`
 *     `PUSH(INT, x, 3)`    ==> `for (int _i = 0; _i < 3; _i++) {
 *                                  PUSH_INT(x[_i], MEM);
 *                               }`
 *     `PUSH(INT, x, 3, 4)` ==> `for (int _i = 0; _i < (3 * 4); _i++) {
 *                                  PUSH_INT(x[0][_i], MEM);
 *                               }`
 */

#define POP(type, var, ...) _POP(type, var, __VA_ARGS__)
/*     `POP(FLOAT, return)`  ==> `return POP_FLOAT(MEM)`
 *     `POP(FLOAT, x)`       ==> `x = POP_FLOAT(MEM)`
 *     `POP(FLOAT, x, 3)`    ==> `for (int _i = 0; _i < 3; _i++) {
 *                                  x[_i] = POP_FLOAT(MEM);
 *                                }`
 *     `POP(FLOAT, x, 3, 4)` ==> `for (int _i = 0; _i < (3 * 4); _i++) {
 *                                  x[0][_i] = POP_FLOAT(MEM);
 *                                }`
 */

/*
 * Now we get into the work behind PUSH and POP, really not too bad
 */

#define IF_NOARGS(...) IF_ELSE(NOT(HAS_ARGS(__VA_ARGS__)))
/*     `IF_NOARGS()(y)(n)`       ==> `y`
 *     `IF_NOARGS(a)(y)(n)`      ==> `n`
 *     `IF_NOARGS(a, ...)(y)(n)` ==> `n`
 */

#define _PUSH(t, v, ...)                                                       \
  IF_NOARGS(__VA_ARGS__)                                                       \
  (PUSH__ONE(t, v))(PUSH__ARRAY(t, v, __VA_ARGS__))

#define _POP(t, v, ...)                                                        \
  IF_NOARGS(__VA_ARGS__)                                                       \
  (POP__ONE(t, v))(POP__ARRAY(t, v, __VA_ARGS__))

/* Single Case (see PUSH/POP for example) */
#define PUSH__ONE(type, a) CAT(PUSH_, type)(a, MEM)
#define POP__ONE(type, a)                                                      \
  IF_RETURN(a)                                                                 \
  (return CAT(POP_, type)(MEM))(a = CAT(POP_, type)(MEM))

/* Array Case (see PUSH/POP for example) */
#define PUSH__ARRAY(type, a, ...)                                              \
  for (int i = 0; i < SUM(__VA_ARGS__); i++) {                                 \
    PUSH__ONE(type, MULTIINDEX(a, i, __VA_ARGS__));                            \
  }
#define POP__ARRAY(type, a, ...)                                               \
  for (int i = SUM(__VA_ARGS__) - 1; i >= 0; i--) {                            \
    POP__ONE(type, MULTIINDEX(a, i, __VA_ARGS__));                             \
  }

/* Sum all arguments (at runtime) */
#define SUM(a, ...) ((a)EVAL(MAP(_MULT_LHS, __VA_ARGS__)))
/*     `SUM(a)`       ==> `(a)`
 *     `SUM(a, b)`    ==> `(a * b)`
 *     `SUM(a, b, c)` ==> `(a * b * c)`
 */

#define _MULT_LHS(x) IF_ELSE(HAS_ARGS(x)) (*(x))()
/*     `_MULT_LHS(x)` ==> `* x`
 *     `_MULT_LHS()`  ==> ``
 */

#define MULTIINDEX(x, i, ...) x _INDEXERS_(__VA_ARGS__)[i]
/*     `MULTIINDEX(x, i)`             ==> `x[i]`
 *     `MULTIINDEX(x, i, 1)`          ==> `x[0][i]`
 *     `MULTIINDEX(x, i, 1, 2, 3, 4)` ==> `x[0][0][0][i]`
 */

#define _INDEXERS_(_, ...) EVAL(MAP(ZEROINDEX, __VA_ARGS__))
/*     `_INDEXERS_(a)`       ==> ``
 *     `_INDEXERS_(a, b)`    ==> `[0]`
 *     `_INDEXERS_(a, b, c)` ==> `[0][0]`
 */

#define ZEROINDEX(x) IF_NOARGS(x)()([0])
/*     `ZEROINDEX(x)` ==> `[0]`
 *     `ZEROINDEX()`  ==> ``
 */

#endif
