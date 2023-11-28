#ifndef BASIS_H
#define BASIS_H

#include "common/debugmacros.h"
#include "mem.h"
#include <string.h>
#include <string>

using namespace std;

//
// Declaration of basic packing and unpacking functions for RPC communication.
// The basis covers all interactions with primitive IDL types and the shared
// memory unit.
//

// Helper function  to ensure there is enough room in the RPC memory
// for the specified number of bytes
static inline void __make_room_for(__rpcmem_t *m, int n_bytes) {
    // doesn't matter if we are adding to stack or heap
    while (m->hp + n_bytes > m->sp)
        rpcmem_expand(m);
}

// PACK
//  These functions pre-decrement the stack pointer and pack data into
// the RPC memory block.

// Pack an integer into the RPC memory
static void __pack_int(unsigned x, __rpcmem_t *m) {
    __make_room_for(m, sizeof(x));
    for (unsigned i = 0; i < sizeof(x); i++)
        m->data[--m->sp] = (unsigned char)(x >> 8 * i);
}

// Pack a boolean into the RPC memory
static void __pack_bool(bool b, __rpcmem_t *m) {
    __make_room_for(m, 1);
    m->data[--m->sp] = (char)b;
}

// Pack an RPC pointer into the RPC memory
static void __pack_rpcptr(__rpcptr_t x, __rpcmem_t *m) { __pack_int(x, m); }

// Pack a floating-point number into the RPC memory
static void __pack_float(float f, __rpcmem_t *m) {
    __pack_int(*(unsigned *)&f, m);
}

// Pack a string into the RPC memory
static void __pack_string(string s, __rpcmem_t *m) {
    int strlen = (int)s.length() + 1; // Includes null byte
    const char *strdata = s.c_str();
    __rpcptr_t strptr = m->hp; // Place strdata at current hp
                               //
    __make_room_for(m, strlen + sizeof(strlen) + sizeof(strptr));

    __pack_int(strlen, m);    // Pack the length of the string
    __pack_rpcptr(strptr, m); // Pack the pointer to the string's heap data
    strncpy(m->data + strptr, strdata, strlen); // Place the string at strptr
    m->hp += strlen; // Move hp forward towards after strdata
}

// UNPACK
// These functions post-increment the stack pointer and unpack data from the RPC
// memory

// Unpack an integer from the RPC memory
static int __unpack_int(__rpcmem_t *m) {
    unsigned char bytes[4];
    for (int i = 4; i > 0; i--)
        bytes[i - 1] = m->data[m->sp++];
    return bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
}

// Unpack a boolean from the RPC memory
static bool __unpack_bool(__rpcmem_t *m) { return m->data[m->sp++]; }

// Unpack an RPC pointer from the RPC memory
static __rpcptr_t __unpack_rpcptr(__rpcmem_t *m) { return __unpack_int(m); }

// Unpack a floating-point number from the RPC memory
static float __unpack_float(__rpcmem_t *m) {
    int float_bytes_as_int = __unpack_int(m);
    return *(float *)&float_bytes_as_int;
}

// Unpack a string from the RPC memory
static string __unpack_string(__rpcmem_t *m) {
    __rpcptr_t strptr = __unpack_rpcptr(m);
    int strlen = __unpack_int(m);
    return string(m->data + strptr, strlen - 1); // doesn't want nullterm
}

#endif // BASIS_H
