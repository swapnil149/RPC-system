#ifndef TRANSMIT_H
#define TRANSMIT_H

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

#include "../debugmacros.h"
#include "basis.h"
#include "mem.h"
#include <stdio.h>
#include <stdlib.h>

static void rpc_send(string fname, __rpcmem_t *mem) {
    ERP("STARTING RPC SEND: %s\n <-- FNAME", fname.c_str());
    MADEIT;
    __pack_string(fname, mem);
    MADEIT;
    int mem_size = mem->hp + (mem->capacity - mem->sp);

    DEBUG("%d", mem_size);
    DEBUGMEM(mem);

    // TODO: don't rely on endianness
    RPCSOCKET->write((char *)&mem_size, sizeof(mem_size));
    RPCSOCKET->write((char *)&mem->hp, sizeof(mem->hp));
    RPCSOCKET->write(mem->data, mem->hp);
    RPCSOCKET->write(mem->data + mem->sp, mem->capacity - mem->sp);
    ERP("FINISHED SEND\n");
}

static string rpc_recv(__rpcmem_t *mem) {
    ERP("STARTING RPC RECEIVE\n");
    int mem_size;
    RPCSOCKET->read((char *)&mem_size, sizeof(mem_size));
    RPCSOCKET->read((char *)&mem->sp, sizeof(mem->sp));
    RPCSOCKET->read(mem->data, mem_size);
    mem->hp = 0;

    DEBUG("%d", mem_size);
    DEBUGMEM(mem);

    string fname = __unpack_string(mem);
    ERP("FINISHING RPC RECEIVE %s\n", fname.c_str());
    return fname;
}

#endif
