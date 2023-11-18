#include <stdio.h>
#include <string>
using namespace std;
#include "person.idl"
#include "rpcproxyhelper.h"
#define RPCSOCKET RPCPROXYSOCKET
#ifndef MEM_H
#define MEM_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY (1 << 16)

typedef uint32_t __rpcptr_t;

struct __rpcmem_t {
  unsigned char *data;
  __rpcptr_t hp;       // heap ptr
  __rpcptr_t sp;       // stack ptr
  __rpcptr_t capacity; // internal use only
};

static __rpcmem_t *rpcmem_new() {
  __rpcmem_t *m = (__rpcmem_t *)malloc(sizeof(*m));
  m->hp = 0;
  m->sp = m->capacity = INITIAL_CAPACITY;
  m->data = (unsigned char *)malloc(m->capacity);
  return m;
}

static void rpcmem_free(__rpcmem_t **m) {
  free((*m)->data);
  free(*m);
  *m = NULL;
}

static void rpcmem_expand(__rpcmem_t *m) {
  int stacksize = m->capacity - m->sp;
  m->capacity *= 2;
  m->data = (unsigned char *)realloc(m->data, m->capacity);
  memmove(m->data + m->capacity - stacksize, m->data + m->sp, stacksize);
}

#endif

#ifndef BASIS_H
#define BASIS_H

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
  __pack_int(s.length(), m); // Pack the length of the string
  __rpcptr_t strPtr = m->hp; // Get the current heap pointer
  for (char c : s) {
    m->data[m->hp++] = c; // Pack each character to the heap
  }
  __pack_rpcptr(strPtr, m); // Pack the pointer to the string on the heap
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
  __rpcptr_t strPtr =
      __unpack_rpcptr(m);    // Unpack the pointer to the string on the heap
  int len = __unpack_int(m); // Unpack the length of the string
  string result = "";
  for (int i = 0; i < len; ++i) {
    result += m->data[strPtr + i]; // Unpack each character from the heap
  }
  return result;
}

#endif

#ifndef TRANSMIT_H
#define TRANSMIT_H

#include <cstdio>
#include <string>
using namespace std;

#ifndef RPCSOCKET // just a placeholder to remove warnings
#define RPCSOCKET dummysock
static struct {
  void (*write)(const void *, int);
  void (*read)(void *, int);
} *dummysock;
#endif

#include <stdio.h>

static void rpc_send(string fname, __rpcmem_t *mem) {
  __pack_string(fname, mem);
  int mem_size = mem->hp + (mem->capacity - mem->sp);
  RPCSOCKET->write(&mem_size,
                   sizeof(mem_size));          // TODO: don't rely on endianness
  RPCSOCKET->write(&mem->hp, sizeof(mem->hp)); // TODO: don't rely on endianness
  RPCSOCKET->write(mem->data, mem->hp);
  RPCSOCKET->write(mem->data + mem->sp, mem->capacity - mem->sp);
}

static string rpc_recv(__rpcmem_t *mem) {
  int mem_size;
  RPCSOCKET->read(&mem_size, sizeof(mem_size));
  RPCSOCKET->read(&mem->sp, sizeof(mem->sp));
  RPCSOCKET->read(mem->data, mem_size);
  return __unpack_string(mem);
}

#endif

void __pack_Person(Person bandhi, __rpcmem_t *mem) {
  __pack_string(bandhi.name, mem);
  __pack_int(bandhi.age, mem);
  for (int __i = 0; __i < 5; __i++) {
    __pack_int(bandhi.lucky_numbers[__i], mem);
  }
}

void __pack_People(People bandhi, __rpcmem_t *mem) {
  for (int __i = 0; __i < 4; __i++) {
    __pack_Person(bandhi.people[__i], mem);
  }
  for (int __i = 0; __i < 4; __i++) {
    __pack_string(bandhi.nicknames[__i], mem);
  }
}

Person __unpack_Person(__rpcmem_t *mem) {
  Person result;
  for (int __i = 5; __i > 0; __i--) {
    result.lucky_numbers[__i - 1] = __unpack_int(mem);
  }
  result.age = __unpack_int(mem);
  result.name = __unpack_string(mem);
  return result;
}

People __unpack_People(__rpcmem_t *mem) {
  People result;
  for (int __i = 4; __i > 0; __i--) {
    result.nicknames[__i - 1] = __unpack_string(mem);
  }
  for (int __i = 4; __i > 0; __i--) {
    result.people[__i - 1] = __unpack_Person(mem);
  }
  return result;
}

float avg_4D(int nums[8][8][8][8]) {
  __rpcmem_t *mem = rpcmem_new();

  for (int __i = 0; __i < 8 * 8 * 8 * 8; __i++) {
    __pack_int(nums[0][0][0][__i], mem);
  }

  rpc_send("avg_4D", mem);
  rpc_recv(mem);

  float result = __unpack_float(mem);

  rpcmem_free(&mem);
  return result;
}

Person nth_person(People ppl, int n) {
  __rpcmem_t *mem = rpcmem_new();

  __pack_People(ppl, mem);
  __pack_int(n, mem);

  rpc_send("nth_person", mem);
  rpc_recv(mem);

  Person result = __unpack_Person(mem);

  rpcmem_free(&mem);
  return result;
}
