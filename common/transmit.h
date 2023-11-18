#ifndef TRANSMIT_H
#define TRANSMIT_H

#include "basis.h"
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

// #include "c150debug.h"
// #include "c150streamsocket.h"
#include "basis.h"
#include "mem.h"
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
