#ifndef BASIS_H
#define BASIS_H

#include "mem.h"
#include <string>

using namespace std;

//
// PACK
//

static void __pack_int(unsigned x, rpcmem_t *m) {
  unsigned char bytes[4] = {
      (unsigned char)x,
      (unsigned char)(x >> 8),
      (unsigned char)(x >> 16),
      (unsigned char)(x >> 24),
  };
  for (int i = 0; i < 4; i++)
    m->data[m->sp--] = bytes[i];
}

static void __pack_bool(bool b, rpcmem_t *m) { m->data[m->sp--] = (char)b; }

static void __pack_rpcptr(rpcptr_t x, rpcmem_t *m) { __pack_int(x, m); }

static void __pack_float(float f, rpcmem_t *m) {
  __pack_int(*(unsigned *)&f, m);
}

static void __pack_string(string s, rpcmem_t *m) {
  __pack_int(s.length(), m); // Pack the length of the string
  rpcptr_t strPtr = m->hp;   // Get the current heap pointer
  for (char c : s) {
    m->data[m->hp++] = c; // Pack each character to the heap
  }
  __pack_rpcptr(strPtr, m); // Pack the pointer to the string on the heap
}
//
// UNPACK
//

static int __unpack_int(rpcmem_t *m) {
  unsigned char bytes[4];
  for (int i = 4 - 1; i >= 0; i--)
    bytes[i] = m->data[++m->sp];
  return bytes[0] | bytes[1] << 8 | bytes[2] << 16 | bytes[3] << 24;
}

static bool __unpack_bool(rpcmem_t *m) { return m->data[++m->sp]; }

static rpcptr_t __unpack_rpcptr(rpcmem_t *m) { return __unpack_int(m); }

static float __unpack_float(rpcmem_t *m) {
  int x = __unpack_int(m);
  return *(float *)&x;
}

static string __unpack_string(rpcmem_t *m) {
  rpcptr_t strPtr =
      __unpack_rpcptr(m);    // Unpack the pointer to the string on the heap
  int len = __unpack_int(m); // Unpack the length of the string
  string result = "";
  for (int i = 0; i < len; ++i) {
    result += m->data[strPtr + i]; // Unpack each character from the heap
  }
  return result;
}

#endif
