#ifndef CPP_HACKING_H
#define CPP_HACKING_H

/* NAUGHTY CPP HACKING
 *
 * Things get pretty ugly in here — but they work.
 * Our best excuse is that you don't get mad at gcc for it's internal spagetti.
 *
 * Recommended reading:
 * http://jhnet.co.uk/articles/cpp_magic
 *
 * Also principle of least power, amirite hehe? :D
 * */

#define CAT(a, b) _CAT_(a, b)
#define _CAT_(a, b) a##b

#define _STRINGIFY_(x) #x
#define STRINGIFY(x) _STRINGIFY_(x)

#define CAR(a, ...) a
#define CDR(_, ...) __VA_ARGS__
#define CAAR(_, b, ...) b
#define CADR(_, __, ...) __VA_ARGS__

#define BOOL(x) NOT(NOT(x))
#define _NOT_0 PROBE()

#define PROBE() ~, 1

#define IS_RETURN_PROBE(...) CAAR(__VA_ARGS__, return)
#define NOT_RETURN(x) IS_RETURN_PROBE(CAT(_NOT_, x))
#define _NOT_return PROBE()

#define IF_ELSE(condition) _IF_ELSE(BOOL(condition))
#define IF_RETURN(condition) _IF_ELSE(NOT_RETURN(condition))
#define _IF_ELSE(condition) CAT(_IF_, condition)

#define _IF_1(...) __VA_ARGS__ _IF_1_ELSE
#define _IF_1_ELSE(...)

#define _IF_return(...) _IF_return_ELSE
#define _IF_return_ELSE(...) __VA_ARGS__

#define _IF_ARRAY(...) _IF_ARRAY_ELSE
#define _IF_ARRAY_ELSE(...) __VA_ARGS__

// can externally define this macro to support more numbers
#ifndef NUM_ARGS_IMPL
#define NUM_ARGS_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13,  \
                      _14, _15, _16, _17, _18, _19, _20, N, ...)               \
  N
#endif

#ifndef NUM_ARGS
#define NUM_ARGS(...)                                                          \
  NUM_ARGS_IMPL(__VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, \
                7, 6, 5, 4, 3, 2, 1, 0)
#endif

#define EMPTY()

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

#define DEFER2(m) m EMPTY EMPTY()()

#define IS_PROBE(...) CAAR(__VA_ARGS__, 0)
#define PROBE() ~, 1

#define NOT(x) IS_PROBE(CAT(_NOT_, x))
#define _NOT_0 PROBE()

#define BOOL(x) NOT(NOT(x))

#define IF_ELSE(condition) _IF_ELSE(BOOL(condition))
#define _IF_ELSE(condition) CAT(_IF_, condition)

#define _IF_1(...) __VA_ARGS__ _IF_1_ELSE
#define _IF_0(...) _IF_0_ELSE

#define _IF_0_ELSE(...) __VA_ARGS__

#define HAS_ARGS(...) BOOL(CAR(_END_OF_ARGUMENTS_ __VA_ARGS__)())
#define _END_OF_ARGUMENTS_() 0

#define MAP(m, first, ...)                                                     \
  m(first) IF_ELSE(HAS_ARGS(__VA_ARGS__))(                                     \
      DEFER2(_MAP)()(m, __VA_ARGS__))(/* Do nothing, just terminate */         \
  )
#define _MAP() MAP

#endif
