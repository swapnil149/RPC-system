#ifndef BASIS_H
#define BASIS_H

#include "../debugmacros.h"
#include "mem.h"
#include <cstring>
#include <string>

using namespace std;

//
// PACK
//

static void __pack_int(unsigned x, __rpcmem_t *m) {
    unsigned char bytes[4] = {
        (unsigned char)x,
        (unsigned char)(x >> 8),
        (unsigned char)(x >> 16),
        (unsigned char)(x >> 24),
    };
    for (int i = 0; i < 4; i++)
        m->data[m->sp--] = bytes[i];
}

static void __pack_bool(bool b, __rpcmem_t *m) { m->data[m->sp--] = (char)b; }

static void __pack_rpcptr(__rpcptr_t x, __rpcmem_t *m) { __pack_int(x, m); }

static void __pack_float(float f, __rpcmem_t *m) {
    __pack_int(*(unsigned *)&f, m);
}

static void __pack_string(string s, __rpcmem_t *m) {
    __rpcptr_t str_ptr = m->hp; // Get the current heap pointer
    DEBUG("%d", str_ptr);
    DEBUG("%d", (int)s.length());
    __pack_rpcptr(str_ptr, m); // Pack the pointer to the string on the heap
    __pack_int((int)s.length(), m); // Pack the length of the string
    for (char c : s) {
        m->data[m->hp++] = c; // Pack each character to the heap
    }
    m->data[m->hp++] = '\0';
}

//
// UNPACK
//

static int __unpack_int(__rpcmem_t *m) {
    unsigned char bytes[4];
    for (int i = 4 - 1; i >= 0; i--)
        bytes[i] = m->data[++m->sp];
    return bytes[0] | bytes[1] << 8 | bytes[2] << 16 | bytes[3] << 24;
}

static bool __unpack_bool(__rpcmem_t *m) { return m->data[++m->sp]; }

static __rpcptr_t __unpack_rpcptr(__rpcmem_t *m) { return __unpack_int(m); }

static float __unpack_float(__rpcmem_t *m) {
    int x = __unpack_int(m);
    return *(float *)&x;
}

static string __unpack_string(__rpcmem_t *m) {
    // Unpack the pointer to the string on the heap
    int len = __unpack_int(m); // Unpack the length of the string
    __rpcptr_t str_ptr = __unpack_rpcptr(m);
    DEBUG("%d", len);
    DEBUG("%d", str_ptr);
    string result = "";
    for (int i = 0; i < len; ++i) {
        result += m->data[str_ptr + i]; // Unpack each character from the heap
    }
    result += '\0';
    return result;
}

#endif
