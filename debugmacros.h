#ifndef DEBUG_MACROS_H
#define DEBUG_MACROS_H

#define DEBUGINT(x) fprintf(stderr, #x ": %d\n", x)
#define DEBUGBOOL(x) fprintf(stderr, #x ": %s\n", x ? "true" : "false")
#define DEBUGPTR(x) fprintf(stderr, #x ": 0x%04x\n", x)

#define MADEIT fprintf(stderr, "%s: made it to line %d\n", __FILE__, __LINE__)

#define DEBUG(fmt, x) fprintf(stderr, #x ": " fmt, x)

extern void exit();
#define ASSERT(condition)                                                      \
  if (!(condition)) {                                                          \
    fprintf(stderr, "assertion failed in %s at line %d: " #condition,          \
            __FILE__, __LINE__);                                               \
    exit(1);                                                                   \
  }

#endif
