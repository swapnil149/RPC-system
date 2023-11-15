#ifndef BASIS_H
#define BASIS_H

#include "mem.h"
#include <string>

using namespace std;

//
// PACK
//

static void pack_int(unsigned x, rpcmem_t *m) {
  unsigned char bytes[4] = {
      (unsigned char)x,
      (unsigned char)(x >> 8),
      (unsigned char)(x >> 16),
      (unsigned char)(x >> 24),
  };
  for (int i = 0; i < 4; i++)
    m->data[m->sp--] = bytes[i];
}

static void pack_bool(bool b, rpcmem_t *m) { m->data[m->sp--] = (char)b; }

static void pack_rpcptr(rpcptr_t x, rpcmem_t *m) { pack_int(x, m); }

static void pack_float(float f, rpcmem_t *m) { pack_int(*(unsigned *)&f, m); }

static void pack_string(string s, rpcmem_t m) {
  (void)s; // TODO
  (void)m;
}

//
// UNPACK
//

static int unpack_int(rpcmem_t *m) {
  unsigned char bytes[4];
  for (int i = 4 - 1; i >= 0; i--)
    bytes[i] = m->data[++m->sp];
  return bytes[0] | bytes[1] << 8 | bytes[2] << 16 | bytes[3] << 24;
}

static bool unpack_bool(rpcmem_t *m) { return m->data[++m->sp]; }

static rpcptr_t unpack_rpcptr(rpcmem_t *m) { return unpack_int(m); }

static float unpack_float(rpcmem_t *m) {
  int x = unpack_int(m);
  return *(float *)&x;
}

static string unpack_string(rpcmem_t *m) {
  string result;
  (void)m; // TODO
  return result;
}

#endif
