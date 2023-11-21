#ifndef TRANSMIT_H
#define TRANSMIT_H

#include "basis.h"
#include <cstdio>
#include <string>
using namespace std;

#ifndef RPCSOCKET // just a placeholder to remove warnings
#define RPCSOCKET dummysock
static struct {
    void (*write)(const char *, int);
    void (*read)(char *, int);
} *dummysock;
#endif

#include "basis.h"
#include "mem.h"
#include <stdio.h>

static void rpc_send(string fname, __rpcmem_t *mem) {
    __pack_string(fname, mem);
    int mem_size = mem->hp + (mem->capacity - mem->sp);
    // TODO: don't rely on endianness
    RPCSOCKET->write((char *)&mem_size, sizeof(mem_size));
    RPCSOCKET->write((char *)&mem->hp, sizeof(mem->hp));
    RPCSOCKET->write(mem->data, mem->hp);
    RPCSOCKET->write(mem->data + mem->sp, mem->capacity - mem->sp);
}

static string rpc_recv(__rpcmem_t *mem) {
    int mem_size;
    RPCSOCKET->read((char *)&mem_size, sizeof(mem_size));
    RPCSOCKET->read((char *)&mem->sp, sizeof(mem->sp));
    RPCSOCKET->read(mem->data, mem_size);
    return __unpack_string(mem);
}

#endif
