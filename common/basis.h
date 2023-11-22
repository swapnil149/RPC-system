#ifndef BASIS_H
#define BASIS_H

#include "../debugmacros.h"
#include "mem.h"
#include <string.h>
#include <string>

using namespace std;

//
// PACK
//
// pre-decrement the stack pointer
//

static void __pack_int(unsigned x, __rpcmem_t *m) {
    for (int i = 0; i < 4; i++)
        m->data[--m->sp] = (unsigned char)(x >> 8 * i);
}

static void __pack_bool(bool b, __rpcmem_t *m) { m->data[--m->sp] = (char)b; }

static void __pack_rpcptr(__rpcptr_t x, __rpcmem_t *m) { __pack_int(x, m); }

static void __pack_float(float f, __rpcmem_t *m) {
    __pack_int(*(unsigned *)&f, m);
}

static void __pack_string(string s, __rpcmem_t *m) {
    __rpcptr_t strptr = m->hp; // Get the current heap pointer
    int strlen = (int)s.length() + 1;
    const char *strdata = s.c_str();
    __pack_int(strlen, m);    // Pack the length of the string
    __pack_rpcptr(strptr, m); // Pack the pointer to the string on the heap
    strncpy(m->data + m->hp, strdata, strlen);
    m->hp += strlen;
}

//
// UNPACK
//
// post-increment the stack pointer
//

static int __unpack_int(__rpcmem_t *m) {
    unsigned char bytes[4];
    for (int i = 4; i > 0; i--)
        bytes[i - 1] = m->data[m->sp++];
    return bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
}

static bool __unpack_bool(__rpcmem_t *m) { return m->data[m->sp++]; }

static __rpcptr_t __unpack_rpcptr(__rpcmem_t *m) { return __unpack_int(m); }

static float __unpack_float(__rpcmem_t *m) {
    int x = __unpack_int(m);
    return *(float *)&x;
}

static string __unpack_string(__rpcmem_t *m) {
    __rpcptr_t strptr = __unpack_rpcptr(m);
    int strlen = __unpack_int(m);
    char *strdata = (char *)malloc(strlen);
    strncpy(strdata, m->data + strptr, strlen);
    string result = string(strdata);
    free(strdata);
    return result;
}

#endif
