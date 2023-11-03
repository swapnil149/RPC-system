#ifndef RPCASM_H
#define RPCASM_H

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

/* name of the implicit memory unit */
#define MEM __mem

/* names of the memory unit fields */
#define SP __sp             // stack pointer
#define HP __hp             // heap pointer
#define HPOFFSET __hpoffset // heap offset
#define DATA __data         // memory

/* uint\d_t depends on RPCADDR_BITS */

#define RPCPTR_INTERNAL_TYPE CAT3(uint, RPCADDR_BITS, _t)
#if RPCADDR_BITS > 16
#define MEM_CAPACITY (1U << 20) // 1mb
#else
#define MEM_CAPACITY (1U << RPCADDR_BITS) // 64kb
#endif

// clang-format off

#define MEMACCESS(s) MEM->s

/* VIRTUAL HARDWARE ACCESS */
#define RSP       MEMACCESS(SP)
#define RHP       MEMACCESS(HP)
#define RHPO      MEMACCESS(HPOFFSET)
#define RDATA     MEMACCESS(DATA)

/* STACK ISA */
#define PUSH_U8(u8, ...)        RDATA[RSP++] = (char)(u8)
#define POP_U8(...)             RDATA[--RSP]

/* HEAP ISA */
#define SBRK(n)                 (RHP = RHP - (n), RHP)
#define STORE(u8, rpc_addr)     RDATA[(rpc_addr) + RHPO] = (u8)
#define LOAD(rpc_addr, cpp_var) (cpp_var) = RDATA[(rpc_addr) + RHPO]

/* EVIL CPP HACKING */ 
#define CAT(a, b) a##b
#define CAT3(a, b, c) CAT3_HELPER(a, b, c)
#define CAT3_HELPER(a, b, c) a##b##c

#define CAR(a, ...) a
#define CDR(_, ...) __VA_ARGS__
#define CAAR(_, b, ...) b
#define CADR(_, __, ...) __VA_ARGS__

#define PROBE() ~, 1

#define IS_RETURN_PROBE(...) CAAR(__VA_ARGS__, return)
#define NOT_RETURN(x) IS_RETURN_PROBE(CAT(_NOT_, x))
#define _NOT_return PROBE()

#define IS_ARRAY_PROBE(...) CAAR(__VA_ARGS__, ARRAY)
#define NOT_ARRAY(x) IS_ARRAY_PROBE(CAT(_NOT_, x)) 
#define _NOT_ARRAY PROBE()

#define IF_RETURN(condition) _IF_ELSE(NOT_RETURN(condition))
#define IF_ARRAY(condition) _IF_ELSE(NOT_ARRAY(condition))
#define _IF_ELSE(condition) CAT(_IF_, condition)

#define _IF_1(...) __VA_ARGS__ _IF_1_ELSE
#define _IF_1_ELSE(...)

#define _IF_return(...) _IF_return_ELSE
#define _IF_return_ELSE(...) __VA_ARGS__

#define _IF_ARRAY(...) _IF_ARRAY_ELSE
#define _IF_ARRAY_ELSE(...) __VA_ARGS__

#define NUM_ARGS_IMPL(_1, _2, _3, N, ...) N
#define NUM_ARGS(...) NUM_ARGS_IMPL(__VA_ARGS__, 3, 2, 1, 0)

#define COUNT_2(kind, ...) CAT(kind, __ARRAY)(__VA_ARGS__)
#define COUNT_1(kind, ...) CAT(kind, __ONE)(__VA_ARGS__)

#define DEFER1(m) m EMPTY()
#define EMPTY(...)

#define EVAL(...) EVAL1024(__VA_ARGS__)
#define EVAL1024(...) EVAL512(EVAL512(__VA_ARGS__))
#define EVAL512(...) EVAL256(EVAL256(__VA_ARGS__))
#define EVAL256(...) EVAL128(EVAL128(__VA_ARGS__))
#define EVAL128(...) EVAL64(EVAL64(__VA_ARGS__))
#define EVAL64(...) EVAL32(EVAL32(__VA_ARGS__))
#define EVAL32(...) EVAL16(EVAL16(__VA_ARGS__))
#define EVAL16(...) EVAL8(EVAL8(__VA_ARGS__))
#define EVAL8(...) EVAL4(EVAL4(__VA_ARGS__))
#define EVAL4(...) EVAL2(EVAL2(__VA_ARGS__))
#define EVAL2(...) EVAL1(EVAL1(__VA_ARGS__))
#define EVAL1(...) __VA_ARGS__

/* CALLING ROUTINES */

#define PUSH__ONE(type, a) CAT(PUSH_, type)(a, MEM)
#define POP__ONE(type, a)               \
  IF_RETURN(a)                     \
    (return CAT(POP_, type) (MEM)) \
     (a = CAT(POP_, type) (MEM))


#define FORLOOP(cmd, n) printf("hello"); cmd;

#define POP__ARRAY(type, a, n)    \
  for (int i = (n); i > 0; i++) { \
    POP__ONE(type, a[i - 1]);     \
  } 

#define PUSH__ARRAY(type, a, n)   \
  for (int i = 0; i < (n); i++) { \
    PUSH__ONE(type, a[i]);        \
  } 

/* SPECIAL ARRAY CASE HANDLER */

#define NEL(arr) (sizeof(arr) / sizeof(*arr))

#define PUSH(type, ...) _PUSH_(type, NUM_ARGS(__VA_ARGS__), __VA_ARGS__)
#define _PUSH_(type, count, ...) CAT(COUNT_, count)(PUSH, type, __VA_ARGS__)

#define POP(type, ...) _POP_(type, NUM_ARGS(__VA_ARGS__), __VA_ARGS__)
#define _POP_(type, count, ...) CAT(COUNT_, count)(POP, type, __VA_ARGS__)


/* DEFINE A CUSTOM ROUTINE */ 

#define PUSHDEF(type, ...)  inline void CAT(PUSH_,type)(__VA_ARGS__, rpcmem_t MEM)
#define POPDEF(type, rtcty) inline rtcty CAT(POP_,type)(rpcmem_t MEM)

/* REINTERPRET BITS (not a cast) */

#define AS(ctype, var) (*(ctype*)&var)


#endif
