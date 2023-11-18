#include "basis.h"
#include <iostream>

//                 "<fname> <stackptr & heapptr>"
#define PREFIX_FMT "%s %08x"

#define MAX_FNAME_LEN 80
#define MAX_PREFIX_LEN (MAX_FNAME_LEN + 20) // rough estimate

static int rpcmem_tobuf(const char *fname, const __rpcmem_t *m, char **outbuf) {
  int prefixlen = snprintf(NULL, 0, PREFIX_FMT, fname, m->hp);
  assert(prefixlen < MAX_PREFIX_LEN && prefixlen);
  *outbuf = (char *)malloc(prefixlen + m->hp + (m->capacity - m->sp));
  snprintf(*outbuf, MAX_PREFIX_LEN, PREFIX_FMT, fname, m->hp);
  memcpy(*outbuf + prefixlen, m->data, m->hp);
  memcpy(*outbuf + prefixlen + m->hp, m->data + m->sp, m->capacity - m->sp);
  return prefixlen + m->hp + (m->capacity - m->sp);
}

// clang-format on

static void rpcmem_frombuf(const char *inbuf, int len,
                           char fname[MAX_PREFIX_LEN], __rpcmem_t *m) {
  int stackptr, prefixlen;
  sscanf(inbuf, PREFIX_FMT "%n", fname, &stackptr, &prefixlen);
  m->sp = stackptr;
  m->hp = 0;
  memcpy(m->data, inbuf + prefixlen, len - prefixlen);
}

#undef PREFIX_FMT
#undef MAX_PREFIX_LEN

int main() {
  // Create a sample data structure
  __rpcmem_t *mem = rpcmem_new();
  string originalString = "Hello, World!";
  int originalInt = 42;
  float originalFloat = 3.14;
  bool originalBool = true;

  // Pack the data
  __pack_int(originalInt, mem);
  __pack_float(originalFloat, mem);
  __pack_string(originalString, mem);
  __pack_bool(originalBool, mem);

  // Display the packed data (for demonstration purposes)
  // cout << "Packed Data: ";
  // for (rpcptr_t i = mem->hp; i < mem->capacity; ++i) {
  //     cout << static_cast<int>(mem->data[i]) << " ";
  // }
  // cout << endl;

  // Unpack the data
  bool unpackedBool = __unpack_bool(mem);
  string unpackedString = __unpack_string(mem);
  float unpackedFloat = __unpack_float(mem);
  int unpackedInt = __unpack_int(mem);

  // Display the unpacked data
  cout << "Unpacked String: " << unpackedString << endl;
  cout << "Unpacked Int: " << unpackedInt << endl;
  cout << "Unpacked Float: " << unpackedFloat << endl;
  cout << "Unpacked Bool: " << unpackedBool << endl;

  // Clean up
  rpcmem_free(&mem);

  return 0;
}
