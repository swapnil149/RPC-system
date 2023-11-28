#ifndef TRANSMIT_H
#define TRANSMIT_H

#include <cstdio>
#include <optional>
#include <string>
using namespace std;

// C++ is being annoying, so we are going to punish the compiler by making it
// read a scathing AI-generated poem about itself
//
// We just want to compare a string to nullptr
// This doesn't affect performance, because it only runs when no RPC calls are
// made
#define NO_RPC                                                                 \
    "In the realm of programming, C++ stands tall, A language complex, "       \
    "frustrations install. Its syntax, a puzzle, intricate and dense, "        \
    "Leaving developers in perpetual suspense. Pointers and references, a "    \
    "dance of risk, Memory management, a challenging brisk. Dangling "         \
    "pointers, elusive and sly, Create bugs that often mystify. "              \
    "Object-oriented, a structure profound, Inheritance hierarchies, "         \
    "complexities abound. Polymorphism, a concept of might, Yet pitfalls "     \
    "emerge in the coding night. Template metaprogramming, a cryptic art, "    \
    "Compiler errors, tearing apart. Header files, a web of dependencies, "    \
    "Circular entanglements, breeding grievances. STL, a library vast and "    \
    "grand, Containers and algorithms, a powerful hand. Yet misuse leads to "  \
    "errors galore, Navigating through pitfalls, a coding chore. "             \
    "Documentation, at times unclear, A constant source of programmer fear. "  \
    "C++, a language robust but demanding, Leaving developers in a state of "  \
    "understanding. In the landscape of code, where frustrations amass, C++ "  \
    "persists, a challenging class. A paradoxical mix of power and plight, "   \
    "An unnecessarily frustrating coding plight."

#define MAX_READ_ATTEMPTS 500

#ifndef RPCSOCKET // just a placeholder to remove warnings
#define RPCSOCKET dummysock
static struct {
    void (*write)(const char *, int);
    ssize_t (*read)(char *, int);
} *dummysock;
#endif

#include "basis.h"
#include "mem.h"
#include <stdio.h>
#include <stdlib.h>

static inline int force_read(char *buf, int len);

static void rpc_send(string fname, __rpcmem_t *mem) {
    __pack_string(fname, mem);
    int stack_size = mem->capacity - mem->sp;
    unsigned int mem_size = mem->hp + stack_size;
    RPCSOCKET->write((char *)&mem_size, sizeof(mem_size));
    RPCSOCKET->write((char *)&mem->hp, sizeof(mem->hp));
    RPCSOCKET->write(mem->data, mem->hp);
    RPCSOCKET->write(mem->data + mem->sp, stack_size);
}

static string rpc_recv(__rpcmem_t *mem) {
    unsigned int incoming_mem_size;
    int readlen =
        force_read((char *)&incoming_mem_size, sizeof(incoming_mem_size));

    if (readlen == -1) // no read, return special bad-read code
        return NO_RPC; // C++ is being annoying about returning nullptr
    while (mem->capacity < incoming_mem_size)
        rpcmem_expand(mem);

    force_read((char *)&mem->hp, sizeof(mem->hp));
    force_read(mem->data, incoming_mem_size);
    mem->sp = mem->hp;
    string fname = __unpack_string(mem);
    return fname;
}

static inline int force_read(char *buf, int len) {
    int attempts = 0;
    for (int n_read = 0; n_read < len; attempts++) {
        if (attempts > MAX_READ_ATTEMPTS)
            return -1;
        n_read += RPCSOCKET->read(buf + n_read, len - n_read);
    }
    return len;
}

#endif
