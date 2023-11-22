#ifndef TRANSMIT_H
#define TRANSMIT_H

#include <cstdio>
#include <string>
using namespace std;

#ifndef RPCSOCKET // just a placeholder to remove warnings
#define RPCSOCKET dummysock
static struct {
    void (*write)(const char *, int);
    ssize_t (*read)(char *, int);
} *dummysock;
#endif

#include "../debugmacros.h"
#include "basis.h"
#include "mem.h"
#include <stdio.h>
#include <stdlib.h>

static void rpc_send(string fname, __rpcmem_t *mem) {
    __pack_string(fname, mem);
    int stack_size = mem->capacity - mem->sp;
    int mem_size = mem->hp + stack_size;
    RPCSOCKET->write((char *)&mem_size, sizeof(mem_size));
    RPCSOCKET->write((char *)&mem->hp, sizeof(mem->hp));
    RPCSOCKET->write(mem->data, mem->hp);
    RPCSOCKET->write(mem->data + mem->sp, stack_size);
}

static inline void force_read(char *buf, int len) {
    for (int n_read = 0; n_read < len;)
        n_read += RPCSOCKET->read(buf + n_read, len - n_read);
}

static string rpc_recv(__rpcmem_t *mem) {
    int mem_size;
    force_read((char *)&mem_size, sizeof(mem_size));
    force_read((char *)&mem->hp, sizeof(mem->hp));
    force_read(mem->data, mem_size);
    mem->sp = mem->hp;
    string fname = __unpack_string(mem);
    return fname;
}

#endif
