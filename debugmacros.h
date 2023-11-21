#ifndef DEBUGMACROS_H
#define DEBUGMACROS_H
#include <stdio.h>

#define DEBUG(fmt, var) fprintf(stdout, #var ": " fmt "\n", var)
#define ERP(...) fprintf(stdout, __VA_ARGS__)
#define MADEIT fprintf(stdout, "Made it to %d in %s\n", __LINE__, __FILE__)

#endif
