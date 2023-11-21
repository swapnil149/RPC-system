#ifndef DEBUGMACROS_H
#define DEBUGMACROS_H
#include <stdio.h>

#define DEBUG(fmt, var) fprintf(stderr, #var ": " fmt "\n", var)

#define MADEIT fprintf(stderr, "Made it to %s in %s\n", __LINE__, __FILE__)

#endif
