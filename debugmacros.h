#ifndef DEBUGMACROS_H
#define DEBUGMACROS_H
#include <stdio.h>

#define DEBUG(fmt, var) fprintf(stderr, #var ": " fmt "\n", var)
#define ERP(...) fprintf(stderr, __VA_ARGS__)
#define MADEIT fprintf(stderr, "Made it to %d in %s\n", __LINE__, __FILE__)
#define DEBUGMEM(m)                                                            \
    fprintf(stderr, "HEAP: %d, STACK: %d, SIZE: %d at %d::%s\n", m->hp, m->sp, \
            m->hp + (m->capacity - m->sp), __LINE__, __FILE__)
#endif
