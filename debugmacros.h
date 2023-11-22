#ifndef DEBUGMACROS_H
#define DEBUGMACROS_H
#include <stdio.h>

#define DEBUG(fmt, var)                                                        \
    fprintf(stderr, "%d::%s " #var ": " fmt "\n", __LINE__, __FILE__, var)
#define ERP(...) fprintf(stderr, __VA_ARGS__)
#define MADEIT fprintf(stderr, "%d::%s\n", __LINE__, __FILE__)
#define DEBUGMEM(m)                                                            \
    fprintf(stderr, "%d::%s HEAP: %d, STACK: %d, SIZE: %d\n", __LINE__,        \
            __FILE__, m->hp, m->sp, m->hp + ((m->capacity) - m->sp))
#endif
