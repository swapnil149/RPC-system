#ifndef DEBUGMACROS_H
#define DEBUGMACROS_H
#include <stdio.h>

#define DEBUG(fmt, var) fprintf(stderr, #var ": " fmt "\n", var)

#endif
